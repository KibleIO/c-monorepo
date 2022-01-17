#include "UDP_SERVER.h"

bool Initialize_UDP_SERVER(UDP_SERVER *server, CONTEXT *ctx) {
	uint32_t input_var;

	server->ctx = ctx;
	Set_Name_UDP_SERVER(server, "unknown");

	log_dbg(((const JSON_TYPE){
		{"message", "initializing udp server on port"},
		JSON_TYPE_END}));

	server->server_address_size = sizeof(server->server_address);
	server->client_address_size = sizeof(server->client_address);

	memset(&server->server_address, 0, server->server_address_size);
	memset(&server->client_address, 0, server->client_address_size);

	server->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server->sockfd < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Server socket failed to open");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	input_var = 1;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &input_var,
		sizeof(uint32_t)) == -1) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	input_var = 1000000;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_SNDBUF, &input_var,
		sizeof(uint32_t)) == -1) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: sndbuf");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if (!Set_Recv_Timeout_UDP_SERVER(server, 5, 0)) {
		return false;
	}

	return true;
}

void Set_Name_UDP_SERVER(UDP_SERVER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_UDP_SERVER(UDP_SERVER *server, int sec, int usec) {
	timeval o_to = { sec, usec };
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_RCVTIMEO, &o_to,
		sizeof(timeval)) == -1) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: timeout");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}
	return true;
}

bool Set_High_Priority_UDP_SERVER(UDP_SERVER *server) {
	int32_t o;
	o = 6;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: priority");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}
	return true;
}

bool Accept_UDP_SERVER(UDP_SERVER* server, int port) {
	int32_t size;
	uint8_t* test_buff;

	server->server_address.sin_family = AF_INET;
	server->server_address.sin_addr.s_addr = INADDR_ANY;
	server->server_address.sin_port = htons(port);

	if (bind(server->sockfd,
		(const struct sockaddr*) &server->server_address,
		server->server_address_size) < 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message",
				"Failed to bind UDP server socket");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	test_buff = new uint8_t[TEST_BUFF_SIZE];

	size = recvfrom(server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
		(sockaddr*)&server->client_address,
		(socklen_t*)&server->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "There was an error receiving "
				"test buffer from client");
			ADD_STR_LOG("name", server->name);
		}
		delete[] test_buff;
		return false;
	}

	size = sendto(server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
		(sockaddr*)&server->client_address,
		server->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "There was an error sending "
				"test buffer to client");
			ADD_STR_LOG("name", server->name);
		}
		delete[] test_buff;
		return false;
	}

	delete[] test_buff;
	return true;
}

bool Send_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	return sendto(server->sockfd, buffer, size, 0,
		(sockaddr*)&server->client_address,
		server->client_address_size) == size;
}

bool Receive_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	return recvfrom(server->sockfd, buffer, size, 0,
		(sockaddr*)&server->client_address,
		(socklen_t*)&server->client_address_size) == size;
}

int Receive_Unsafe_UDP_SERVER(UDP_SERVER *server, char *buffer) {
	return recvfrom(server->sockfd, buffer, ARBITRARILY_LARGE_PACKET, 0,
		(sockaddr*)&server->client_address,
		(socklen_t*)&server->client_address_size);
}

void Delete_UDP_SERVER(UDP_SERVER *server) {
	if (server->sockfd < 0) {
		shutdown(server->sockfd, 2);
		close(server->sockfd);
		server->sockfd = NULL;

		LOG_INFO_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Server connection closed");
			ADD_STR_LOG("name", server->name);
		}
		return;
	}

	LOG_ERROR_CTX((server->ctx)) {
		ADD_STR_LOG("message",
			"Server connection has already been closed");
		ADD_STR_LOG("name", server->name);
	}
}
