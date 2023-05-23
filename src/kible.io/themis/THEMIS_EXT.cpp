#include "THEMIS_EXT.h"

bool Initialize_THEMIS_EXT(THEMIS_EXT *themis, KCONTEXT *ctx) {
	themis->connected = false;

	if (!Initialize_SERVICE_SERVER_REGISTRY(&themis->registry, ctx, 4,
		new VIDEO_SERVER,
		new AUDIO_SERVER,
		new MOUSE_SERVER,
		new KEYBOARD_SERVER)) {
		
		return false;
	}

	themis->connected = true;

	return true;
}

void Delete_THEMIS_EXT(THEMIS_EXT *themis) {
	Delete_SERVICE_SERVER_REGISTRY(&themis->registry);
}

bool Running_THEMIS_EXT(THEMIS_EXT *themis) {
	return themis->connected;
}
