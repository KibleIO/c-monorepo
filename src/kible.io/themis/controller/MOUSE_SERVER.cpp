#include "MOUSE_SERVER.h"

void Handle_Mouse_X11_Single1(MOUSE_EVENT_T *m_event) {
	if (m_event->clicked) {
		XTestFakeButtonEvent(MOUSE::dpy, m_event->button,
			m_event->state, CurrentTime);
		//if (m_event->button == 4 || m_event->button == 5) {
		//	XTestFakeButtonEvent(MOUSE::dpy, m_event->button,
		//		0, CurrentTime);
		//}
	} else {
		if (m_event->state == MOUSE_ABS_COORD) {
			XTestFakeMotionEvent(
			MOUSE::dpy, 0, m_event->x, m_event->y, CurrentTime);
		} else {
			XTestFakeRelativeMotionEvent(
			MOUSE::dpy, m_event->x, m_event->y, CurrentTime);
		}
	}
	XFlush(MOUSE::dpy);
}

void Recv_Callback_MOUSE_SERVER(void *user_ptr, char *buffer, int buffer_size) {
	MOUSE_EVENT_T *buf = (MOUSE_EVENT_T *) buffer;
	Handle_Mouse_X11_Single1(buf);
}

bool MOUSE_SERVER::Initialize(KCONTEXT *ctx_in,
	SERVICE_SERVER_REGISTRY *registry) {

	ctx = ctx_in;

	if (!Initialize_SOCKET_SERVER(&socket_server,
		Recv_Callback_MOUSE_SERVER, &registry->socket_server_registry,
		ctx, this)) {
		
		return false;
	}

	Open_Display_MOUSE();

	return true;
}

void MOUSE_SERVER::Delete() {
	Delete_SOCKET_SERVER(&socket_server);
}