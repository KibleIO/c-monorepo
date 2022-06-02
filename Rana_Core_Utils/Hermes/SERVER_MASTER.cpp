#include "SERVER_MASTER.h"

bool Initialize_SERVER_MASTER(SERVER_MASTER *server, KCONTEXT *ctx, int type,
	int port) {

	server->ctx = ctx;
	server->type = type;

	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Initialize_TCP_SERVER_MASTER(
				&server->tcp_server_master, ctx, port);
		case NETWORK_TYPE_UDP:
			return Initialize_UDP_SERVER_MASTER(
				&server->udp_server_master, ctx, port);
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
	return false;
}

void Set_Name_SERVER_MASTER(SERVER_MASTER *server, char *name) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			Set_Name_TCP_SERVER_MASTER(&server->tcp_server_master,
				name);
			return;
		case NETWORK_TYPE_UDP:
			Set_Name_UDP_SERVER_MASTER(&server->udp_server_master,
				name);
			return;
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
}

bool Set_Recv_Timeout_SERVER_MASTER(SERVER_MASTER *server, int sec, int usec) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Set_Recv_Timeout_TCP_SERVER_MASTER(
				&server->tcp_server_master, sec, usec);
		case NETWORK_TYPE_UDP:
			return Set_Recv_Timeout_UDP_SERVER_MASTER(
				&server->udp_server_master, sec, usec);
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
	return false;
}

bool Set_High_Priority_SERVER_MASTER(SERVER_MASTER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Set_High_Priority_TCP_SERVER_MASTER(
				&server->tcp_server_master);
		case NETWORK_TYPE_UDP:
			return Set_High_Priority_UDP_SERVER_MASTER(
				&server->udp_server_master);
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
	return false;
}

void Delete_SERVER_MASTER(SERVER_MASTER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			Delete_TCP_SERVER_MASTER(&server->tcp_server_master);
			return;
		case NETWORK_TYPE_UDP:
			Delete_UDP_SERVER_MASTER(&server->udp_server_master);
			return;
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
}
