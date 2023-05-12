#include "UDP_CLIENT.h"

bool Initialize_UDP_CLIENT(UDP_CLIENT *client, KCONTEXT *ctx,
	UDP_CLIENT_MASTER *udp_master, int id) {

	client->ctx = ctx;
	client->udp_master = udp_master;
	client->id = id;
	client->buffer[0] = id;
	Set_Name_UDP_CLIENT(client, "unknown");

	Set_Recv_Timeout_UDP_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0);

	return true;
}

void Set_Name_UDP_CLIENT(UDP_CLIENT *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_UDP_CLIENT(UDP_CLIENT *client, int sec, int usec) {
	client->timeout.tv_sec = sec;
	client->timeout.tv_usec = usec;
	return true;
}

bool Set_High_Priority_UDP_CLIENT(UDP_CLIENT *client) {
	return Set_High_Priority_UDP_CLIENT_MASTER(client->udp_master);
}

bool Connect_UDP_CLIENT(UDP_CLIENT *client) {
	int32_t size;
	uint8_t test_buff[TEST_BUFF_SIZE];
	sockaddr_in server_address;
	uint32_t server_address_size;

	if (client->udp_master->connected) {
		//already accepted brother
		return true;
	}

	if (!Set_Recv_Timeout_UDP_CLIENT_MASTER(client->udp_master, 0,
		DEFAULT_CONNECT_TIMEOUT)) {
		return false;
	}

	size = sendto(client->udp_master->sockfd, (char*)test_buff,
		TEST_BUFF_SIZE, MSG_WAITALL,
		(sockaddr*) &client->udp_master->server_address,
		client->udp_master->server_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_WARN_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error sending test "
				"buff");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	size = recvfrom(client->udp_master->sockfd, (char*)test_buff,
		TEST_BUFF_SIZE, MSG_WAITALL, (sockaddr*) &server_address,
		(socklen_t*) &server_address_size);

	if (size != TEST_BUFF_SIZE) {
		LOG_WARN_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error receiving test "
				"buff... trying again");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	if (!Set_Recv_Timeout_UDP_CLIENT_MASTER(client->udp_master,
		DEFAULT_RECV_TIMEOUT, 0)) {

		return false;
	}

	if (client->udp_master->recv_thread != NULL) {
		//whoa... what a weird scenario
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to accept: Weird, "
				"unresolvable error occured");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	client->udp_master->running = true;
	client->udp_master->recv_thread = new thread(
		Recv_Loop_UDP_CLIENT_MASTER, client->udp_master);

	client->udp_master->connected = true;

	return true;
}

bool Send_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int size) {
	if (client->id < 0) return false;

	memcpy(client->buffer + 1, buffer, size);

	return sendto(client->udp_master->sockfd, (char*) client->buffer,
		size + 1, MSG_WAITALL,
		(sockaddr*) &client->udp_master->server_address,
		client->udp_master->server_address_size) == size;
}

bool Receive_UDP_CLIENT(UDP_CLIENT *client, char *buffer, int size) {
	//FOR THE LOVE OF GOD DO NOT USE THIS FUNCTION
	UDP_PACKET *temp_buff;
	int attempts = RECV_ATTEMPTS;

	if (client->id < 0) return false;

	while (--attempts >= 0 &&
		client->udp_master->recv_queues[client->id]->empty()) {

		Sleep_Milli((client->timeout.tv_sec * 1000) / RECV_ATTEMPTS);
	}

	if (attempts < 0) {
		return false;
	}

	temp_buff = client->udp_master->recv_queues[client->id]->pop();

	memcpy(buffer, temp_buff->buffer + 1, temp_buff->size - 1);

	client->udp_master->recv_pool->push(temp_buff);

	return (temp_buff->size - 1) == size;
}

int Receive_Unsafe_UDP_CLIENT(UDP_CLIENT *client, char *buffer) {
	UDP_PACKET *temp_buff;
	int attempts = RECV_ATTEMPTS;

	if (client->id < 0) return false;

	while (--attempts >= 0 &&
		client->udp_master->recv_queues[client->id]->empty()) {

		Sleep_Milli((client->timeout.tv_sec * 1000) / RECV_ATTEMPTS);
	}

	if (attempts < 0) {
		return 0;
	}

	temp_buff = client->udp_master->recv_queues[client->id]->pop();

	memcpy(buffer, temp_buff->buffer + 1, temp_buff->size - 1);

	client->udp_master->recv_pool->push(temp_buff);

	return temp_buff->size - 1;
}

void Delete_UDP_CLIENT(UDP_CLIENT *client) {
	if (client->id >= 0) {
		while (!client->udp_master->recv_queues[client->id]->empty()) {
			client->udp_master->recv_pool->push(
				client->udp_master->recv_queues[client->id]->
				pop());
		}

		client->id = -1;

		if (client->udp_master->recv_thread != NULL) {
			client->udp_master->running = false;
			client->udp_master->recv_thread->join();
			delete client->udp_master->recv_thread;
			client->udp_master->recv_thread = NULL;
		}
		client->udp_master->connected = false;
	}

	LOG_WARN_CTX((client->ctx)) {
		ADD_STR_LOG("message",
			"Client connection has already been closed");
		ADD_STR_LOG("name", client->name);
	}
}
