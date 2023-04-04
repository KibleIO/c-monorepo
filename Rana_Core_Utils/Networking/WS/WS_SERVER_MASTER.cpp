#include "WS_SERVER_MASTER.h"

int callback_dumb_increment(lws* wsi, lws_callback_reasons reason,
	void* user, void* in, size_t len) {

	(void) user;

	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;

	if (lws_get_vhost(wsi) == NULL) {
		return 0;
	}

	WS_SERVER_MASTER *server = (WS_SERVER_MASTER*)
		lws_vhost_user(lws_get_vhost(wsi));

	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			server->accept = true;
			lws_set_timer_usecs(wsi, 5000);
			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (server->active_write->size() <= 0) {
				break;
			}

			server->active_write->pop(temp);

			lws_write(wsi, (unsigned char*)
			&temp->bytes[LWS_SEND_BUFFER_PRE_PADDING], temp->size,
			LWS_WRITE_BINARY);

			temp->size = -1;

			if (server->active_write->size() > 0) {
				lws_callback_on_writable_all_protocol_vhost(
				lws_get_vhost(wsi), lws_get_protocol(wsi));
			}

			server->pool->push(temp);
			break;
		case LWS_CALLBACK_RECEIVE:
			if (int(((uint8_t*) in)[0]) < 0 || int(((uint8_t*) in)[0]) >= MAX_HOSTS) {
				log_err("invalid host id.");
				break;
			}

			if (server->active_read[int(((uint8_t*) in)[0])]->size() >=
				MAX_ACCUMULATED_FRAMES) {

				//a reader is getting lazy... drop this packet
				log_err("a reader got lazy, dropping this packet.");
				break;
			}

			while (server->pool->size() <= 0 &&
				time_out < WEB_SOCKET_TIME_OUT) {

				time_out++;
				Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
			}
			if (time_out >= WEB_SOCKET_TIME_OUT) {
				break;
			}

			server->pool->pop(temp);

			if (len > MAX_WEBSOCKET_PACKET_SIZE) {
				log_err("received large packet. truncated.");
				len = MAX_WEBSOCKET_PACKET_SIZE;
			}

			copy((uint8_t*)in + 1, ((uint8_t*)in + 1) + (len - 1),
				temp->bytes);
			temp->size = len - 1;

			server->active_read[int(((uint8_t*) in)[0])]->push(temp);
            		break;
		case LWS_CALLBACK_CLOSED:
			server->accept = false;
			break;
		case LWS_CALLBACK_TIMER:
			lws_callback_on_writable_all_protocol_vhost(
				lws_get_vhost(wsi),
				lws_vhost_name_to_protocol(
				lws_get_vhost(wsi),
				"dumb-increment-protocol"));
			lws_set_timer_usecs(wsi, 5000);
			break;
	}
	return 0;
}

bool Initialize_WS_SERVER_MASTER(WS_SERVER_MASTER *server, int port) {
	WEBSOCKET_ELEMENT *temp;

	server->pool		= new Queue<WEBSOCKET_ELEMENT*>;
	server->active_write	= new Queue<WEBSOCKET_ELEMENT*>;

	for (int i = 0; i < MAX_HOSTS; i++) {
		server->active_read[i] = new Queue<WEBSOCKET_ELEMENT*>;
	}

	for (int i = 0; i < WEB_SOCKET_POOL_SIZE; i++) {
		temp		= new WEBSOCKET_ELEMENT;
		temp->size	= -1;
		temp->bytes	= new uint8_t[MAX_WEBSOCKET_PACKET_SIZE];

		server->pool->push(temp);
	}

	Set_Name_WS_SERVER_MASTER(server, "master");

	server->host_count = 0;
	server->accept = false;
	server->port = port;
	server->running = true;
	server->main_thread = new thread(
		Service_Thread_WS_SERVER_MASTER, server);
	
	while (!server->accept) {
		Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
	}

	return true;
}

void Service_Thread_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	lws_context_creation_info context_info;
	memset(&context_info, 0, sizeof(context_info));

	context_info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

	server->context = lws_create_context(&context_info);
	if (server->context == NULL) {
		log_err("failed to init master websocket context");
		return;
	}

	lws_protocols protocols[NUM_PROTOCOLS];
	DEFAULT_PROTOCOLS(protocols);

	context_info.port = server->port;
	context_info.protocols = protocols;
	context_info.gid = -1;
	context_info.uid = -1;
	context_info.user = (void*) server;
	context_info.vhost_name = "helloworld";
	
	server->vhost = lws_create_vhost(server->context, &context_info);

	if (server->vhost == NULL) {
		log_err("failed to initialize web socket server");
		return;
	}

	while (server->running) {
		lws_service(server->context, LWS_SLEEP_TIME);
	}
}

bool Send_WS_SERVER_MASTER(WS_SERVER_MASTER *server,
	uint8_t* bytes, uint32_t size, uint8_t server_index) {
    
	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;

	while (server->pool->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT &&
		server->accept) {

		time_out++;
		Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT ||
		!server->accept) {

		return false;
	}

	if (size > (MAX_WEBSOCKET_PACKET_SIZE - 1)) {
		log_err("sending large packet. truncated.");
		size = MAX_WEBSOCKET_PACKET_SIZE - 1;
	}

	server->pool->pop(temp);

	temp->bytes[LWS_SEND_BUFFER_PRE_PADDING] = server_index;
	copy(bytes, bytes + size, &temp->bytes[LWS_SEND_BUFFER_PRE_PADDING + 1]);
	temp->size = size + 1;

	server->active_write->push(temp);
	return true;
}

bool Receive_WS_SERVER_MASTER(WS_SERVER_MASTER *server,
	uint8_t* bytes, int32_t size, int32_t recv_timeout,
	uint8_t server_index) {

	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;
	uint8_t return_val;
	int32_t sleep = recv_timeout / WEB_SOCKET_TIME_OUT;

	while (server->active_read[server_index]->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT &&
		server->accept) {

		time_out++;
		Sleep_Milli(sleep);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT ||
		!server->accept) {

		return false;
	}

	server->active_read[server_index]->pop(temp);

	copy(temp->bytes, temp->bytes + size, bytes);
	return_val = size == temp->size;
	temp->size = -1;

	server->pool->push(temp);

	return return_val;
}

int Receive_Unsafe_WS_SERVER_MASTER(WS_SERVER_MASTER *server,
	uint8_t* bytes, int32_t recv_timeout,
	uint8_t server_index) {

	WEBSOCKET_ELEMENT *temp = NULL;
	uint32_t time_out = 0;
	uint8_t return_val;
	int32_t sleep = recv_timeout / WEB_SOCKET_TIME_OUT;

	while (server->active_read[server_index]->size() <= 0 &&
		time_out < WEB_SOCKET_TIME_OUT &&
		server->accept) {

		time_out++;
		Sleep_Milli(sleep);
	}
	if (time_out >= WEB_SOCKET_TIME_OUT ||
		!server->accept) {

		return false;
	}

	server->active_read[server_index]->pop(temp);

	copy(temp->bytes, temp->bytes + temp->size, bytes);
	return_val = temp->size;
	temp->size = -1;

	server->pool->push(temp);

	return return_val;
}

uint8_t Register_Vhost_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	if (server->host_count >= MAX_HOSTS) {
		log_err("Too many hosts.");
		return -1;
	}
	return server->host_count++;
}

void Set_Name_WS_SERVER_MASTER(WS_SERVER_MASTER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_WS_SERVER_MASTER(WS_SERVER_MASTER *server, int sec,
	int usec) {
	
	return true;
}

bool Set_High_Priority_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	return true;
}

void Delete_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	//lord please have mercy on my soul
}
