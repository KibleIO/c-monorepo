#include "MOUSE_SERVICE.h"

bool Initialize_MOUSE_SERVICE(MOUSE_SERVICE *mouse, KCONTEXT *ctx) {
	mouse->ctx = ctx;
	mouse->main_loop = NULL;
	mouse->main_loop_running = false;

	return true;
}

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

void Main_Loop_MOUSE_SERVICE(MOUSE_SERVICE *mouse) {
	MOUSE_EVENT_T buf;
	int size;

	while (mouse->main_loop_running) {
		/*
		size = Receive_Unsafe_SERVER(mouse->s, (char*)buf);

		if (size > 0) {
			Handle_Mouse_X11_Single1(&buf[C_ARRAY_QUEUE_SIZE - 1]);
		}
		*/

		if (Receive_SERVER(mouse->s, (char*) &buf, sizeof(MOUSE_EVENT_T))) {
			Handle_Mouse_X11_Single1(&buf);
		}
	}
}

bool Connect_MOUSE_SERVICE(MOUSE_SERVICE *mouse, SERVER *s) {
	mouse->s = s;

	Open_Display_MOUSE();

	ASSERT_E_R(mouse->s != NULL, "Server is NULL", mouse->ctx);

	mouse->main_loop_running = true;
	mouse->main_loop = new thread(Main_Loop_MOUSE_SERVICE, mouse);

	return true;
}

void Disconnect_MOUSE_SERVICE(MOUSE_SERVICE *mouse) {
	mouse->main_loop_running = false;
	if (mouse->main_loop != NULL) {
		mouse->main_loop->join();
		delete mouse->main_loop;
		mouse->main_loop = NULL;
	}
}

void Delete_MOUSE_SERVICE(MOUSE_SERVICE *mouse) {

}
