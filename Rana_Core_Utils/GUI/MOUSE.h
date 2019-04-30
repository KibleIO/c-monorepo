//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  MOUSE_H_
#define  MOUSE_H_

#include <thread>

#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include "../Utilities/EVENT.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/TIMER.h"
#include "../Utilities/CONCURRENT_QUEUE.h"

// Linux specific includes {{{
#ifdef __linux__
#include <libinput.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <poll.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <linux/input.h>
#include <libevdev/libevdev.h>
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

#define MOUSE_BUTTON_LEFT			1
#define MOUSE_BUTTON_RIGHT			3
#define MOUSE_BUTTON_MIDDLE			2
#define MOUSE_BUTTON_SCROLL_UP		4
#define MOUSE_BUTTON_SCROLL_DOWN	5

// Linux specific code {{{
#ifdef __linux__
#define MAX_STR_LEN 256

struct MOUSE_EVENT_ELEMENT {
	libinput_event* Event;
};

struct Mouse_Info {
	int vendor;
	int product;
	int version;
	int busid;

	char properties[INPUT_PROP_MAX];

	char name[MAX_STR_LEN];
	char key[KEY_MAX];
	char rel[REL_MAX];
	char abs[ABS_MAX];
	input_absinfo abs_info[ABS_MAX];
};
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}

struct MOUSE_EVENT_T {
	uint16_t x;
	uint16_t y;
	bool clicked;
	uint16_t button;
	uint16_t state;
};

struct MOUSE{
	int				Minimum_X;
	int				Maximum_X;
	int				Minimum_Y;
	int				Maximum_Y;
	static int		Current_X;
	static int		Current_Y;
	static bool		Clicked;
	float			Sensitivity;

	// Linux specific code {{{
	#ifdef __linux__
	libinput_device *device;
	libinput *li;
	bool grab;
	pollfd fds;
	struct sigaction act;
	libinput_interface interface;

	volatile bool Listening;
	thread*       Event_Listener;
	Queue<MOUSE_EVENT_ELEMENT*> Events;

	Mouse_Info mouse_info;

	EVENT* Event_Status;
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
void Initialize_Mouse(MOUSE*, int, int, int, int, float, string, EVENT*);

MOUSE* Construct_Mouse(int, int, int, int, float, string, EVENT*);

void Delete_Mouse(MOUSE*);

void Update_Mouse(MOUSE*);

void Listen_Mouse(MOUSE*);

void Listen_Mouse_Once(MOUSE* mouse);

int open_restricted(const char* path, int flags, void* user_data);

void close_restricted(int fd, void* user_data);

void sighandler(int signal, siginfo_t* siginfo, void* userdata);

void Handle_Mouse_X11(int display_ID, Queue<MOUSE_EVENT_T*>* events);
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
