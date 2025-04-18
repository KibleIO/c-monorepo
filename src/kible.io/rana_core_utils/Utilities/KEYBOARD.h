//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  KEYBOARD_H_
#define  KEYBOARD_H_

#include "LOGGING.h"
#include "CONCURRENT_QUEUE.h"

// Linux specific includes {{{
#ifdef __linux__
#include <linux/input.h>
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

#include "SDL_scancode.h"

struct KEYBOARD_EVENT_T {
	uint32_t code;
	int32_t value;
	uint32_t event_index; //this will eventually have a wrap around problem
} __attribute__((packed));

struct KEYBOARD {
	static bool    Shift;
	static bool    Caps_Lock;

	#ifdef __linux__
	static Display* dpy;
	#endif
};

// Linux specific code {{{
#ifdef __linux__
#define XK_Return                        0xff0d
#define XK_Shift_L                       0xffe1
#define XK_Shift_R                       0xffe2
#define XK_Caps_Lock                     0xffe5
#define XK_BackSpace                     0xff08
#define XK_KP_Up                         0xff52
#define XK_KP_Down                       0xff54
#define XK_KP_Left                       0xff51
#define XK_KP_Right                      0xff53
#define XK_Tab                           0xff09
#define XK_Control_L                     0xffe3
#define XK_Control_R                    0xffe4
#define XK_Alt_L 			0xFFE9
#define XK_Alt_R			0xFFEA
#define XK_Escape                        0xff1b
#define XK_F1                            0xffbe
#define XK_F2                            0xffbf
#define XK_F3                            0xffc0
#define XK_F4                            0xffc1
#define XK_F5                            0xffc2
#define XK_F6                            0xffc3
#define XK_F7                            0xffc4
#define XK_F8                            0xffc5
#define XK_F9                            0xffc6
#define XK_F10                           0xffc7
#define XK_F11                           0xffc8
#define XK_F12                           0xffc9
#define XK_Insert                        0xff63  /* Insert, insert here */
#define XK_Delete                        0xffff  /* Delete, rubout */
#define XK_Home                          0xff50
#define XK_Page_Up                       0xff55
#define XK_Page_Down                     0xff56
#define XK_End                           0xff57  /* EOL */

void Open_Display_KEYBOARD();
void Close_Display_KEYBOARD();

void Handle_Keyboard_X11_Single(KEYBOARD_EVENT_T*);
void Handle_Keyboard_X11(int display_ID, Queue<KEYBOARD_EVENT_T*>* events);
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Handle_Keyboard_WINAPI(KEYBOARD_EVENT_T *k_event);
void Handle_Keyboard_WINAPI(int display_ID, Queue<KEYBOARD_EVENT_T*>* events);
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}


#endif
