#include "UDP_SERVER_MASTER.h"

bool Initialize_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, KCONTEXT *ctx,
	int port) {

	uint32_t input_var;
	sockaddr_in	server_address;

	int32_t server_address_size = sizeof(server_address);
	server->client_address_size = sizeof(server->client_address);

	memset(&server_address, 0, server_address_size);
	memset(&server->client_address, 0, server->client_address_size);

	server->ctx = ctx;
	server->accepted = false;
	server->recv_thread = NULL;
	server->running = false;
	Set_Name_UDP_SERVER_MASTER(server, "master");

        #ifdef linux

	server->sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (server->sockfd < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Server socket failed to open"},
			{"name", server->name},
		});
		return false;
	}

        #else

        server->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server->sockfd < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Server socket failed to open"},
			{"name", server->name},
		});
		return false;
	}

        #endif

	input_var = 1;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &input_var,
		sizeof(uint32_t)) == -1) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#ifndef _WIN64

	input_var = 1;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEPORT, &input_var,
		sizeof(uint32_t)) == -1) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#endif

	input_var = 1000000;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_SNDBUF, (const char*) &input_var,
		sizeof(uint32_t)) == -1) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: sndbuf"},
			{"name", server->name},
		});
		return false;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);

	if (::bind(server->sockfd, (const struct sockaddr*) &server_address,
		server_address_size) < 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: Failed to bind UDP server"},
			{"name", server->name},
		});
		return false;
	}

	server->recv_pool = new Queue<UDP_PACKET*>;

	for (int i = 0; i < MAX_UDP_CONNECTIONS; i++) {
		server->recv_queues[i] = new Queue<UDP_PACKET*>;
	}

	for (int i = 0; i < POOL_QUEUE_SIZE; i++) {
		server->recv_pool->push(new UDP_PACKET);
	}

	return true;
}

void Set_Name_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, char *name) {
	strcpy(server->name, name);
}

void Recv_Loop_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server) {
	sockaddr_in	client_address;
	int32_t client_address_size = sizeof(client_address);
	UDP_PACKET *temp_buff = NULL;

	while (server->running) {
		if (temp_buff == NULL) {
			if (server->recv_pool->empty()) {
				//what? should never happen
				continue;
			}
			temp_buff = server->recv_pool->pop();
		}

		temp_buff->size = recvfrom(server->sockfd, temp_buff->buffer,
			ARBITRARILY_LARGE_PACKET, 0,
			(sockaddr*)&client_address,
			(socklen_t*)&client_address_size);

		if (temp_buff->size > 1 && temp_buff->buffer[0] >= 0 && temp_buff->buffer[0] < MAX_UDP_CONNECTIONS) {
			server->recv_queues[temp_buff->buffer[0]]->push(
				temp_buff);

			temp_buff = NULL;
		}
	}
}

bool Set_Recv_Timeout_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server, int sec, int usec) {
	timeval o_to = { sec, usec };
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &o_to,
		sizeof(timeval)) == -1) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: timeout"},
			{"name", server->name},
		});
		return false;
	}
	return true;
}

bool Set_High_Priority_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server) {
        #ifdef linux

	int32_t o;
	o = 6;
	if (setsockopt(server->sockfd, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: priority"},
			{"name", server->name},
		});
		return false;
	}

        #endif

	return true;
}

void Delete_UDP_SERVER_MASTER(UDP_SERVER_MASTER *server) {
	//this logic causes a segfault... buyer beware
	if (server->sockfd > 0) {
		shutdown(server->sockfd, 2);
		close(server->sockfd);

		server->sockfd = NULL;

		for (int i = 0; i < MAX_UDP_CONNECTIONS; i++) {
			while (!server->recv_queues[i]->empty()) {
				delete server->recv_queues[i]->pop();
			}
		}

		while (!server->recv_pool->empty()) {
			delete server->recv_pool->pop();
		}

		delete server->recv_pool;

		for (int i = 0; i < POOL_QUEUE_SIZE; i++) {
			delete server->recv_queues[i];
		}

		if (server->recv_thread != NULL) {
			server->running = false;
			server->recv_thread->join();
			delete server->recv_thread;
			server->recv_thread = NULL;
		}
		server->accepted = false;

		return;
	}
	LOGGER_WARN(server->ctx, {
		{"message", "Server connection has already been closed"},
		{"name", server->name},
	});
}
