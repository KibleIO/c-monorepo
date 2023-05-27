#include "WS_SERVER.h"

bool Initialize_WS_SERVER(WS_SERVER *server,
	Receive_Callback_SOCKET_SERVER callback,
	SOCKET_SERVER_REGISTRY *registry, KCONTEXT* ctx, void *user_ptr) {

	server->ctx = ctx;

	server->ws_master = 
		&registry->root_sockets[ROOT_SOCKET_TYPE_WS]->ws_server_master;
	Set_Name_WS_SERVER(server, "unknown");
	
	if (!Set_Recv_Timeout_WS_SERVER(server, WS_RECV_TIMEOUT, 0)) {
		return false;
	}

	server->server_id = Register_Vhost_WS_SERVER_MASTER(server->ws_master,
		callback, user_ptr);

	return true;
}

void Delete_WS_SERVER(WS_SERVER *server) {
	//lord... please
}

bool Send_WS_SERVER(WS_SERVER *server, char *buffer, int size) {
	return Send_WS_SERVER_MASTER(server->ws_master, (uint8_t*) buffer, size,
		server->server_id);
}

void Set_Name_WS_SERVER(WS_SERVER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_WS_SERVER(WS_SERVER *server, int sec, int usec) {
	server->recv_timeout = (sec * 1000) + (usec / 1000);
	return true;
}

bool Set_High_Priority_WS_SERVER(WS_SERVER *server) {
	return true;
}