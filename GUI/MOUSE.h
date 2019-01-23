#ifndef  MOUSE_H_
#define  MOUSE_H_

#include <libinput.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <linux/input.h>
#include <thread>
#include <Utilities/DynArr.h>
#include <libevdev/libevdev.h>
#include <Utilities/EVENT.h>
#include <Utilities/LOGGING.h>
#include <Utilities/Timer.h>
#include <Utilities/CONCURRENT_QUEUE.h>

struct nk_context;

struct MOUSE_EVENT_ELEMENT : public NXTElement {
	libinput_event* Event;
};

struct MOUSE_EVENT {
	int x;
	int y;
	bool clicked;
	int state;
};

#define MAX_STR_LEN 256

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

struct MOUSE{
	int     	   Minimum_X;
	int     	   Maximum_X;
	int     	   Minimum_Y;
	int     	   Maximum_Y;
	static int     Current_X;
	static int     Current_Y;
	static bool 	Clicked;
	float          Sensitivity;

	libinput_device *device;
	libinput *li;
	bool grab;
	pollfd fds;
	struct sigaction act;
	libinput_interface interface;

	volatile bool Listening;
	thread*       Event_Listener;
	Queue<MOUSE_EVENT_ELEMENT*> Mouse_Event_Stack;

	Mouse_Info mouse_info;

	EVENT* Event_Status;
};

void   Initialize_Mouse(MOUSE*, int, int, int, int, float, string, EVENT*);
MOUSE* Construct_Mouse (int, int, int, int, float, string, EVENT*);
void   Delete_Mouse    (MOUSE*);
void   Update_Mouse    (MOUSE*);
void   Listen_Mouse    (MOUSE*);

int open_restricted(const char* path, int flags, void* user_data);
void close_restricted(int fd, void* user_data);
void sighandler(int signal, siginfo_t* siginfo, void* userdata);

//void Handle_Mouse_X11(int display_ID, MOUSE** mouse, int len);
void Handle_Mouse_X11(int display_ID, Queue<MOUSE_EVENT*>* events);
#endif
