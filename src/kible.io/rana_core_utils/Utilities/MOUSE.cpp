//PLATFORMS: Linux, Windows, OSX

#include "MOUSE.h"

#ifdef __linux__
Display* MOUSE::dpy;
#endif

// Linux specific code {{{
#ifdef __linux__

void Open_Display_MOUSE() {
	MOUSE::dpy = XOpenDisplay(":1");
}

void Close_Display_MOUSE() {
	XCloseDisplay(MOUSE::dpy);
}

void Handle_Mouse_X11_Single(MOUSE_EVENT_T *m_event) {
	if (m_event->clicked) {
		XTestFakeButtonEvent(MOUSE::dpy, m_event->button,
			m_event->state, CurrentTime);
		if (m_event->button == 4 || m_event->button == 5) {
			XTestFakeButtonEvent(MOUSE::dpy, m_event->button,
				0, CurrentTime);
		}
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

void Handle_Mouse_X11(int display_ID, Queue<MOUSE_EVENT_T*>* events) {
	if (!MOUSE::dpy) {
		log_err(((const JSON_TYPE){
			{"message", "Unable to open display"},
			JSON_TYPE_END}));
		return;
	}

	for (int i = events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event = NULL;
		events->pop(m_event);
		if (m_event->clicked) {
			XTestFakeButtonEvent(
			MOUSE::dpy, m_event->button, m_event->state, CurrentTime);
		} else {
			if (m_event->state == MOUSE_ABS_COORD) {
				XTestFakeMotionEvent(
				MOUSE::dpy, 0, m_event->x, m_event->y, CurrentTime);
			} else {
				XTestFakeRelativeMotionEvent(
				MOUSE::dpy, m_event->x, m_event->y, CurrentTime);
			}
		}
		delete m_event;
	}

	XFlush(MOUSE::dpy);
}
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
/*
void Handle_Mouse_WINAPI(MOUSE_EVENT_T* m_event) {
	INPUT ip;
	ip.type = INPUT_MOUSE;
	ip.mi.time = 0;
	ip.mi.mouseData = 0;
	ip.mi.dwExtraInfo = 0;
	if (m_event->clicked) {
		switch (m_event->button) {
		case MOUSE_BUTTON_LEFT:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			}
			break;
		case MOUSE_BUTTON_RIGHT:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			}
			break;
		case MOUSE_BUTTON_MIDDLE:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			}
			break;
		case MOUSE_BUTTON_SCROLL_UP:
			ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
			ip.mi.mouseData = m_event->state * 45;
			break;
		case MOUSE_BUTTON_SCROLL_DOWN:
			ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
			ip.mi.mouseData = m_event->state * -45;
			break;
		}
		SendInput(1, &ip, sizeof(INPUT));
	} else {
		ip.mi.dx = (m_event->x * 0xFFFF) / (GetSystemMetrics(SM_CXSCREEN) - 1);

		ip.mi.dy = (m_event->y * 0xFFFF) / (GetSystemMetrics(SM_CYSCREEN) - 1);

		ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &ip, sizeof(INPUT));
	}
	delete m_event;
}

void Handle_Mouse_WINAPI(int display_ID, Queue<MOUSE_EVENT_T*>* events) {
	INPUT ip;

	//Set up the INPUT structure
	ip.type = INPUT_MOUSE;
	ip.mi.time = 0;
	ip.mi.mouseData = 0;
	ip.mi.dwExtraInfo = 0;

	for (int i = events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event;
		events->pop(m_event);
		if (m_event->clicked) {
			switch (m_event->button) {
			case MOUSE_BUTTON_LEFT:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				}
				break;
			case MOUSE_BUTTON_RIGHT:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				}
				break;
			case MOUSE_BUTTON_MIDDLE:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				}
				break;
			case MOUSE_BUTTON_SCROLL_UP:
				ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
				ip.mi.mouseData = m_event->state * 45;
				break;
			case MOUSE_BUTTON_SCROLL_DOWN:
				ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
				ip.mi.mouseData = m_event->state * -45;
				break;
			}
			SendInput(1, &ip, sizeof(INPUT));
		} else {
			ip.mi.dx = (
			m_event->x * 0xFFFF) / (GetSystemMetrics(SM_CXSCREEN) - 1);

			ip.mi.dy = (
			m_event->y * 0xFFFF) / (GetSystemMetrics(SM_CYSCREEN) - 1);

			ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			SendInput(1, &ip, sizeof(INPUT));
		}

		delete m_event;
	}
}
*/
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}
