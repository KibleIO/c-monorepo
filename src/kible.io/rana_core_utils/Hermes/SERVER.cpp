#include "SERVER.h"

bool Initialize_SERVER(SERVER *server, KCONTEXT *ctx, SERVER_MASTER *master,
	int id) {

	server->ctx = ctx;
	server->type = master->type;
	server->master = master;

	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Initialize_TCP_SERVER(&server->tcp_server, ctx,
				&master->tcp_server_master, id);
		case NETWORK_TYPE_UDP:
			return Initialize_UDP_SERVER(&server->udp_server, ctx,
				&master->udp_server_master, id);
		case NETWORK_TYPE_WS:
			return Initialize_WS_SERVER(&server->ws_server, ctx,
				&master->ws_server_master, id);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

void Set_Name_SERVER(SERVER *server, char *name) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			Set_Name_TCP_SERVER(&server->tcp_server, name);
			return;
		case NETWORK_TYPE_UDP:
			Set_Name_UDP_SERVER(&server->udp_server, name);
			return;
		case NETWORK_TYPE_WS:
			Set_Name_WS_SERVER(&server->ws_server, name);
			return;
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
}

bool Set_Recv_Timeout_SERVER(SERVER *server, int sec, int usec) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Set_Recv_Timeout_TCP_SERVER(&server->tcp_server,
				sec, usec);
		case NETWORK_TYPE_UDP:
			return Set_Recv_Timeout_UDP_SERVER(&server->udp_server,
				sec, usec);
		case NETWORK_TYPE_WS:
			return Set_Recv_Timeout_WS_SERVER(&server->ws_server,
				sec, usec);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

bool Set_High_Priority_SERVER(SERVER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Set_High_Priority_TCP_SERVER(
				&server->tcp_server);
		case NETWORK_TYPE_UDP:
			return Set_High_Priority_UDP_SERVER(
				&server->udp_server);
		case NETWORK_TYPE_WS:
			return Set_High_Priority_WS_SERVER(
				&server->ws_server);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

bool Accept_SERVER(SERVER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Accept_TCP_SERVER(&server->tcp_server);
		case NETWORK_TYPE_UDP:
			return Accept_UDP_SERVER(&server->udp_server);
		case NETWORK_TYPE_WS:
			return Accept_WS_SERVER(&server->ws_server);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

bool Send_SERVER(SERVER *server, char *buffer, int size) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Send_TCP_SERVER(&server->tcp_server, buffer,
				size);
		case NETWORK_TYPE_UDP:
			return Send_UDP_SERVER(&server->udp_server, buffer,
				size);
		case NETWORK_TYPE_WS:
			return Send_WS_SERVER(&server->ws_server, buffer,
				size);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

bool Receive_SERVER(SERVER *server, char *buffer, int size) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Receive_TCP_SERVER(&server->tcp_server, buffer,
				size);
		case NETWORK_TYPE_UDP:
			return Receive_UDP_SERVER(&server->udp_server, buffer,
				size);
		case NETWORK_TYPE_WS:
			return Receive_WS_SERVER(&server->ws_server, buffer,
				size);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

int Receive_Unsafe_SERVER(SERVER *server, char *buffer) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Receive_Unsafe_TCP_SERVER(&server->tcp_server,
				buffer);
		case NETWORK_TYPE_UDP:
			return Receive_Unsafe_UDP_SERVER(&server->udp_server,
				buffer);
		case NETWORK_TYPE_WS:
			return Receive_Unsafe_WS_SERVER(&server->ws_server,
				buffer);
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
	return false;
}

void Delete_SERVER(SERVER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			Delete_TCP_SERVER(&server->tcp_server);
			return;
		case NETWORK_TYPE_UDP:
			Delete_UDP_SERVER(&server->udp_server);
			return;
		case NETWORK_TYPE_WS:
			Delete_WS_SERVER(&server->ws_server);
			return;
	}
	LOGGER_ERROR(server->ctx, {
		{"message", "Unknown type"},
		{"net_type", server->type},
	});
}
