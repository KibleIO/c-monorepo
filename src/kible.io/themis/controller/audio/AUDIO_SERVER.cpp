#include "AUDIO_SERVER.h"

void Recv_Callback_AUDIO_SERVER(void *user_ptr, char *buffer, int buffer_size) {
	//nop... video server doesn't receive data
}

bool AUDIO_SERVER::Initialize(KCONTEXT *ctx_in,
	SERVICE_SERVER_REGISTRY *registry) {
	
	ctx = ctx_in;
	Compressed = new uint8_t[AS_MAX_FRAME_SIZE];
	PCM_bytes = new uint8_t[AS_FRAME_SIZE * AS_CHANNELS * 2];
	PCM_bytes_back = new uint8_t[AS_FRAME_SIZE * AS_CHANNELS * 2];

	memset(Compressed, 0, AS_MAX_FRAME_SIZE);
	memset(PCM_bytes, 0, AS_FRAME_SIZE * AS_CHANNELS * 2);
	memset(PCM_bytes_back, 0, AS_FRAME_SIZE * AS_CHANNELS * 2);

	if (!Initialize_OPUS_PA(&opus_pa)) {
		return false;
	}

	if (!Initialize_SOCKET_SERVER(&socket_server,
		Recv_Callback_AUDIO_SERVER, &registry->socket_server_registry,
		ctx, this)) {
		
		return false;
	}

	main_loop_running = true;
        main_loop = new thread(Main_AUDIO_SERVER, this);
}

void AUDIO_SERVER::Delete() {
	main_loop_running = false;
	if (main_loop != NULL) {
		main_loop->join();
		delete main_loop;
		main_loop = NULL;
	}

        Delete_OPUS_PA(&opus_pa);

	delete [] Compressed;
	delete [] PCM_bytes;
	delete [] PCM_bytes_back;

	Delete_SOCKET_SERVER(&socket_server);
}

void Send_Buffer(AUDIO_SERVER *audio) {
	int send_size = Encode_Buffer_OPUS_PA(&audio->opus_pa,
		(char*) audio->PCM_bytes_back, (char*) audio->Compressed);

	if (send_size > 0) {
		Send_SOCKET_SERVER(&audio->socket_server,
				(char*)audio->Compressed,
				send_size);
	}
}

void Main_AUDIO_SERVER(AUDIO_SERVER *audio) {
	thread* opus_thread = NULL;
	uint8_t* swapper;

	int error;

	while (audio->main_loop_running) {
		opus_thread = new thread(Send_Buffer, audio);

		Get_Audio_OPUS_PA(&audio->opus_pa, (char*)audio->PCM_bytes);

		opus_thread->join();
		delete opus_thread;

		swapper = audio->PCM_bytes;
		audio->PCM_bytes = audio->PCM_bytes_back;
		audio->PCM_bytes_back = swapper;
	}
}