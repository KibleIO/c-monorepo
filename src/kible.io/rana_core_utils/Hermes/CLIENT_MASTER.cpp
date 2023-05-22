#include "CLIENT_MASTER.h"

bool Initialize_CLIENT_MASTER(CLIENT_MASTER *client, KCONTEXT *ctx, int type,
	int port, char *ip) {

	client->ctx = ctx;
	client->type = type;

	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Initialize_TCP_CLIENT_MASTER(
				&client->tcp_client_master, ctx, port, ip);
		case NETWORK_TYPE_UDP:
			return Initialize_UDP_CLIENT_MASTER(
				&client->udp_client_master, ctx, port, ip);
		case NETWORK_TYPE_WS:
			return Initialize_WS_CLIENT_MASTER(
				&client->ws_client_master, ctx, port, ip);
	}
	LOGGER_ERROR(client->ctx, {
		{"message", "Unknown type"},
		{"net_type", client->type},
	});
	return false;
}

void Set_Name_CLIENT_MASTER(CLIENT_MASTER *client, char *name) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			Set_Name_TCP_CLIENT_MASTER(&client->tcp_client_master,
				name);
		case NETWORK_TYPE_UDP:
			Set_Name_UDP_CLIENT_MASTER(&client->udp_client_master,
				name);
		case NETWORK_TYPE_WS:
			Set_Name_WS_CLIENT_MASTER(&client->ws_client_master,
				name);
	}
	LOGGER_ERROR(client->ctx, {
		{"message", "Unknown type"},
		{"net_type", client->type},
	});
}

bool Set_Recv_Timeout_CLIENT_MASTER(CLIENT_MASTER *client, int sec, int usec) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Set_Recv_Timeout_TCP_CLIENT_MASTER(
				&client->tcp_client_master, sec, usec);
		case NETWORK_TYPE_UDP:
			return Set_Recv_Timeout_UDP_CLIENT_MASTER(
				&client->udp_client_master, sec, usec);
		case NETWORK_TYPE_WS:
			return Set_Recv_Timeout_WS_CLIENT_MASTER(
				&client->ws_client_master, sec, usec);
	}
	LOGGER_ERROR(client->ctx, {
		{"message", "Unknown type"},
		{"net_type", client->type},
	});
	return false;
}

bool Set_High_Priority_CLIENT_MASTER(CLIENT_MASTER *client) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Set_High_Priority_TCP_CLIENT_MASTER(
				&client->tcp_client_master);
		case NETWORK_TYPE_UDP:
			return Set_High_Priority_UDP_CLIENT_MASTER(
				&client->udp_client_master);
		case NETWORK_TYPE_WS:
			return Set_High_Priority_WS_CLIENT_MASTER(
				&client->ws_client_master);
	}
	LOGGER_ERROR(client->ctx, {
		{"message", "Unknown type"},
		{"net_type", client->type},
	});
	return false;
}

void Delete_CLIENT_MASTER(CLIENT_MASTER *client) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			Delete_TCP_CLIENT_MASTER(&client->tcp_client_master);
			return;
		case NETWORK_TYPE_UDP:
			Delete_UDP_CLIENT_MASTER(&client->udp_client_master);
			return;
		case NETWORK_TYPE_WS:
			Delete_WS_CLIENT_MASTER(&client->ws_client_master);
			return;
	}
	LOGGER_ERROR(client->ctx, {
		{"message", "Unknown type"},
		{"net_type", client->type},
	});
}
