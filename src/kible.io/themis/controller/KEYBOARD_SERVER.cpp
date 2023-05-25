#include "KEYBOARD_SERVER.h"

void Recv_Callback_KEYBOARD_SERVER(void *user_ptr, char *buffer, int buffer_size) {
	KEYBOARD_EVENT_T *buf = (KEYBOARD_EVENT_T *) buffer;
	Handle_Keyboard_X11_Single(buf);
}

bool KEYBOARD_SERVER::Initialize(KCONTEXT *ctx_in,
	SERVICE_SERVER_REGISTRY *registry) {
	
	ctx = ctx_in;

	if (!Initialize_SOCKET_SERVER(&socket_server,
		Recv_Callback_KEYBOARD_SERVER,
		&registry->socket_server_registry, ctx, this)) {
		
		return false;
	}

	Open_Display_KEYBOARD();

	return true;
}

void KEYBOARD_SERVER::Delete() {
	Delete_SOCKET_SERVER(&socket_server);
}
