#ifndef  KEYBOARD_H_
#define  KEYBOARD_H_

#include <Utilities/EVENT.h>
#include <Utilities/DEVICE.h>
#include <Utilities/LOGGING.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>


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
#define XK_Control_R                     0xffe4
#define XK_Escape                        0xff1b

struct nk_context;

struct KEYBOARD_EVENT {
	uint16_t code;
	int32_t value;
};

struct KEYBOARD {
	static bool    Shift;
	static bool    Caps_Lock;
	static char*   Keys;
	static char*   Keys_Shifted;

	string path;
	int fd;
	Queue<KEYBOARD_EVENT*> Events;
	volatile bool Listening;
	thread* Event_Listener;
	EVENT* Event_Status;
};

void      Initialize_Keyboard(KEYBOARD*, string, EVENT*);
KEYBOARD* Construct_Keyboard (string, EVENT*);
void      Delete_Keyboard    (KEYBOARD*);
void Listen_Keyboard(KEYBOARD* keyboard);

//void Handle_Keyboard_X11(int display_ID, KEYBOARD** keyboard, int len);
void Handle_Keyboard_X11(int display_ID, Queue<KEYBOARD_EVENT*>* events);
//void Handle_Keyboard_NK(nk_context* context, KEYBOARD** keyboard, int len);

#endif
