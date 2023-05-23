#ifndef THEMIS_EXT_H_
#define THEMIS_EXT_H_

#include <Utilities/KCONTEXT.h>
#include <Utilities/ASSERT.h>
#include "controller/video/VIDEO_SERVER.h"
#include "controller/MOUSE_SERVER.h"
#include "controller/KEYBOARD_SERVER.h"
#include "controller/audio/AUDIO_SERVER.h"
#include "LIMITS.h"

struct THEMIS_EXT {
	volatile bool connected;
	SERVICE_SERVER_REGISTRY registry;
};

bool Initialize_THEMIS_EXT(THEMIS_EXT*, KCONTEXT*);
bool Running_THEMIS_EXT(THEMIS_EXT*);
void Delete_THEMIS_EXT(THEMIS_EXT*);

#endif
