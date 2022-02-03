#include "UDP_SERVER_MASTER.h"

bool Initialize_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, CONTEXT *ctx,
	int port) {

	uint32_t input_var;

	server->ctx = ctx;
	server->port = port;
	Set_Name_UDP_SERVER_MASTER(server, "master");

	return true;
}

void Set_Name_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, int sec, int usec) {
	return true;
}

bool Set_High_Priority_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server) {
	return true;
}

void Delete_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server) {
}
