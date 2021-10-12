//PLATFORMS: Linux, Windows, OSX (TODO)
#ifndef  DEVICE_MANAGER_H_
#define  DEVICE_MANAGER_H_

#include "CONCURRENT_QUEUE.h"
#include "KEYBOARD.h"
#include "MOUSE.h"
#include "../Networking/Client.h"
#include "../Networking/Server.h"
#include "UTILS.h"

// Linux specific includes {{{
#ifdef __linux__
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
	//OMG OMG OMG someone fix this
	static Client	*client;
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

bool Initialize_DEVICE_MANAGER(DEVICE_MANAGER* dev_finder);
bool Connect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Server* server);
void Disconnect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);
bool Connect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Client* client);
void Disconnect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);
void Delete_DEVICE_MANAGER(DEVICE_MANAGER* dev_man);

bool Send_Mouse_Data_Single(MOUSE_EVENT_T *);
bool Send_Keyboard_Data_Single(KEYBOARD_EVENT_T *);

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
