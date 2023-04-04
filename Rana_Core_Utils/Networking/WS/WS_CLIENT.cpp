#include "WS_CLIENT.h"

bool Initialize_WS_CLIENT(WS_CLIENT *client, WS_CLIENT_MASTER *master, int id) {
	client->ws_master = master;
	client->recv_timeout = 100;
	Set_Name_WS_CLIENT(client, "unknown");

	return true;
}

void Set_Name_WS_CLIENT(WS_CLIENT *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_WS_CLIENT(WS_CLIENT *client, int sec, int usec) {
	client->recv_timeout = (sec * 1000) + (usec / 1000);
	return true;
}

bool Set_High_Priority_WS_CLIENT(WS_CLIENT *client) {
	return true;
}

bool Connect_WS_CLIENT(WS_CLIENT *client) {
	client->client_id = Register_Vhost_WS_CLIENT_MASTER(client->ws_master);

	if (!Set_Recv_Timeout_WS_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	return true;
}

bool Send_WS_CLIENT(WS_CLIENT *client, char *buffer, int size) {
	return Send_WS_CLIENT_MASTER(client->ws_master, (uint8_t*) buffer, size,
		client->client_id);
}

bool Receive_WS_CLIENT(WS_CLIENT *client, char *buffer, int size) {
	return Receive_WS_CLIENT_MASTER(client->ws_master, (uint8_t*) buffer,
		size, client->recv_timeout, client->client_id);
}

int Receive_Unsafe_WS_CLIENT(WS_CLIENT *client, char *buffer) {
	return Receive_Unsafe_WS_CLIENT_MASTER(client->ws_master, (uint8_t*) buffer,
		client->recv_timeout, client->client_id);
}

void Delete_WS_CLIENT(WS_CLIENT *client) {
}