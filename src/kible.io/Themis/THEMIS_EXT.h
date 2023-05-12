#ifndef THEMIS_EXT_H_
#define THEMIS_EXT_H_

#include <sys/shm.h>
#include <Hermes/HERMES_SERVER.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/ASSERT.h>
#include "services/VIDEO_SERVICE.h"
#include "services/THEMIS_SERVICE.h"
#include "services/MOUSE_SERVICE.h"
#include "services/KEYBOARD_SERVICE.h"
#include "services/AUDIO_SERVICE.h"
#include "services/CLIPBOARD_SERVICE.h"
#include "LIMITS.h"

struct THEMIS_EXT {
	KCONTEXT *ctx;
	bool *connected;
	int port;
	thread *hermes_loop;

	HERMES_SERVER hs;
	THEMIS_SERVICE themis;
	VIDEO_SERVICE video;
	MOUSE_SERVICE mouse;
	KEYBOARD_SERVICE keyboard;
	AUDIO_SERVICE audio;
	CLIPBOARD_SERVICE clipboard;
};

bool Initialize_THEMIS_EXT(THEMIS_EXT*, KCONTEXT*, int);
bool Connect_THEMIS_EXT(THEMIS_EXT*);
bool Running_THEMIS_EXT(THEMIS_EXT*);
void Disconnect_THEMIS_EXT(THEMIS_EXT*);
void Delete_THEMIS_EXT(THEMIS_EXT*);

#endif
