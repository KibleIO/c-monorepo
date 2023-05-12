#include "THEMIS_EXT.h"

bool Initialize_THEMIS_EXT(THEMIS_EXT *themis, KCONTEXT *ctx, int port) {
	int32_t shmid;
	key_t key;

	themis->ctx = ctx;
	themis->port = port;

	key = SHM_BASE;
	shmid = shmget(key, sizeof(bool), IPC_CREAT | 0666);
	ASSERT_E_R(shmid != -1, "could not find shared memory", themis->ctx);

	themis->connected = (bool*)shmat(shmid, 0, 0);
	ASSERT_E_R(themis->connected != NULL, "couldn't map shared memory",
		themis->ctx);

	ASSERT_E_R((Initialize_HERMES_SERVER(&themis->hs, themis->ctx,
		themis->port)), "Couldn't initialize hermes server",
		themis->ctx);
	ASSERT_E_R((Initialize_VIDEO_SERVICE(&themis->video, themis->ctx)),
		"Couldn't initialize video service", themis->ctx);
	ASSERT_E_R((Initialize_THEMIS_SERVICE(&themis->themis, themis->ctx,
		&themis->video)),
		"Couldn't initialize themis service", themis->ctx);
	ASSERT_E_R((Initialize_MOUSE_SERVICE(&themis->mouse, themis->ctx)),
		"Couldn't initialize mouse service", themis->ctx);
	ASSERT_E_R((Initialize_KEYBOARD_SERVICE(&themis->keyboard, themis->ctx)),
		"Couldn't initialize keyboard service", themis->ctx);
	ASSERT_E_R((Initialize_AUDIO_SERVICE(&themis->audio, themis->ctx)),
		"Couldn't initialize audio service", themis->ctx);
	ASSERT_E_R((Initialize_CLIPBOARD_SERVICE(&themis->clipboard, themis->ctx)),
		"Couldn't initialize clipboard service", themis->ctx);

	return true;
}

bool Connect_THEMIS_EXT(THEMIS_EXT *themis) {
	HERMES_TYPE themis_types[] = {HERMES_SERVER_WS, HERMES_THEMIS_WS,
		HERMES_VIDEO_INIT_WS, HERMES_VIDEO_WS, HERMES_MOUSE_WS,
		HERMES_KEYBOARD_WS, HERMES_AUDIO_WS, HERMES_CLIPBOARD_WS,
		HERMES_NULL};

	ASSERT_R((Connect_HERMES_SERVER(&themis->hs, themis_types)));
	ASSERT_E_R((Connect_THEMIS_SERVICE(&themis->themis,
		Get_HERMES_SERVER(&themis->hs, HERMES_THEMIS_WS))),
		"Couldn't connect themis service", themis->ctx);
	ASSERT_E_R((Connect_VIDEO_SERVICE(&themis->video,
		Get_HERMES_SERVER(&themis->hs, HERMES_VIDEO_INIT_WS),
		Get_HERMES_SERVER(&themis->hs, HERMES_VIDEO_WS))),
		"Couldn't connect video service", themis->ctx);
	ASSERT_E_R((Connect_MOUSE_SERVICE(&themis->mouse,
		Get_HERMES_SERVER(&themis->hs, HERMES_MOUSE_WS))),
		"Couldn't connect mouse service", themis->ctx);
	ASSERT_E_R((Connect_KEYBOARD_SERVICE(&themis->keyboard,
		Get_HERMES_SERVER(&themis->hs, HERMES_KEYBOARD_WS))),
		"Couldn't connect keyboard service", themis->ctx);
	ASSERT_E_R((Connect_AUDIO_SERVICE(&themis->audio,
		Get_HERMES_SERVER(&themis->hs, HERMES_AUDIO_WS))),
		"Couldn't connect audio service", themis->ctx);
	ASSERT_E_R((Connect_CLIPBOARD_SERVICE(&themis->clipboard,
		Get_HERMES_SERVER(&themis->hs, HERMES_CLIPBOARD_WS))),
		"Couldn't connect clipboard service", themis->ctx);

	*themis->connected = true;

	return true;
}

void Disconnect_THEMIS_EXT(THEMIS_EXT *themis) {
	Disconnect_HERMES_SERVER(&themis->hs);
	Disconnect_THEMIS_SERVICE(&themis->themis);
	Disconnect_VIDEO_SERVICE(&themis->video);
	Disconnect_MOUSE_SERVICE(&themis->mouse);
	Disconnect_KEYBOARD_SERVICE(&themis->keyboard);
	Disconnect_AUDIO_SERVICE(&themis->audio);
	Disconnect_CLIPBOARD_SERVICE(&themis->clipboard);

        *themis->connected = false;
}

void Delete_THEMIS_EXT(THEMIS_EXT *themis) {
	Delete_THEMIS_SERVICE(&themis->themis);
	Delete_VIDEO_SERVICE(&themis->video);
	Delete_MOUSE_SERVICE(&themis->mouse);
	Delete_KEYBOARD_SERVICE(&themis->keyboard);
	Delete_AUDIO_SERVICE(&themis->audio);
	Delete_CLIPBOARD_SERVICE(&themis->clipboard);
	Delete_HERMES_SERVER(&themis->hs);
}

bool Running_THEMIS_EXT(THEMIS_EXT *themis) {
	if (!themis->hs.connected) {
		return false;
	}

	if (!*themis->connected) {
		Clean_Exit_HERMES_SERVER(&themis->hs);

		while (themis->hs.status != HERMES_STATUS_DISCONNECTED &&
			themis->hs.connected) {

			Sleep_Milli(500);
		}

		return false;
	}

	return themis->hs.status != HERMES_STATUS_DISCONNECTED;
}
