#ifndef AUDIO_SERVICE_H_
#define AUDIO_SERVICE_H_

#include <thread>
#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Hermes/SERVER.h>
#include "../audio/AUDIO_SERVER.h"

struct AUDIO_SERVICE {
	KCONTEXT *ctx;
	SERVER *s;
	AUDIO_SERVER audio_server;
};

bool Initialize_AUDIO_SERVICE(AUDIO_SERVICE*, KCONTEXT*);
bool Connect_AUDIO_SERVICE(AUDIO_SERVICE*, SERVER*);
void Disconnect_AUDIO_SERVICE(AUDIO_SERVICE*);
void Delete_AUDIO_SERVICE(AUDIO_SERVICE*);

#endif
