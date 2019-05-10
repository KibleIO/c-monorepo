//PLATFORMS: Linux, Windows, OSX (TODO)
#ifndef  DEVICE_MANAGER_H_
#define  DEVICE_MANAGER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "CONCURRENT_QUEUE.h"
#include "../GUI/KEYBOARD.h"
#include "../GUI/MOUSE.h"
#include "../Networking/Client.h"
#include "../Networking/Server.h"
#include "TIMER.h"
#include "EVENT.h"

// Linux specific includes {{{
#ifdef __linux__
#include <linux/input.h>
#include <libevdev/libevdev-uinput.h>
#include <linux/uinput.h>
#include <dirent.h>
#include <unistd.h>
#define INPUT_PATH "/dev/input"
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}

#define MAX_DEV 100
#define _MOUSE 0
#define _KEYBOARD 1
#define _NIL 2
#define _GARBAGE 2
#define _DATA 1
#define _STOP 0
#define TIMEOUT 1000

#define MOUSE_PACKET 1
#define KEY_PACKET 2

using namespace std;

// Linux specific code {{{
#ifdef __linux__
struct DEVICE_NODE {
	string str;
	int type;
	bool okay;
	union {
		MOUSE* mouse;
		KEYBOARD* keyboard;
	} hw;
};

#endif
// }}} Windows specific code {{{
#ifdef _WIN64
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}

struct DEVICE_MANAGER {
	// Linux specific code {{{
	#ifdef __linux__
	volatile bool sending;

	DEVICE_NODE** current_dev;
	int           c_d_size;
	static DEVICE_NODE** previous_dev;
	static volatile int  p_d_size;

	char path[PATH_MAX];
	DIR *dp;
	dirent *dirp;
	int fd;
	struct stat buffer;
	unsigned int types[EV_MAX];
	unsigned int events[(KEY_MAX - 1) / 32 + 1];
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	mutex client_mtx;
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	static Queue<KEYBOARD_EVENT_T*> Keyboard_Events;
	static Queue<MOUSE_EVENT_T*> Mouse_Events;

	Client*       client;
	Server*		  server;

	volatile bool receiving;

	int 		  w, h; //these are only needed to initialize the mouse, sue me

	EVENT* Event_Status;
};

void Initialize_Device_Manager(
DEVICE_MANAGER* dev_finder, int w, int h, EVENT* event_status = NULL);

void Device_Server_Start(DEVICE_MANAGER* dev_finder);

void Device_Server_Listen(DEVICE_MANAGER* dev_man, Server* server);

void Device_Server_Stop(DEVICE_MANAGER* dev_finder);

void Device_Client_Connect(DEVICE_MANAGER* dev_man, Client* client);

void Set_Mouse_Speed(double);

// Linux specific code {{{
#ifdef __linux__
void Device_Client_Start(DEVICE_MANAGER* dev_finder);

void Device_Client_Stop(DEVICE_MANAGER* dev_finder);

void Device_Client_Close_Connection(DEVICE_MANAGER* dev_finder);

void Device_Server_Close_Connection(DEVICE_MANAGER* dev_finder);

void Refresh_Devices(DEVICE_MANAGER* dev_finder);

void Delete_Device_Manager(DEVICE_MANAGER* dev_man);
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Send_Mouse_Data(DEVICE_MANAGER* dev_man, MOUSE_EVENT_T* m_event);

void Send_Keyboard_Data(DEVICE_MANAGER* dev_man, KEYBOARD_EVENT_T* k_event);
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}

#endif
