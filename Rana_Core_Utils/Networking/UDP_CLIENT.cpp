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
	client->server_address_size = sizeof(client->server_address);
	memset(&client->server_address, 0, client->server_address_size);

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

	if (!Set_Recv_Timeout_UDP_CLIENT(client, 5, 0)) {
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

	client->server_address.sin_family = AF_INET;
	client->server_address.sin_port = htons(port);

	if (inet_pton(AF_INET, ip,
		&(client->server_address.sin_addr.s_addr)) < 1) {

		struct addrinfo hints, *res;
		struct addrinfo* itr;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_CANONNAME;

		if (getaddrinfo(ip, NULL, &hints, &res) != 0) {
			LOG_ERROR_CTX((client->ctx)) {
				ADD_STR_LOG("message", "getaddrinfo() failed");
				ADD_STR_LOG("ip", ip);
				ADD_STR_LOG("name", client->name);
			}

			return false;
		}

		itr = res;

		while (itr) {
			if (itr->ai_family == AF_INET) {
				client->server_address.sin_addr.s_addr =
					((struct sockaddr_in*)itr->ai_addr)->
					sin_addr.s_addr;

				freeaddrinfo(res);

				goto success;
			}
			itr = itr->ai_next;
		}

		freeaddrinfo(res);

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "failed to find AF_INET");
			ADD_STR_LOG("ip", ip);
			ADD_STR_LOG("name", client->name);
		}

		return false;
	}
	success:

	if (!Set_Recv_Timeout_UDP_CLIENT(client, 0, 100000)) {
		return false;
	}

	while (connect_attempts-- > 0) {
		errno = 0;
		size = sendto(client->sockfd, (char*)test_buff,
			TEST_BUFF_SIZE, 0, (sockaddr*)&client->server_address,
			client->server_address_size);

		if (size != TEST_BUFF_SIZE) {
			LOG_WARN_CTX((client->ctx)) {
				ADD_STR_LOG("message", "Error sending test "
					"buff... trying again");
				ADD_STR_LOG("name", client->name);
			}
			continue;
		}

		errno = 0;
		size = recvfrom(client->sockfd, (char*)test_buff,
			TEST_BUFF_SIZE, 0, (sockaddr*)&client->server_address,
			&client->server_address_size);

		if (size != TEST_BUFF_SIZE) {
			LOG_WARN_CTX((client->ctx)) {
				ADD_STR_LOG("message", "Error receiving test "
					"buff... trying again");
				ADD_STR_LOG("name", client->name);
			}
			continue;
		}
		break;
	}

	delete[] test_buff;

	if (!Set_Recv_Timeout_UDP_CLIENT(client, 5, 0)) {
		return false;
	}

	if (connect_attempts > 0) {
		LOG_INFO_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Connection successful");
			ADD_STR_LOG("name", client->name);
		}
		return true;
	} else {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Connection timedout");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}
}

bool Send_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int32_t size) {
	return sendto(client->sockfd, buffer, size, 0,
		(sockaddr*)&client->server_address,
		client->server_address_size) == size;
}

bool Receive_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int32_t size) {
	//FOR THE LOVE OF GOD DO NOT USE THIS FUNCTION
	return recvfrom(client->sockfd, buffer, size, 0,
		(sockaddr*)&client->server_address,
		(socklen_t*)&client->server_address_size) == size;
}

int Receive_Unsafe_UDP_CLIENT(UDP_CLIENT *client, char *buffer) {
	return recvfrom(client->sockfd, buffer, ARBITRARILY_LARGE_PACKET, 0,
		(sockaddr*)&client->server_address,
		(socklen_t*)&client->server_address_size);
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
