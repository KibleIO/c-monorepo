#include "UDP_SERVER.h"

bool Initialize_UDP_SERVER(UDP_SERVER* udp_server, uint32_t port) {
	uint32_t input_var;

	log_dbg(((const JSON_TYPE){
		{"message", "initializing udp server on port"},
		JSON_TYPE_END}));

	udp_server->server_address_size = sizeof(
		udp_server->server_address);
	udp_server->client_address_size = sizeof(
		udp_server->client_address);

	memset(&udp_server->server_address, 0, udp_server->server_address_size);
	memset(&udp_server->client_address, 0, udp_server->client_address_size);

	udp_server->sockfd = socket(
		AF_INET, SOCK_DGRAM, 0);

	udp_server->server_address.sin_family = AF_INET;
	udp_server->server_address.sin_addr.s_addr = INADDR_ANY;
	udp_server->server_address.sin_port = htons(port);

	if (udp_server->sockfd < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to initialize UDP server socket"},
			JSON_TYPE_END}));
		return false;
	}

	if (bind(udp_server->sockfd,
		(const struct sockaddr*) &udp_server->server_address,
		udp_server->server_address_size) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to bind UDP server socket"},
			JSON_TYPE_END}));
		return false;
	}

	input_var = 1;
	if (setsockopt(udp_server->sockfd, SOL_SOCKET, SO_REUSEADDR, &input_var,
		sizeof(uint32_t)) == -1) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_REUSEADDR on UDP server socket"},
			JSON_TYPE_END}));
		return false;
	}

	input_var = 1000000;
	if (setsockopt(udp_server->sockfd, SOL_SOCKET, SO_SNDBUF, &input_var,
		sizeof(uint32_t)) == -1) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_SNDBUF on UDP server socket"},
			JSON_TYPE_END}));
		return false;
	}

	timeval o_to = { 5, 0 };
	if (setsockopt(udp_server->sockfd, SOL_SOCKET, SO_RCVTIMEO, &o_to,
		sizeof(timeval)) == -1) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_RCVTIMEO on UDP server socket"},
			JSON_TYPE_END}));
		return false;
	}

	return true;
}

void Set_Timeout_UDP_SERVER(UDP_SERVER* udp_server, int s, int u) {
	timeval o_to = { s, u };
	if (setsockopt(udp_server->sockfd, SOL_SOCKET, SO_RCVTIMEO, &o_to,
		sizeof(timeval)) == -1) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_RCVTIMEO on UDP server socket"},
			JSON_TYPE_END}));
	}
}

bool Accept_UDP_SERVER(UDP_SERVER* udp_server) {
	int32_t size;
	uint8_t* test_buff;

	test_buff = new uint8_t[TEST_BUFF_SIZE];

	size = recvfrom(
		udp_server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
		(sockaddr*)&udp_server->client_address,
		(socklen_t*)&udp_server->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		log_err(((const JSON_TYPE){
			{"message", "There was an error receiving test buffer from client"},
			JSON_TYPE_END}));
		delete[] test_buff;
		return false;
	}

	size = sendto(
		udp_server->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
		(sockaddr*)&udp_server->client_address, udp_server->client_address_size);

	if (size != TEST_BUFF_SIZE) {
		log_err(((const JSON_TYPE){
			{"message", "There was an error sending test buffer to client"},
			JSON_TYPE_END}));
		delete[] test_buff;
		return false;
	}

	delete[] test_buff;
	return true;
}

bool Send_UDP_SERVER(UDP_SERVER* udp_server, uint8_t* buffer, int32_t size) {
	return size == sendto(
		udp_server->sockfd, (char*)buffer, size, 0,
		(sockaddr*)&udp_server->client_address, udp_server->client_address_size);
}

int32_t Receive_UDP_SERVER(
	UDP_SERVER* udp_server, uint8_t* buffer, int32_t size) {
	return recvfrom(
		udp_server->sockfd, (char*)buffer, size, 0,
		(sockaddr*)&udp_server->client_address,
		(socklen_t*)&udp_server->client_address_size);
}

uint8_t Receive_Peek_UDP_SERVER(UDP_SERVER* udp_server, int32_t& size
/*, uint8_t* buffer, int32_t size*/) {
	return recvfrom(
		udp_server->sockfd, (char*)&size, sizeof(int32_t), MSG_PEEK,
		(sockaddr*)&udp_server->server_address,
		(socklen_t*)&udp_server->server_address_size);
}


void Delete_UDP_SERVER(UDP_SERVER* udp_server) {
	if (udp_server->sockfd < 0) {
		shutdown(udp_server->sockfd, 2);
		close(udp_server->sockfd);
		udp_server->sockfd = NULL;
	}
}
