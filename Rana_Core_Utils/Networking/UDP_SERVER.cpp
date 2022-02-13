#include "UDP_SERVER.h"

bool Initialize_UDP_SERVER(UDP_SERVER *server, CONTEXT *ctx, UDP_SERVER_MASTER *udp_master) {
	uint32_t input_var;

	server->ctx = ctx;
	server->udp_master = udp_master;
	Set_Name_UDP_SERVER(server, "unknown");

	server->sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
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

	input_var = 1;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEPORT, &input_var,
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

	if (!Set_Recv_Timeout_UDP_SERVER(server, DEFAULT_RECV_TIMEOUT, 0)) {
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

bool Accept_UDP_SERVER(UDP_SERVER *server) {
	int32_t size;
	uint8_t* test_buff;

	sockaddr_in	server_address;
	sockaddr_in	client_address;

	int32_t server_address_size = sizeof(server_address);
	int32_t client_address_size = sizeof(client_address);

	memset(&server_address, 0, server_address_size);
	memset(&client_address, 0, client_address_size);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(server->udp_master->port);

	if (!Set_Recv_Timeout_UDP_SERVER(server, DEFAULT_ACCEPT_TIMEOUT, 0)) {
		return false;
	}

	if (bind(server->sockfd, (const struct sockaddr*) &server_address,
		server_address_size) < 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message",
				"Failed to accept: Failed to bind UDP server socket");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	test_buff = new uint8_t[TEST_BUFF_SIZE];

	size = recvfrom(server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, MSG_WAITALL,
		(sockaddr*)&client_address, (socklen_t*)&client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: There was an error receiving "
				"test buffer from client");
			ADD_STR_LOG("name", server->name);
		}
		delete[] test_buff;
		return false;
	}

	size = sendto(server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, MSG_WAITALL,
		(sockaddr*)&client_address, client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: There was an error sending "
				"test buffer to client");
			ADD_STR_LOG("name", server->name);
		}
		delete[] test_buff;
		return false;
	}

	if (connect(server->sockfd, (sockaddr*)&client_address,
		client_address_size) < 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: connect() failed");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	delete[] test_buff;

	if (!Set_Recv_Timeout_UDP_SERVER(server, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	return true;
}

bool Send_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	return send(server->sockfd, buffer, size, MSG_WAITALL) == size;
}

bool Receive_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	return recv(server->sockfd, buffer, size, MSG_WAITALL) == size;
}

int Receive_Unsafe_UDP_SERVER(UDP_SERVER *server, char *buffer) {
	return recv(server->sockfd, buffer, ARBITRARILY_LARGE_PACKET, 0);
}

void Delete_UDP_SERVER(UDP_SERVER *server) {
	if (server->sockfd < 0) {
		shutdown(server->sockfd, 2);
		close(server->sockfd);
		server->sockfd = NULL;

		return;
	}

	LOG_WARN_CTX((server->ctx)) {
		ADD_STR_LOG("message",
			"Server connection has already been closed");
		ADD_STR_LOG("name", server->name);
	}
}
