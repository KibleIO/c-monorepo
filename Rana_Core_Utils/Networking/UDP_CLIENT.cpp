#include "UDP_CLIENT.h"

bool Initialize_UDP_CLIENT(UDP_CLIENT* client, CONTEXT *ctx) {
	uint32_t input_var;

	client->ctx = ctx;
	Set_Name_UDP_CLIENT(client, "unknown");

	client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client->sockfd < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client socket failed to open");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	input_var = 1000000;
	if (setsockopt(client->sockfd, SOL_SOCKET, SO_RCVBUF, &input_var,
				   sizeof(uint32_t)) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: recvbuf");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	input_var = 1;
	if (setsockopt(client->sockfd, SOL_SOCKET, SO_REUSEADDR, &input_var,
		sizeof input_var) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	if (setsockopt(client->sockfd, SOL_SOCKET, SO_REUSEPORT, &input_var,
		sizeof input_var) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	if (!Set_Recv_Timeout_UDP_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	return true;
}

void Set_Name_UDP_CLIENT(UDP_CLIENT *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_UDP_CLIENT(UDP_CLIENT *client, int s, int u) {
	timeval tv;
	tv.tv_sec = s;
	tv.tv_usec = u;
	if (setsockopt(client->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv,
		sizeof(tv)) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: rcvtimeo");
			ADD_STR_LOG("name", client->name);
		}

		return false;
	}
	return true;
}

bool Set_High_Priority_UDP_CLIENT(UDP_CLIENT *client) {
	int32_t o;
	o = 6;
	if (setsockopt(client->sockfd, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: priority");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}
	return true;
}

bool Connect_UDP_CLIENT(UDP_CLIENT *client, int port, char *ip) {
	int32_t size;
	int connect_attempts = CONNECT_ATTEMPTS;
	uint8_t* test_buff;

	test_buff = new uint8_t[TEST_BUFF_SIZE];

	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);

	if (inet_pton(AF_INET, ip, &(destination.sin_addr.s_addr)) < 1) {
		if (!getaddrinfo_k(&destination.sin_addr.s_addr, ip, 2)) {
			LOG_ERROR_CTX((client->ctx)) {
				ADD_STR_LOG("message", "getaddrinfo_k() failed");
				ADD_STR_LOG("ip", ip);
				ADD_STR_LOG("name", client->name);
			}
			return false;
		}
	}

	if (!Set_Recv_Timeout_UDP_CLIENT(client, 0, DEFAULT_CONNECT_TIMEOUT)) {
		return false;
	}

	if (connect(client->sockfd, (sockaddr*)&destination,
		sizeof(destination)) < 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "connect() failed");
			ADD_STR_LOG("ip", ip);
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	size = send(client->sockfd, (char*)test_buff, TEST_BUFF_SIZE,
		MSG_WAITALL);

	if (size != TEST_BUFF_SIZE) {
		LOG_WARN_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Error sending test "
				"buff... trying again");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	size = recv(client->sockfd, (char*)test_buff, TEST_BUFF_SIZE,
		MSG_WAITALL);

	if (size != TEST_BUFF_SIZE) {
		LOG_WARN_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Error receiving test "
				"buff... trying again");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	delete[] test_buff;

	if (!Set_Recv_Timeout_UDP_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	LOG_INFO_CTX((client->ctx)) {
		ADD_STR_LOG("message", "Connection successful");
		ADD_STR_LOG("name", client->name);
	}
	return true;
}

bool Send_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int size) {
	return send(client->sockfd, buffer, size, MSG_WAITALL) == size;
}

bool Receive_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int size) {
	//FOR THE LOVE OF GOD DO NOT USE THIS FUNCTION
	return recv(client->sockfd, buffer, size, MSG_WAITALL) == size;
}

int Receive_Unsafe_UDP_CLIENT(UDP_CLIENT *client, char *buffer) {
	return recv(client->sockfd, buffer, ARBITRARILY_LARGE_PACKET, 0);
}

void Delete_UDP_CLIENT(UDP_CLIENT *client) {
	if (client->sockfd > 0) {
		shutdown(client->sockfd, 2);
		close(client->sockfd);
		client->sockfd = NULL;

		LOG_INFO_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client connection closed");
			ADD_STR_LOG("name", client->name);
		}
		return;
	}

	LOG_ERROR_CTX((client->ctx)) {
		ADD_STR_LOG("message",
			"Client connection has already been closed");
		ADD_STR_LOG("name", client->name);
	}
}
