#include "OPUS_PA.h"

bool Initialize_OPUS_PA(OPUS_PA* server) {
	int32_t rc;
	int error;
	pa_buffer_attr attr;
	attr.maxlength = (uint32_t) -1;
	attr.tlength = (uint32_t) -1;
	attr.prebuf = (uint32_t) -1;
	attr.minreq = (uint32_t) -1;
	attr.fragsize = 1600;

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 48000,
		.channels = 2
	};

	server->encoder = opus_encoder_create(
	AS_SAMPLE_RATE, AS_CHANNELS, OPUS_APPLICATION_AUDIO, &rc);
	if (rc < 0) {
		return false;
	}
	opus_encoder_ctl(server->encoder, OPUS_SET_BITRATE(AS_BITRATE));
	
	if (!(server->pa_server = pa_simple_new(NULL, "themis",
		PA_STREAM_RECORD, NULL, "record", &ss, NULL, &attr, &error))) {
		
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n",
			pa_strerror(error));
		return false;
	}

	return true;
}

int Encode_Buffer_OPUS_PA(OPUS_PA* server, char *input, char *output) {
	int send_size = opus_encode(server->encoder,
		(opus_int16*)input, AS_FRAME_SIZE,
		(unsigned char*) output + 8, AS_MAX_FRAME_SIZE);

	if (send_size < 0) {
		return 0;
	}

	((int*)output)[0] = server->audio_count++;
	((int*)output)[1] = send_size;
	send_size += 8;

	return send_size;
}

void Get_Audio_OPUS_PA(OPUS_PA* server, char *output) {
	int error;
	if (pa_simple_read(server->pa_server, output,
		AS_FRAME_SIZE * 4, &error) < 0) {
		
		fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n",
			pa_strerror(error));
	}
}

void Delete_OPUS_PA(OPUS_PA* server) {
	if (server->encoder) {
		opus_encoder_destroy(server->encoder);
	}
}
