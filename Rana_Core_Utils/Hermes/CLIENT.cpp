#include "CLIENT.h"

bool Initialize_CLIENT(CLIENT *client, CONTEXT *ctx, CLIENT_MASTER *master,
	int id) {

	client->ctx = ctx;
	client->type = master->type;
	client->master = master;

	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Initialize_TCP_CLIENT(&client->tcp_client, ctx,
				&master->tcp_client_master, id);
		case NETWORK_TYPE_UDP:
			return Initialize_UDP_CLIENT(&client->udp_client, ctx,
				&master->udp_client_master, id);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

void Set_Name_CLIENT(CLIENT *client, char *name) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			Set_Name_TCP_CLIENT(&client->tcp_client, name);
			return;
		case NETWORK_TYPE_UDP:
			Set_Name_UDP_CLIENT(&client->udp_client, name);
			return;
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
}

bool Set_Recv_Timeout_CLIENT(CLIENT *client, int sec, int usec) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Set_Recv_Timeout_TCP_CLIENT(&client->tcp_client,
				sec, usec);
		case NETWORK_TYPE_UDP:
			return Set_Recv_Timeout_UDP_CLIENT(&client->udp_client,
				sec, usec);

	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

bool Set_High_Priority_CLIENT(CLIENT *client) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Set_High_Priority_TCP_CLIENT(
				&client->tcp_client);
		case NETWORK_TYPE_UDP:
			return Set_High_Priority_UDP_CLIENT(
				&client->udp_client);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

bool Connect_CLIENT(CLIENT *client) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Connect_TCP_CLIENT(&client->tcp_client);
		case NETWORK_TYPE_UDP:
			return Connect_UDP_CLIENT(&client->udp_client);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

bool Send_CLIENT(CLIENT *client, char *buffer, int size) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Send_TCP_CLIENT(&client->tcp_client, buffer,
				size);
		case NETWORK_TYPE_UDP:
			return Send_UDP_CLIENT(&client->udp_client, buffer,
				size);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

bool Receive_CLIENT(CLIENT *client, char *buffer, int size) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Receive_TCP_CLIENT(&client->tcp_client, buffer,
				size);
		case NETWORK_TYPE_UDP:
			return Receive_UDP_CLIENT(&client->udp_client, buffer,
				size);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

int Receive_Unsafe_CLIENT(CLIENT *client, char *buffer) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			return Receive_Unsafe_TCP_CLIENT(&client->tcp_client,
				buffer);
		case NETWORK_TYPE_UDP:
			return Receive_Unsafe_UDP_CLIENT(&client->udp_client,
				buffer);
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
	return false;
}

void Delete_CLIENT(CLIENT *client) {
	switch(client->type) {
		case NETWORK_TYPE_TCP:
			Delete_TCP_CLIENT(&client->tcp_client);
			return;
		case NETWORK_TYPE_UDP:
			Delete_UDP_CLIENT(&client->udp_client);
			return;
	}
	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Unknown type");
		ADD_INT_LOG("net_type", client->type);
	}
}
