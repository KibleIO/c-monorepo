#include "TCP_CLIENT_MASTER.h"

bool Initialize_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client, KCONTEXT *ctx,
	int port, char *ip) {

	client->port = port;
	strcpy(client->ip, ip);
	Set_Name_TCP_CLIENT_MASTER(client, "unknown");

	return true;
}

void Set_Name_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client, char *name) {
	strcpy(client->name, name);
}

void Recv_Loop_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client) {
}

bool Set_Recv_Timeout_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client, int sec,
	int usec) {

	return true;
}

bool Set_High_Priority_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client) {
	return true;
}

void Delete_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER *client) {
}
