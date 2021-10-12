//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  MOUSE_H_
#define  MOUSE_H_

#include "LOGGING.h"
#include "CONCURRENT_QUEUE.h"

// Linux specific includes {{{
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#include "input-event-codeswin.h"
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif
// }}}

#define MOUSE_ABS_COORD				0
#define MOUSE_REL_COORD				1
#define MOUSE_BUTTON_LEFT			1
#define MOUSE_BUTTON_RIGHT			3
#define MOUSE_BUTTON_MIDDLE			2
#define MOUSE_BUTTON_SCROLL_UP		4
#define MOUSE_BUTTON_SCROLL_DOWN	5

// Linux specific code {{{
#ifdef __linux__
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}

struct MOUSE_EVENT_T {
	int16_t x;
	int16_t y;
	bool clicked;
	uint16_t button;
	uint16_t state;
};

struct MOUSE {
	// Linux specific code {{{
	#ifdef __linux__
	static Display* dpy;
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	#endif
	// }}}
};

// Linux specific code {{{
#ifdef __linux__
void Open_Display_MOUSE();

void Handle_Mouse_X11(int, Queue<MOUSE_EVENT_T*>*);
void Handle_Mouse_X11_Single(MOUSE_EVENT_T*);
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Handle_Mouse_WINAPI(MOUSE_EVENT_T* m_event);
void Handle_Mouse_WINAPI(int display_ID, Queue<MOUSE_EVENT_T*>* events);
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}

#endif
