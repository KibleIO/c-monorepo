//PLATFORMS: Linux, Windows, OSX (TODO)

#include "DEVICE_MANAGER.h"

Queue<KEYBOARD_EVENT_T*> DEVICE_MANAGER::Keyboard_Events;
Queue<MOUSE_EVENT_T*> DEVICE_MANAGER::Mouse_Events;

void Reset_Mouse_Clicks() {
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,1,1});
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,1,0});
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,2,1});
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,2,0});
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,3,1});
	DEVICE_MANAGER::Mouse_Events.push(new MOUSE_EVENT_T{0,0,1,3,0});
}

bool Initialize_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	*dev_man = {};

	// Linux specific code {{{
	#ifdef __linux__
	dev_man->sending = false;
	#endif
	#ifdef _WIN64
	dev_man->client_mtx = new mutex;
	#endif

	dev_man->client = NULL;
	dev_man->server = NULL;

	dev_man->receiving = false;
	return true;
}

void Receive_Thread(DEVICE_MANAGER* dev_man) {
	uint8_t ptype;

	log_dbg(((const JSON_TYPE){
		{"message", "receive thread started"},
		JSON_TYPE_END}));
	while (dev_man->receiving) {
		if (!dev_man->server->Receive((char*)&ptype, sizeof(uint8_t))) {
			log_err(((const JSON_TYPE){
				{"message", "could not receive input packet"},
				JSON_TYPE_END}));
			dev_man->receiving = false;
			continue;
		}

		if (ptype == MOUSE_PACKET) {
			MOUSE_EVENT_T* m_event = new MOUSE_EVENT_T;

			if (!dev_man->server->Receive((char*)m_event,
				sizeof(MOUSE_EVENT_T))) {

				log_err(((const JSON_TYPE){
					{"message", "could not receive mouse event"},
					JSON_TYPE_END}));
				dev_man->receiving = false;
				continue;
			}
			// Linux specific code {{{
			#ifdef __linux__
			DEVICE_MANAGER::Mouse_Events.push(m_event);
			#endif
			// }}} Windows specific code {{{
			#ifdef _WIN64
			Handle_Mouse_WINAPI(m_event);
			#endif
			// }}} OSX specific code {{{
			#ifdef __APPLE__
			//TODO apple code
			#endif
			// }}}
		} else if (ptype == KEY_PACKET) {
			KEYBOARD_EVENT_T* k_event = new KEYBOARD_EVENT_T;
			if (!dev_man->server->Receive((char*)k_event,
				sizeof(KEYBOARD_EVENT_T))) {

				log_err(((const JSON_TYPE){
					{"message", "could not receive keyboard event"},
					JSON_TYPE_END}));
				dev_man->receiving = false;
				continue;
			}
			// Linux specific code {{{
			#ifdef __linux__
			DEVICE_MANAGER::Keyboard_Events.push(k_event);
			#endif
			// }}} Windows specific code {{{
			#ifdef _WIN64
			Handle_Keyboard_WINAPI(k_event);
			#endif
			// }}} OSX specific code {{{
			#ifdef __APPLE__
			//TODO apple code
			#endif
			// }}}
		} else {
			log_err(((const JSON_TYPE){
				{"message", "unknown packet type"},
				JSON_TYPE_END}));
		}
	}
	log_dbg(((const JSON_TYPE){
		{"message", "receive thread going down"},
		JSON_TYPE_END}));
}

void Connect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Server* server) {
	Disconnect_Server_DEVICE_MANAGER(dev_man);
	dev_man->server = server;
	dev_man->server->Set_Recv_Timeout(NETWORKING_NO_TIMEOUT);
	dev_man->server->Set_High_Priority();
	dev_man->receiving = true;
	dev_man->recv_thr = new thread(Receive_Thread, dev_man);
}

void Disconnect_Server_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	if (dev_man->server) {
		dev_man->server->CloseConnection();
	}
	dev_man->receiving = false;
	if (dev_man->recv_thr) {
		dev_man->recv_thr->join();
		delete dev_man->recv_thr;
		dev_man->recv_thr = NULL;
	}
	dev_man->server = NULL;
}

// Linux specific code {{{
#ifdef __linux__
bool Send_Keyboard_Data(DEVICE_MANAGER* dev_man) {
	uint8_t ptype = KEY_PACKET;
	int len = DEVICE_MANAGER::Keyboard_Events.size();
	for (int i = 0; i < len; i++) {
		KEYBOARD_EVENT_T* k_event = NULL;
		DEVICE_MANAGER::Keyboard_Events.pop(k_event);
		if (!k_event) {
			log_err(((const JSON_TYPE){
				{"message", "expected keyboard event, found null"},
				JSON_TYPE_END}));
			continue;
		}
		if (!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)k_event,
			sizeof(KEYBOARD_EVENT_T))) {

			log_err(((const JSON_TYPE){
				{"message", "could not send keyboard event"},
				JSON_TYPE_END}));
			return false;
		}
		delete k_event;
	}
	return true;
}

bool Send_Mouse_Data(DEVICE_MANAGER* dev_man) {
	uint8_t ptype = MOUSE_PACKET;
	int len = DEVICE_MANAGER::Mouse_Events.size();
	for (int i = 0; i < len; i++) {
		MOUSE_EVENT_T* m_event = NULL;
		DEVICE_MANAGER::Mouse_Events.pop(m_event);
		if (!m_event) {
			log_err(((const JSON_TYPE){
				{"message", "expected mouse event, found null"},
				JSON_TYPE_END}));
			continue;
		}
		if (!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)m_event,
			sizeof(MOUSE_EVENT_T))) {

			log_err(((const JSON_TYPE){
				{"message", "failed to send mouse event"},
				JSON_TYPE_END}));
			return false;
		}
		delete m_event;
	}

	return true;
}

void Send_Thread(DEVICE_MANAGER* dev_man) {
	while (dev_man->sending) {
		if (!Send_Keyboard_Data(dev_man) || !Send_Mouse_Data(dev_man)) {
			dev_man->sending = false;
		}
		Sleep_Milli(16);
	}

	log_dbg(((const JSON_TYPE){
		{"message", "send thread going down"},
		JSON_TYPE_END}));
}

void Connect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Client* client) {
	Disconnect_Client_DEVICE_MANAGER(dev_man);
	dev_man->client = client;
	dev_man->sending = true;
	dev_man->send_thr = new thread(Send_Thread, dev_man);
}

void Disconnect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	if (dev_man->client) {
		dev_man->client->CloseConnection();
	}
	dev_man->sending = false;
	if (dev_man->send_thr) {
		dev_man->send_thr->join();
		delete dev_man->send_thr;
		dev_man->send_thr = NULL;
	}
	dev_man->client = NULL;
}

void Delete_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	log_dbg(((const JSON_TYPE){
		{"message", "deleting device manager"},
		JSON_TYPE_END}));
	Disconnect_Server_DEVICE_MANAGER(dev_man);
	Disconnect_Client_DEVICE_MANAGER(dev_man);

	log_dbg(((const JSON_TYPE){
		{"message", "done deleting device manager"},
		JSON_TYPE_END}));
	#ifdef _WIN64
	delete dev_man->client_mtx;
	#endif
	*dev_man = NULLIFY;
}

#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Send_Keyboard_Data(DEVICE_MANAGER* dev_man, KEYBOARD_EVENT_T* k_event) {
	uint8_t ptype;

	dev_man->client_mtx->lock();
	if (dev_man->client) {
		ptype = KEY_PACKET;
		if (
			!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)k_event, sizeof(KEYBOARD_EVENT_T))) {
			log_err("could not send keyboard data");
			dev_man->client = NULL;
		}
		delete k_event;
	}
	else {
		DEVICE_MANAGER::Keyboard_Events.push(k_event);
	}
	dev_man->client_mtx->unlock();
}

void Send_Mouse_Data(DEVICE_MANAGER* dev_man, MOUSE_EVENT_T* m_event) {
	uint8_t ptype;

	MOUSE::Current_X = m_event->x;
	MOUSE::Current_Y = m_event->y;

	dev_man->client_mtx->lock();
	if (dev_man->client) {
		ptype = MOUSE_PACKET;
		if (
			!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)m_event, sizeof(MOUSE_EVENT_T))) {
			log_err("could not send mouse data");
			dev_man->client = NULL;
		}
		delete m_event;
	}
	else {
		DEVICE_MANAGER::Mouse_Events.push(m_event);
	}
	dev_man->client_mtx->unlock();
}

void Connect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man, Client* client) {
	dev_man->client_mtx->lock();
	dev_man->client = client;
	dev_man->client_mtx->unlock();
}

void Disconnect_Client_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	dev_man->client_mtx->lock();
	dev_man->client = NULL;
	dev_man->client_mtx->unlock();
}

void Delete_DEVICE_MANAGER(DEVICE_MANAGER* dev_man) {
	delete dev_man->client_mtx;
}
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}
