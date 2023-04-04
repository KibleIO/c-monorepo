#include "WS_CLIENT_MASTER.h"

#ifdef __EMSCRIPTEN__

EM_BOOL On_Open_WS_CLIENT_MASTER(int eventType,
	const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {

	WS_CLIENT_MASTER *client = (WS_CLIENT_MASTER*) userData;

	client->socket = websocketEvent->socket;
	client->accept = true;
	return EM_TRUE;
}

EM_BOOL On_Error_WS_CLIENT_MASTER(int eventType,
	const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData) {
	
	WS_CLIENT_MASTER *client = (WS_CLIENT_MASTER*) userData;
	client->accept = false;
	return EM_TRUE;
}

EM_BOOL On_Close_WS_CLIENT_MASTER(int eventType,
	const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) {

	WS_CLIENT_MASTER *client = (WS_CLIENT_MASTER*) userData;
	client->accept = false;
	return EM_TRUE;
}

EM_BOOL On_Message_WS_CLIENT_MASTER(int eventType,
	const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
	
	WS_CLIENT_MASTER *client = (WS_CLIENT_MASTER*) userData;
	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;

	int len = websocketEvent->numBytes;

	if (int(websocketEvent->data[0]) < 0 ||
		int(websocketEvent->data[0]) >= MAX_HOSTS) {
		log_err("invalid host id.");
		return EM_FALSE;
	}

	if (client->active_read[int(websocketEvent->data[0])]->size() >=
		MAX_ACCUMULATED_FRAMES) {

		//a reader is getting lazy... drop this packet
		log_err("a reader got lazy, dropping this packet.");
		return EM_FALSE;
	}

	while (client->pool->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT) {

		time_out++;
		Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT) {
		return EM_FALSE;
	}

	client->pool->pop(temp);

	if (len > MAX_WEBSOCKET_PACKET_SIZE) {
		log_err("received large packet. truncated.");
		len = MAX_WEBSOCKET_PACKET_SIZE;
	}

	copy((uint8_t*)websocketEvent->data + 1,
		((uint8_t*)websocketEvent->data + 1) + (len - 1), temp->bytes);
	temp->size = len - 1;

	client->active_read[int(websocketEvent->data[0])]->push(temp);

	return EM_TRUE;
}

#endif

bool Initialize_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client,
	int port, char *ip) {

	WEBSOCKET_ELEMENT *temp;
	
	client->host_count = 0;
	client->accept = false;
	client->pool = new Queue<WEBSOCKET_ELEMENT*>;

	for (int i = 0; i < MAX_HOSTS; i++) {
		client->active_read[i] = new Queue<WEBSOCKET_ELEMENT*>;
	}

	for (int i = 0; i < WEB_SOCKET_POOL_SIZE; i++) {
		temp		= new WEBSOCKET_ELEMENT;
		temp->size	= -1;
		temp->bytes	= new uint8_t[MAX_WEBSOCKET_PACKET_SIZE];

		client->pool->push(temp);
	}

	Set_Name_WS_CLIENT_MASTER(client, "unknown");

	#ifdef __EMSCRIPTEN__

	if (!emscripten_websocket_is_supported()) {
		return false;
	}

	string address = "ws://";
	address += ip;
	address += ":";
	address += to_string(port);

	EmscriptenWebSocketCreateAttributes ws_attrs = {
		address.c_str(), NULL, EM_TRUE
	};

	EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(&ws_attrs);
	emscripten_websocket_set_onopen_callback(ws, (void*) client,
		On_Open_WS_CLIENT_MASTER);
	emscripten_websocket_set_onerror_callback(ws, (void*) client,
		On_Error_WS_CLIENT_MASTER);
	emscripten_websocket_set_onclose_callback(ws, (void*) client,
		On_Close_WS_CLIENT_MASTER);
	emscripten_websocket_set_onmessage_callback(ws, (void*) client,
		On_Message_WS_CLIENT_MASTER);
	
	while (!client->accept) {
		Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
	}

	#endif

	return true;
}

bool Send_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client,
	uint8_t* bytes, uint32_t size, uint8_t client_index) {

	if (size > (MAX_WEBSOCKET_PACKET_SIZE - 1)) {
		//log_err("sending large packet. truncated.");
		size = MAX_WEBSOCKET_PACKET_SIZE - 1;
	}

	uint8_t	send_bytes[MAX_WEBSOCKET_PACKET_SIZE];

	send_bytes[0] = client_index;
	copy(bytes, bytes + size, &send_bytes[1]);

	#ifdef __EMSCRIPTEN__

	emscripten_websocket_send_binary(client->socket, send_bytes, size + 1);

	#endif

	return true;
}

bool Receive_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client,
	uint8_t* bytes, int32_t size, int32_t recv_timeout,
	uint8_t client_index) {

	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;
	uint8_t return_val;
	int32_t sleep = recv_timeout / WEB_SOCKET_TIME_OUT;

	while (client->active_read[client_index]->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT &&
		client->accept) {

		time_out++;
		Sleep_Milli(sleep);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT ||
		!client->accept) {

		return false;
	}

	client->active_read[client_index]->pop(temp);

	copy(temp->bytes, temp->bytes + size, bytes);
	return_val = size == temp->size;
	temp->size = -1;

	client->pool->push(temp);

	return return_val;
}

int Receive_Unsafe_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client,
	uint8_t* bytes, int32_t recv_timeout,
	uint8_t client_index) {

	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;
	uint8_t return_val;
	int32_t sleep = recv_timeout / WEB_SOCKET_TIME_OUT;

	while (client->active_read[client_index]->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT &&
		client->accept) {

		time_out++;
		Sleep_Milli(sleep);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT ||
		!client->accept) {

		return false;
	}

	client->active_read[client_index]->pop(temp);

	copy(temp->bytes, temp->bytes + temp->size, bytes);
	return_val = temp->size;
	temp->size = -1;

	client->pool->push(temp);

	return return_val;
}

uint8_t Register_Vhost_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client) {
	if (client->host_count >= MAX_HOSTS) {
		log_err("Too many hosts.");
		return -1;
	}
	return client->host_count++;
}

void Set_Name_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client, int sec,
	int usec) {

	return true;
}

bool Set_High_Priority_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client) {
	return true;
}

void Delete_WS_CLIENT_MASTER(WS_CLIENT_MASTER *client) {
}
