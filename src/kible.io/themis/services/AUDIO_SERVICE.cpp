#include "AUDIO_SERVICE.h"

bool Initialize_AUDIO_SERVICE(AUDIO_SERVICE *audio, KCONTEXT *ctx) {
	audio->ctx = ctx;

	Initialize_AUDIO_SERVER(&audio->audio_server);

	return true;
}


bool Connect_AUDIO_SERVICE(AUDIO_SERVICE *audio, SERVER *s) {
	audio->s = s;

	ASSERT_E_R(audio->s != NULL, "server is NULL", audio->ctx);

	Connect_AUDIO_SERVER(&audio->audio_server, audio->s);

	return true;
}

void Disconnect_AUDIO_SERVICE(AUDIO_SERVICE *audio) {
	Disconnect_AUDIO_SERVER(&audio->audio_server);
}

void Delete_AUDIO_SERVICE(AUDIO_SERVICE *audio) {
	Delete_AUDIO_SERVER(&audio->audio_server);
}
