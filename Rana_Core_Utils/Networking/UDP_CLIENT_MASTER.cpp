#include "UDP_CLIENT_MASTER.h"

bool Initialize_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client, CONTEXT *ctx,
	int port, char *ip) {

	uint32_t input_var;

	client->server_address_size = sizeof(client->server_address);
	memset(&client->server_address, 0, client->server_address_size);

	client->ctx = ctx;
	client->running = false;
	client->connected = false;
	client->recv_thread = NULL;
	Set_Name_UDP_CLIENT_MASTER(client, "unknown");

        #ifdef linux

	client->sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (client->sockfd < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client socket failed to open");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #else

        client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client->sockfd < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client socket failed to open");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #endif

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

	client->server_address.sin_family = AF_INET;
	client->server_address.sin_addr.s_addr = INADDR_ANY;
	client->server_address.sin_port = htons(port);

	if (inet_pton(AF_INET, ip,
		&(client->server_address.sin_addr.s_addr)) < 1) {

		if (!getaddrinfo_k(&client->server_address.sin_addr.s_addr, ip,
			2)) {

			LOG_ERROR_CTX((client->ctx)) {
				ADD_STR_LOG("message", "Failed to connect: "
					"getaddrinfo_k() failed");
				ADD_STR_LOG("ip", ip);
				ADD_STR_LOG("name", client->name);
			}
			return false;
		}
	}

	client->recv_pool = new Queue<UDP_PACKET*>;

	for (int i = 0; i < MAX_UDP_CONNECTIONS; i++) {
		client->recv_queues[i] = new Queue<UDP_PACKET*>;
	}

	for (int i = 0; i < POOL_QUEUE_SIZE; i++) {
		client->recv_pool->push(new UDP_PACKET);
	}

	return true;
}

void Set_Name_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client, char *name) {
	strcpy(client->name, name);
}

void Recv_Loop_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client) {
	sockaddr_in	server_address;
	int32_t server_address_size = sizeof(server_address);
	UDP_PACKET *temp_buff = NULL;

	while (client->running) {
		if (temp_buff == NULL) {
			if (client->recv_pool->empty()) {
				//what? should never happen
				continue;
			}
			temp_buff = client->recv_pool->pop();
		}

		temp_buff->size = recvfrom(client->sockfd, temp_buff->buffer,
			ARBITRARILY_LARGE_PACKET, 0,
			(sockaddr*)&server_address,
			(socklen_t*)&server_address_size);

		if (temp_buff->size > 0) {
			client->recv_queues[temp_buff->buffer[0]]->push(
				temp_buff);

			temp_buff = NULL;
		}
	}
}

bool Set_Recv_Timeout_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client, int s,
	int u) {

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

bool Set_High_Priority_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client) {
        #ifdef linux

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

        #endif

	return true;
}

void Delete_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER *client) {
	if (client->sockfd > 0) {
		shutdown(client->sockfd, 2);
		close(client->sockfd);
		client->sockfd = NULL;

		for (int i = 0; i < MAX_UDP_CONNECTIONS; i++) {
			while (!client->recv_queues[i]->empty()) {
				delete client->recv_queues[i]->pop();
			}
		}

		while (!client->recv_pool->empty()) {
			delete client->recv_pool->pop();
		}

		delete client->recv_pool;

		for (int i = 0; i < POOL_QUEUE_SIZE; i++) {
			delete client->recv_queues[i];
		}

		if (client->recv_thread != NULL) {
			client->running = false;
			client->recv_thread->join();
			delete client->recv_thread;
			client->recv_thread = NULL;
		}
		client->connected = false;

		return;
	}

	LOG_WARN_CTX((client->ctx)) {
		ADD_STR_LOG("message",
			"Client connection has already been closed");
		ADD_STR_LOG("name", client->name);
	}
}
