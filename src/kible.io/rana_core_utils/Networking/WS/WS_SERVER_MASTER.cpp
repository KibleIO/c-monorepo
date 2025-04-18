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
			lws_set_timer_usecs(wsi, LWS_SLEEP_TIME);
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
			delete [] temp->bytes;

			server->pool->push(temp);
			break;
		case LWS_CALLBACK_RECEIVE:
			if (int(((uint8_t*) in)[0]) < 0 ||
				int(((uint8_t*) in)[0]) >= MAX_HOSTS) {
				//log_err("invalid host id.");
				break;
			}

			if (server->consumers[int(((uint8_t*) in)[0])] != NULL) {
				server->consumers[int(((uint8_t*) in)[0])]->callback(
					server->consumers[int(((uint8_t*) in)[0])]->user_ptr,
					(char*)in + 1,
					len - 1);
			}
            		break;
		case LWS_CALLBACK_CLOSED:
			server->accept = false;
			break;
		case LWS_CALLBACK_TIMER:
			lws_set_timer_usecs(wsi, LWS_SLEEP_TIME);
			break;
	}
	return 0;
}


bool Initialize_WS_SERVER_MASTER(WS_SERVER_MASTER *server, KCONTEXT *ctx,
	int port) {
	
	WEBSOCKET_ELEMENT *temp;
	int counter = WS_CONNECT_TIMEOUT;

	server->ctx = ctx;
	server->pool		= new Queue<WEBSOCKET_ELEMENT*>;
	server->active_write	= new Queue<WEBSOCKET_ELEMENT*>;

	for (int i = 0; i < MAX_HOSTS; i++) {
		server->consumers[i] = NULL;
	}

	for (int i = 0; i < WEB_SOCKET_POOL_SIZE; i++) {
		temp		= new WEBSOCKET_ELEMENT;
		temp->size	= -1;
		//temp->bytes	= new uint8_t[MAX_WEBSOCKET_PACKET_SIZE];

		server->pool->push(temp);
	}

	Set_Name_WS_SERVER_MASTER(server, "master");

	server->host_count = 0;
	server->accept = false;
	server->port = port;
	server->running = true;
	server->main_thread = new thread(
		Service_Thread_WS_SERVER_MASTER, server);
	
	while (!server->accept && counter-- > 0) {
		Sleep_Milli(WEB_SOCKET_SLEEP_TIME);
	}

	return counter > 0;
}

void Delete_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	WEBSOCKET_ELEMENT *temp = NULL;
	
	server->accept = false;
	server->running = false;
	if (server->main_thread != NULL) {
		server->main_thread->join();
		delete server->main_thread;
		server->main_thread = NULL;
	}

	if (server->context != NULL) {
		lws_context_destroy(server->context);
		server->context = NULL;
	}

	//drain active_write
	if (server->active_write != NULL) {
		while (server->active_write->size() > 0) {
			server->active_write->pop(temp);
			delete [] temp->bytes;
			delete temp;
		}
		delete server->active_write;
		server->active_write = NULL;
	}

	//drain pool
	if (server->pool != NULL) {
		while (server->pool->size() > 0) {
			server->pool->pop(temp);
			delete temp;
		}
		delete server->pool;
		server->pool = NULL;
	}

	//cleanout consumers
	for (int i = 0; i < MAX_HOSTS; i++) {
		if (server->consumers[i] != NULL) {
			delete server->consumers[i];
			server->consumers[i] = NULL;
		}
	}
}

void Service_Thread_WS_SERVER_MASTER(WS_SERVER_MASTER *server) {
	lws_context_creation_info context_info;
	memset(&context_info, 0, sizeof(context_info));

	context_info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

	server->context = lws_create_context(&context_info);
	if (server->context == NULL) {
		//log_err("failed to init master websocket context");
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
		//log_err("failed to initialize web socket server");
		return;
	}

	while (server->running) {
		lws_service(server->context, LWS_SLEEP_TIME);

		if (server->active_write->size() > 0) {
			lws_callback_on_writable_all_protocol_vhost(
			server->vhost, lws_vhost_name_to_protocol(
				server->vhost,
				"dumb-increment-protocol"));
		}
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

	/*
	if (size > (MAX_WEBSOCKET_PACKET_SIZE - 1)) {
		//log_err("sending large packet. truncated.");
		size = MAX_WEBSOCKET_PACKET_SIZE - 1;
	}
	*/

	server->pool->pop(temp);

	temp->bytes = new uint8_t[LWS_SEND_BUFFER_PRE_PADDING + 1 + size];
	temp->bytes[LWS_SEND_BUFFER_PRE_PADDING] = server_index;
	copy(bytes, bytes + size, &temp->bytes[LWS_SEND_BUFFER_PRE_PADDING + 1]);
	temp->size = size + 1;

	server->active_write->push(temp);
	return true;
}

uint8_t Register_Vhost_WS_SERVER_MASTER(WS_SERVER_MASTER *server,
	Receive_Callback_SOCKET_SERVER callback, void *user_ptr) {

	if (server->host_count >= MAX_HOSTS) {
		//log_err("Too many hosts.");
		return -1;
	}

	server->consumers[server->host_count] = new WEBSOCKET_CONSUMER;
	server->consumers[server->host_count]->user_ptr = user_ptr;
	server->consumers[server->host_count]->callback = callback;

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
