#ifndef AUDIO_SERVER_H_
#define AUDIO_SERVER_H_

#include <thread>
#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <controller/server/SERVICE_SERVER.h>
#include "OPUS_PA.h"

struct AUDIO_SERVER : public SERVICE_SERVER{
	KCONTEXT *ctx;
	OPUS_PA opus_pa;
	SOCKET_SERVER socket_server;
	thread *main_loop;
	volatile bool main_loop_running;
	uint8_t*	Compressed;
	uint8_t*	PCM_bytes;
	uint8_t*	PCM_bytes_back;

	bool Initialize(KCONTEXT*, SERVICE_SERVER_REGISTRY*) override;
	void Delete() override;
};

void Main_AUDIO_SERVER(AUDIO_SERVER*);

#endif
