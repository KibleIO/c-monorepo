#ifndef  DEVICE_MANAGER_H_
#define  DEVICE_MANAGER_H_

#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <linux/input.h>
#include <libevdev/libevdev-uinput.h>
#include <linux/uinput.h>
#include <thread>
#include <string>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <Networking/Networking.h>
#include <Networking/Client.h>
#include <Networking/Server.h>
#include <Utilities/Integer.h>
#include <Utilities/Timer.h>
#include <Utilities/INDEX_QUEUE.h>
#include <Utilities/EVENT.h>
#include <GUI/KEYBOARD.h>
#include <GUI/MOUSE.h>
#include <Utilities/CONCURRENT_QUEUE.h>

#define INPUT_PATH "/dev/input"
#define PORT_IRIS 8880
#define PORT_RANA 7770
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

struct DEVICE_NODE {
	string str;
	int type;
	bool okay;
	union {
		MOUSE* mouse;
		KEYBOARD* keyboard;
	} hw;
};

struct DEVICE_MANAGER {
	Client*       client;
	volatile bool sending;
	Server*		  server;
	volatile bool receiving;
	
	DEVICE_NODE** current_dev;
	int           c_d_size;
	DEVICE_NODE** previous_dev;
	int           p_d_size;
	int 		  w, h; //these are only needed to initialize the mouse, sue me

	static Queue<KEYBOARD_EVENT*> Keyboard_Events;
	static Queue<MOUSE_EVENT*> Mouse_Events;

	//stuff for reading /dev/input files
	char path[PATH_MAX];
    DIR *dp;
    dirent *dirp;
    int fd;
    struct stat buffer;
    unsigned int types[EV_MAX];
    unsigned int events[(KEY_MAX - 1) / 32 + 1];

	EVENT* Event_Status;
};

void Device_Server_Start(DEVICE_MANAGER* dev_finder);
void Device_Server_Stop(DEVICE_MANAGER* dev_finder);

void Device_Client_Start(DEVICE_MANAGER* dev_finder);
void Device_Client_Stop(DEVICE_MANAGER* dev_finder);

void Device_Server_Listen(DEVICE_MANAGER* dev_finder, int port);
void Device_Server_Listen(DEVICE_MANAGER* dev_man, Server* server);
void Device_Server_Close_Connection(DEVICE_MANAGER* dev_finder);

void Device_Client_Connect(DEVICE_MANAGER* dev_finder, int port, string ip);
void Device_Client_Connect(DEVICE_MANAGER* dev_man, Client* client);
void Device_Client_Close_Connection(DEVICE_MANAGER* dev_finder);

void Refresh_Devices(DEVICE_MANAGER* dev_finder);
void Initialize_Device_Manager(DEVICE_MANAGER* dev_finder, int w, int h, EVENT* event_status);

void Delete_Device_Manager(DEVICE_MANAGER* dev_man);
#endif
