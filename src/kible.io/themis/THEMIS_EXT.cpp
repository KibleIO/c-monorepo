#include "THEMIS_EXT.h"

bool Initialize_THEMIS_EXT(THEMIS_EXT *themis, KCONTEXT *ctx) {
	themis->connected = true;
	if (!Initialize_SERVICE_SERVER_REGISTRY(&themis->registry, ctx, 4,
		new MOUSE_SERVER,
		new KEYBOARD_SERVER,
		new VIDEO_SERVER,
		new AUDIO_SERVER)) {
		
		return false;
	}

	Start_TIMER(&themis->timer);

	return true;
}

void Delete_THEMIS_EXT(THEMIS_EXT *themis) {
	Delete_SERVICE_SERVER_REGISTRY(&themis->registry);
	themis->connected = false;
}

bool Running_THEMIS_EXT(THEMIS_EXT *themis) {
	return (Stop_TIMER(&themis->timer) <= THEMIS_TIMEOUT_THRESHOLD);
}
