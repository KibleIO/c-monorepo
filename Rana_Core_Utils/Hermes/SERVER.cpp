#include "SERVER.h"

bool Initialize_SERVER(SERVER *server, CONTEXT *ctx, SERVER_MASTER *master,
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
}

bool Set_Recv_Timeout_SERVER(SERVER *server, int sec, int usec) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Set_Recv_Timeout_TCP_SERVER(&server->tcp_server,
				sec, usec);
		case NETWORK_TYPE_UDP:
			return Set_Recv_Timeout_UDP_SERVER(&server->udp_server,
				sec, usec);
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
	return false;
}

bool Accept_SERVER(SERVER *server) {
	switch(server->type) {
		case NETWORK_TYPE_TCP:
			return Accept_TCP_SERVER(&server->tcp_server);
		case NETWORK_TYPE_UDP:
			return Accept_UDP_SERVER(&server->udp_server);
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
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
	}
	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", server->type);
	}
}
