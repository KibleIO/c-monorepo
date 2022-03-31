#include "UDP_SERVER.h"

bool Initialize_UDP_SERVER(UDP_SERVER *server, CONTEXT *ctx,
	UDP_SERVER_MASTER *udp_master, int id) {

	server->ctx = ctx;
	server->udp_master = udp_master;
	server->id = id;
	server->buffer[0] = id;
	Set_Name_UDP_SERVER(server, "unknown");

	Set_Recv_Timeout_UDP_SERVER(server, DEFAULT_RECV_TIMEOUT, 0);

	return true;
}

void Set_Name_UDP_SERVER(UDP_SERVER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_UDP_SERVER(UDP_SERVER *server, int sec, int usec) {
	server->timeout.tv_sec = sec;
	server->timeout.tv_usec = usec;
	return true;
}

bool Set_High_Priority_UDP_SERVER(UDP_SERVER *server) {
	return Set_High_Priority_UDP_SERVER_MASTER(server->udp_master);
}

bool Accept_UDP_SERVER(UDP_SERVER *server) {
	int32_t size;
	uint8_t test_buff[TEST_BUFF_SIZE];

	if (server->udp_master->accepted) {
		//already accepted brother
		return true;
	}

	if (!Set_Recv_Timeout_UDP_SERVER_MASTER(server->udp_master,
		DEFAULT_ACCEPT_TIMEOUT, 0)) {

		return false;
	}

	size = recvfrom(server->udp_master->sockfd, (char*)test_buff,
		TEST_BUFF_SIZE, MSG_WAITALL,
		(sockaddr*) &server->udp_master->client_address,
		(socklen_t*) &server->udp_master->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: There was an "
				"error receiving test buffer from client");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	size = sendto(server->udp_master->sockfd, (char*)test_buff,
		TEST_BUFF_SIZE, MSG_WAITALL,
		(sockaddr*) &server->udp_master->client_address,
		server->udp_master->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: There was an "
				"error sending test buffer to client");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if (!Set_Recv_Timeout_UDP_SERVER_MASTER(server->udp_master,
		DEFAULT_RECV_TIMEOUT, 0)) {

		return false;
	}

	if (server->udp_master->recv_thread != NULL) {
		//whoa... what a weird scenario
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: Weird, "
				"unresolvable error occured");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	server->udp_master->running = true;
	server->udp_master->recv_thread = new thread(
		Recv_Loop_UDP_SERVER_MASTER, server->udp_master);

	server->udp_master->accepted = true;

	return true;
}

bool Send_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	if (server->id < 0) return false;

	memcpy(server->buffer + 1, buffer, size);

	return sendto(server->udp_master->sockfd, (char*) server->buffer,
		size + 1, MSG_WAITALL,
		(sockaddr*)&server->udp_master->client_address,
		server->udp_master->client_address_size) == size;
}

bool Receive_UDP_SERVER(UDP_SERVER *server, char *buffer, int size) {
	UDP_PACKET *temp_buff;
	int attempts = RECV_ATTEMPTS;

	while (server->id >= 0 && --attempts >= 0 &&
		server->udp_master->recv_queues[server->id]->empty()) {
		//OKAY THERE IS A MASSIVE ERROR HERE
		//please eventually also take into account usec pls
		Sleep_Milli((server->timeout.tv_sec * 1000) / RECV_ATTEMPTS);
	}

	if (attempts < 0 || server->id < 0) {
		return false;
	}

	temp_buff = server->udp_master->recv_queues[server->id]->pop();

	memcpy(buffer, temp_buff->buffer + 1, temp_buff->size - 1);

	server->udp_master->recv_pool->push(temp_buff);

	return (temp_buff->size - 1) == size;
}

int Receive_Unsafe_UDP_SERVER(UDP_SERVER *server, char *buffer) {
	UDP_PACKET *temp_buff;
	int attempts = RECV_ATTEMPTS;

	while (server->id >= 0 && --attempts >= 0 &&
		server->udp_master->recv_queues[server->id]->empty()) {

		Sleep_Milli((server->timeout.tv_sec * 1000) / RECV_ATTEMPTS);
	}

	if (attempts < 0 || server->id < 0) {
		return 0;
	}

	temp_buff = server->udp_master->recv_queues[server->id]->pop();

	memcpy(buffer, temp_buff->buffer + 1, temp_buff->size - 1);

	server->udp_master->recv_pool->push(temp_buff);

	return temp_buff->size - 1;
}

void Delete_UDP_SERVER(UDP_SERVER *server) {
        int temp_id = server->id;

        server->id = -1;

	if (temp_id >= 0) {
                if (server->udp_master->recv_thread != NULL) {
			server->udp_master->running = false;
			server->udp_master->recv_thread->join();
			delete server->udp_master->recv_thread;
			server->udp_master->recv_thread = NULL;
		}

		while (!server->udp_master->recv_queues[temp_id]->empty()) {
                        server->udp_master->recv_pool->push(
				server->udp_master->recv_queues[temp_id]->
				pop());
		}

		server->udp_master->accepted = false;
	}

	LOG_WARN_CTX((server->ctx)) {
		ADD_STR_LOG("message",
			"Client connection has already been closed");
		ADD_STR_LOG("name", server->name);
	}
}
