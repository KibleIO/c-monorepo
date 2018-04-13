#ifndef  MOUSE_H_
#define  MOUSE_H_

#include <libinput.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <linux/input.h>
#include <thread>
#include <Utilities/DynArr.h>
#include <libevdev.h>

struct MOUSE_EVENT_ELEMENT : public NXTElement {
	libinput_event* Event;
};

struct Abs_info {
	int minimum;
	int maximum;
	int fuzz;
	int flat;
	int resolution;
};

struct Mouse_Info {
	int vendor;
	int product;
	int version;
	int busid;
	
	char key[KEY_MAX];
	char rel[REL_MAX];
	char abs[ABS_MAX];
	Abs_info abs_info[ABS_MAX];
};

struct MOUSE{
	int     Minimum_X;
	int     Maximum_X;
	int     Minimum_Y;
	int     Maximum_Y;
	int     Current_X;
	int     Current_Y;
	float   Sensitivity;
	
	libinput_device *device;
	libinput *li;
	bool grab;
	pollfd fds;
	struct sigaction act;
	libinput_interface interface;
	
	volatile bool Listening;
	thread*       Event_Listener;
	DynArr        Mouse_Event_Stack;
	
	Mouse_Info mouse_info;
};

void   Initialize_Mouse(MOUSE*, int, int, int, int, float, string);
MOUSE* Construct_Mouse (int, int, int, int, float, string);
void   Delete_Mouse    (MOUSE*);
void   Update_Mouse    (MOUSE*);
void   Listen_Mouse    (MOUSE*);

int open_restricted(const char* path, int flags, void* user_data);
void close_restricted(int fd, void* user_data);
void sighandler(int signal, siginfo_t* siginfo, void* userdata);

#endif