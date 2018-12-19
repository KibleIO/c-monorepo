#ifndef  DEVICE_FINDER_H_
#define  DEVICE_FINDER_H_

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

#define DFID_RANA 0
#define DFID_IRIS 1
#define DFID_THEMIS 2

using namespace std;

struct x32_timeval {
        __u64 tv_sec;
        __u64 tv_usec;
};

struct x32_input_event {
        x32_timeval time;
        __u16 type;
        __u16 code;
        __s32 value;
};

struct DEVICE_NODE {
	string str;
	int port;
	int type;
	bool running;
	bool okay;
	thread* thr;
};

struct Recv_node {
	thread* thr;
	bool complete;
};

struct DEVICE_FINDER {
	int type_id;

	DEVICE_NODE** current_dev;
	int           c_d_size;
	DEVICE_NODE** previous_dev;
	int           p_d_size;
	string        address;
	MOUSE**    mouse;
	KEYBOARD** keyboard;
	int w, h; //these are only needed to initialize the mouse, sue me

	//stuff for reading /dev/input files
	char path[PATH_MAX];
    DIR *dp;
    dirent *dirp;
    int fd;
    struct stat buffer;
    unsigned int types[EV_MAX];
    unsigned int events[(KEY_MAX - 1) / 32 + 1];

	int iris_id;
	EVENT* Event_Status;
	Client*       client;
	INDEX_QUEUE*  index_queue;
	
	static volatile bool locked;
};

//DEVICE FINDER
void   Initialize_Device_Node(DEVICE_NODE*, string, int, int);
void   Delete_Device_Node(DEVICE_NODE*);
bool   Check_Device_Node(DEVICE_NODE*);
int    Get_Bit(unsigned int*, unsigned int);
void   Send_Data(DEVICE_FINDER*, DEVICE_NODE*);
void   Add_Device_Node(DEVICE_FINDER*, DEVICE_NODE*);
void   Remove_Device_Node(DEVICE_FINDER*, DEVICE_NODE*);
void   Refresh_Devices(DEVICE_FINDER*);
void   Initialize_Device_Finder(DEVICE_FINDER*, int, int, int, EVENT*);
void Delete_Device_Finder(DEVICE_FINDER* dev_finder);

//NSYNC CLIENT
bool   Initialize_NSYNC(DEVICE_FINDER*, int, string);
void   Delete_NSYNC(DEVICE_FINDER*);

//NSYNC SERVER
void Device_Thread(int, int, volatile bool*);
void Receive_Data(int, int, string, bool*);
int Create_New_Mouse(string&, int);
int Create_New_Keyboard(string&, int);

#endif
