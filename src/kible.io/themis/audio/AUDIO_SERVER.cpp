#include "AUDIO_SERVER.h"

//KORD: Kevin, what in gods name is this shit
//KEVIN: It is a way to limit how long a function will run

pthread_mutex_t calculating = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done = PTHREAD_COND_INITIALIZER;

void *expensive_call(void* s)
{
        int oldtype;
	AUDIO_SERVER* server = (AUDIO_SERVER*) s;

        /* allow the thread to be killed at any time */
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

        /* ... calculations and expensive io here, for example:
         * infinitely loop
         */
        server->rc = snd_pcm_open(&server->handle, AUDIO_DEV, SND_PCM_STREAM_CAPTURE, 0);

        /* wake up the caller if we've completed in time */
        pthread_cond_signal(&done);
        return NULL;
}

/* note: this is not thread safe as it uses a global condition/mutex */
int do_or_timeout(struct timespec *max_wait, AUDIO_SERVER* server)
{
        struct timespec abs_time;
        pthread_t tid;
        int err;

        pthread_mutex_lock(&calculating);

        /* pthread cond_timedwait expects an absolute time to wait until */
        clock_gettime(CLOCK_REALTIME, &abs_time);
        abs_time.tv_sec += max_wait->tv_sec;
        abs_time.tv_nsec += max_wait->tv_nsec;

        pthread_create(&tid, NULL, expensive_call, server);

        /* pthread_cond_timedwait can return spuriously: this should
         * be in a loop for production code
         */
        err = pthread_cond_timedwait(&done, &calculating, &abs_time);

        if (err == ETIMEDOUT) {
		fprintf(stderr, "%s: calculation timed out\n", __func__);
		server->rc = -1;
	}

        if (!err)
                pthread_mutex_unlock(&calculating);

        return err;
}

bool Initialize_AUDIO_SERVER(AUDIO_SERVER* server) {
	if (server->Raw || server->Compressed || server-> PCM_bytes ||
		server->PCM_bytes_back || server->S || server->encoder ||
		server->thr) {
		return false;
	}

	int32_t dir = 0;
	int32_t rc = 0;

	//Create encoder state
	server->encoder = opus_encoder_create(
	AS_SAMPLE_RATE, AS_CHANNELS, AS_APPLICATION, &rc);
	if (rc < 0) {
		return false;
	}
	opus_encoder_ctl(server->encoder, OPUS_SET_BITRATE(AS_BITRATE));
	// opus_encoder_ctl(server->encoder, OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
	// opus_encoder_ctl(server->encoder, OPUS_SET_BITRATE(OPUS_AUTO));

	//Open soundcard
	#ifdef _WIN64

	server->s_rate = AS_SAMPLE_RATE;
	server->f_size = AS_FRAME_SIZE;

	server->client = new UDP_CLIENT;
	if (!Initialize_UDP_CLIENT(server->client)) {
		Delete_AUDIO_SERVER(server);
		return false;
	}
	#endif
	#ifdef __linux__

	server->s_rate			= AS_SAMPLE_RATE;
	server->f_size			= AS_FRAME_SIZE;
	server->p_size			= AS_PERIOD_SIZE;
	server->handle = NULL;
	unsigned int exact_rate = server->s_rate;

	//Buffers for Opus
	server->Raw				= new opus_int16[server->f_size * AS_CHANNELS];
	server->Compressed 		= 	new uint8_t[AS_MAX_FRAME_SIZE];
	server->PCM_bytes		= new uint8_t[server->f_size * AS_CHANNELS * 2];
	server->PCM_bytes_back	= new uint8_t[server->f_size * AS_CHANNELS * 2];

	memset(server->Raw, 0, server->f_size * AS_CHANNELS * 2);
	memset(server->Compressed, 0, AS_MAX_FRAME_SIZE);
	memset(server->PCM_bytes, 0, server->f_size * AS_CHANNELS * 2);
	memset(server->PCM_bytes_back, 0, server->f_size * AS_CHANNELS * 2);

	snd_pcm_hw_params_t*	params;
	snd_pcm_uframes_t 		periods = 2;

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 48000,
		.channels = 2
	};
	server->s = NULL;
	int ret = 1;
	int error;

	pa_buffer_attr attr;
	attr.maxlength = (uint32_t) -1;
	attr.tlength = (uint32_t) -1;
	attr.prebuf = (uint32_t) -1;
	attr.minreq = (uint32_t) -1;
	attr.fragsize = 1600;

	/* Create the recording stream */
	if (!(server->s = pa_simple_new(NULL, "themis", PA_STREAM_RECORD, NULL, "record", &ss, NULL, &attr, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		return false;
	}

	/*
	struct timespec max_wait;

        memset(&max_wait, 0, sizeof(max_wait));

        // wait at most 2 seconds
        max_wait.tv_sec = 1;
        do_or_timeout(&max_wait, server);
	if (server->rc < 0) {
		log_err(((const JSON_TYPE){
			{"message", "unable to open pcm device"},
			JSON_TYPE_END}));
		Delete_AUDIO_SERVER(server);

		return false;
	}

	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(server->handle, params);
	snd_pcm_hw_params_set_access(
	server->handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	snd_pcm_hw_params_set_format(server->handle, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(server->handle, params, AS_CHANNELS);
	snd_pcm_hw_params_set_rate_near(
		server->handle, params, &exact_rate, &dir);

	if (server->s_rate != exact_rate) {
		log_dbg(((const JSON_TYPE){
			{"message", "Hz is not supported by your hardware"},
			JSON_TYPE_END}));
		//log_dbg("The rate " + to_string(server->s_rate) + " Hz is not supported by your hardware.\n==> Using " + to_string(exact_rate) + " Hz instead.\n");
		// fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n==> Using %d Hz instead.\n", server->s_rate, exact_rate);
	}

	snd_pcm_hw_params_set_periods(
	server->handle, params, periods, dir);

	snd_pcm_hw_params_set_period_size_near(
	server->handle, params, &server->p_size, &dir);

	snd_pcm_uframes_t temp_size = (server->p_size * periods) >> 2;
	if (snd_pcm_hw_params_set_buffer_size_near(server->handle, params, &temp_size) < 0) {
		log_dbg(((const JSON_TYPE){
			{"message", "Error setting buffersize"},
			JSON_TYPE_END}));
		return false;
	}

	log_dbg(((const JSON_TYPE){
		{"message", "S_RATE:"},
		JSON_TYPE_END}));

	//Set Params
	rc = snd_pcm_hw_params(server->handle, params);
  	if (rc < 0) {
		log_err(((const JSON_TYPE){
			{"message", "unable to set hw parameters"},
			JSON_TYPE_END}));
		Delete_AUDIO_SERVER(server);
		return false;
	}

	if ((rc = snd_pcm_prepare(server->handle)) < 0) {
		log_err(((const JSON_TYPE){
			{"message", "unable to prepare hw parameters"},
			JSON_TYPE_END}));
		Delete_AUDIO_SERVER(server);
		return false;
	}

	if (!Initialize_C_ARRAY(&server->audio_arr, sizeof(AUDIO_UNIT))) {
		return false;
	}

	server->audio_count = 1;
	*/

	// snd_pcm_hw_params_get_format(params, (snd_pcm_format_t *)&exact_rate);

	// if (exact_rate <= 5) {
	// 	log_dbg("format is 16.\n");
	// 	server->format = 16;
	// } else if (exact_rate <= 9) {
	// 	log_dbg("format is 24.\n");
	// 	server->format = 24;
	// } else {
	// 	log_dbg("format is 32.\n");
	// 	server->format = 32;
	// }
	// snd_pcm_hw_params_get_rate(params, &server->s_rate, NULL);
	// snd_pcm_hw_params_get_period_size(params, &server->p_size, NULL);
	// server->f_size = server->p_size / ((server->format / 8) * AS_CHANNELS);

#endif

	return true;
}

void Send_Buffer(AUDIO_SERVER* server) {
	//AUDIO_UNIT unit;

	int send_size;

	send_size = opus_encode(server->encoder, (opus_int16*)server->PCM_bytes_back,
		AS_FRAME_SIZE, (unsigned char*) server->Compressed + 8,
		AS_MAX_FRAME_SIZE);

	if (send_size < 0) {
		return;
	}

	((int*)server->Compressed)[0] = server->audio_count++;
	((int*)server->Compressed)[1] = send_size;
	send_size += 8;

	/*
	if (unit.buffer_size < 0 || unit.buffer_size >= MAX_OPUS_OUTPUT_SIZE) {
		log_err(((const JSON_TYPE){
			{"message", "encode failed"},
			JSON_TYPE_END}));
		return;
	}
	memcpy(unit.buffer, server->temp_buffer, unit.buffer_size);

	unit.event_index = server->audio_count++;
	Put_C_ARRAY(&server->audio_arr, (uint8_t *) &unit);
	*/

	//Send_UDP_SERVER(server->S, Get_C_ARRAY(&server->audio_arr),
	//	sizeof(AUDIO_UNIT) * C_ARRAY_QUEUE_SIZE);

	Send_SERVER(server->S, (char*)&send_size, sizeof(send_size));
	Send_SERVER(server->S, (char*)server->Compressed, send_size);


	/*
	if (!server->S->Send((char*)&send_size, sizeof(int32_t))) {
		log_err(((const JSON_TYPE){
			{"message", "failed to send size"},
			JSON_TYPE_END}));
		server->running = false;
		return;
	}

	if (!server->S->Send((char*)server->Compressed, send_size)) {
		log_err(((const JSON_TYPE){
			{"message", "failed to send buffer"},
			JSON_TYPE_END}));
		server->running = false;
		return;
	}
	*/
}

void Audio_Thread(AUDIO_SERVER* server) {

	thread* opus_thread = NULL;
	uint8_t* swapper;
	int rc;

	int ret = 1;
    int error;

	#ifdef _WIN64
	char* buffer = new char[MAX_SO_PACKETSIZE];
	int bytes_left_over = 0;
	int buffer_index = 0;
	int bytes_copied;
	int index;
	#endif

	while (server->running) {
		#ifdef _WIN64

		if (
		Receive_UDP_CLIENT(server->client, buffer, MAX_SO_PACKETSIZE) !=
		MAX_SO_PACKETSIZE) {
			Sleep(16);
			continue;
		}

		bytes_left_over = MAX_SO_PACKETSIZE - 2;

		bytes_copied = ((bytes_left_over > ((AS_FRAME_SIZE*AS_CHANNELS * 2) -
		buffer_index)) ? ((AS_FRAME_SIZE*AS_CHANNELS * 2) - buffer_index) :
		bytes_left_over);

		memcpy(
		((char*)server->PCM_bytes) + buffer_index, &buffer[2], bytes_copied);

		buffer_index += bytes_copied;
		bytes_left_over -= bytes_copied;

		if (buffer_index >= AS_FRAME_SIZE * AS_CHANNELS * 2) {
			//compress and send!!!
			if (opus_thread != NULL) {
				opus_thread->join();
				delete opus_thread;
			}

			swapper = server->PCM_bytes;
			server->PCM_bytes = server->PCM_bytes_back;
			server->PCM_bytes_back = swapper;

			opus_thread = new thread(Send_Buffer, server);

			buffer_index = 0;
		}

		if (bytes_left_over > 0) {
			bytes_copied = ((bytes_left_over >
			((AS_FRAME_SIZE*AS_CHANNELS * 2) - buffer_index)) ?
			((AS_FRAME_SIZE*AS_CHANNELS * 2) - buffer_index) : bytes_left_over);

			memcpy(
			((char*)server->PCM_bytes) + buffer_index, &buffer[2] +
			((MAX_SO_PACKETSIZE - 2) - bytes_copied), bytes_copied);

			buffer_index += bytes_copied;
		}
		#endif
		#ifdef __linux__
		opus_thread = new thread(Send_Buffer, server);

		//cout << "hellooo" << endl;
		/*
		rc = snd_pcm_readi(server->handle, (void *)server->PCM_bytes,
			AS_FRAME_SIZE);

		while (rc < 0) {
			snd_pcm_recover(server->handle, rc, 0);
			rc = snd_pcm_readi(server->handle, (void *)server->PCM_bytes,
				AS_FRAME_SIZE);
		}
		*/
		if (pa_simple_read(server->s, server->PCM_bytes, AS_FRAME_SIZE * 4, &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
		}

		//cout << "hellooo232" << endl;

		opus_thread->join();
		delete opus_thread;

		swapper = server->PCM_bytes;
		server->PCM_bytes = server->PCM_bytes_back;
		server->PCM_bytes_back = swapper;
		#endif
	}

	#ifdef __linux__
	//snd_pcm_drain(server->handle);
	#endif
	#ifdef _WIN64
	delete [] buffer;
	#endif
}

void Connect_AUDIO_SERVER(AUDIO_SERVER* server, SERVER* s) {
	//if (server->handle == NULL) {
	//	return;
	//}

	Disconnect_AUDIO_SERVER(server);
        server->audio_count = 1;
	server->running = true;
	server->S = s;
	server->thr = new thread(Audio_Thread, server);
}

void Disconnect_AUDIO_SERVER(AUDIO_SERVER* server) {
	if (server->thr) {
		server->running = false;
		server->thr->join();
		delete server->thr;
		server->thr = NULL;
	}
	server->S = NULL;
}

void Delete_AUDIO_SERVER(AUDIO_SERVER* server) {
	Disconnect_AUDIO_SERVER(server);

	// return;
	delete [] server->Raw;
	delete [] server->Compressed;
	delete [] server->PCM_bytes;
	delete [] server->PCM_bytes_back;
	if (server->encoder) {
		opus_encoder_destroy(server->encoder);
	}

	#ifdef __linux__
	if (server->handle) {
  		snd_pcm_close(server->handle);
	}
	#endif

	#ifdef _WIN64
	Close_UDP_CLIENT(server->client);
	delete server->client;
	#endif

	*server = NULLIFY;
}
