#include "WS_SERVER.h"

bool Initialize_WS_SERVER(WS_SERVER *server, WS_SERVER_MASTER *ws_master,
	int id) {

	server->ws_master = ws_master;
	server->recv_timeout = 100;
	Set_Name_WS_SERVER(server, "unknown");
	
	return true;
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

bool Accept_WS_SERVER(WS_SERVER *server) {
	uint8_t test_buff[TEST_BUFF_SIZE];

	server->server_id = Register_Vhost_WS_SERVER_MASTER(server->ws_master);

	if (!Set_Recv_Timeout_WS_SERVER(server, 5 /*DEFAULT_RECV_TIMEOUT*/, 0)) {
		return false;
	}

	return true;
}

bool Send_WS_SERVER(WS_SERVER *server, char *buffer, int size) {
	return Send_WS_SERVER_MASTER(server->ws_master, (uint8_t*) buffer, size,
		server->server_id);
}

bool Receive_WS_SERVER(WS_SERVER *server, char *buffer, int size) {
	return Receive_WS_SERVER_MASTER(server->ws_master, (uint8_t*) buffer,
		size, server->recv_timeout, server->server_id);
}

int Receive_Unsafe_WS_SERVER(WS_SERVER *server, char *buffer) {
	return Receive_Unsafe_WS_SERVER_MASTER(server->ws_master, (uint8_t*) buffer,
		server->recv_timeout, server->server_id);
}

void Delete_WS_SERVER(WS_SERVER *server) {
	//lord... please
}
