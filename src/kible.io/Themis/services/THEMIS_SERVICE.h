#ifndef THEMIS_SERVICE_H_
#define THEMIS_SERVICE_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Hermes/SERVER.h>
#include "VIDEO_SERVICE.h"

#define THEMIS_SERVICE_CODE_NULL 0
#define THEMIS_SERVICE_CODE_CHANGE_SCREEN_SIZE 1

struct THEMIS_SERVICE {
	KCONTEXT *ctx;
	VIDEO_SERVICE *video_service;
	thread *main_loop;
	volatile bool main_loop_running;
	SERVER *s;
};

bool Initialize_THEMIS_SERVICE(THEMIS_SERVICE*, KCONTEXT*, VIDEO_SERVICE*);
bool Connect_THEMIS_SERVICE(THEMIS_SERVICE*, SERVER*);
void Disconnect_THEMIS_SERVICE(THEMIS_SERVICE*);
void Delete_THEMIS_SERVICE(THEMIS_SERVICE*);

#endif
