//PLATFORMS: Linux, Windows, OSX (TODO)
#ifndef  DEVICE_MANAGER_H_
#define  DEVICE_MANAGER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "CONCURRENT_QUEUE.h"
#include "KEYBOARD.h"
#include "MOUSE.h"
#include "../Networking/Client.h"
#include "../Networking/Server.h"
#include "../Utilities/Stuff.h"
#include "TIMER.h"
#include "EVENT.h"

// Linux specific includes {{{
#ifdef __linux__
#include <linux/input.h>
#include <libevdev/libevdev-uinput.h>
#include <linux/uinput.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/inotify.h>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}

#define MOUSE_PACKET 1
#define KEY_PACKET 2

using namespace std;

struct DEVICE_MANAGER {
	Client	*client;
	Server	*server;

#ifdef __linux__
	volatile uint8_t sending;
	thread* send_thr;
#endif
#ifdef _WIN64
	mutex* client_mtx;
#endif

	static Queue<KEYBOARD_EVENT_T*> Keyboard_Events;
	static Queue<MOUSE_EVENT_T*> Mouse_Events;

	volatile bool receiving;
	thread* recv_thr;
};

void Reset_Mouse_Clicks();

bool Initialize_DEVICE_MANAGER(
DEVICE_MANAGER* dev_finder, int w, int h, EVENT* event_status = NULL);

void Connect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Server* server);

void Disconnect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);

void Connect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Client* client);

void Disconnect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);

void Delete_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);

// Linux specific code {{{
#ifdef __linux__
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
