#include "UDP_CLIENT.h"

#ifdef _W64
uint8_t Initialize_UDP_CLIENT(UDP_CLIENT* udp_client) {
	if (WSAStartup(MAKEWORD(2, 2), &udp_client->wsa) != 0) {
		log_err("could not initialize wsa");
		return false;
	}

	udp_client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_client->sockfd == SOCKET_ERROR) {
		log_err("could not set up socket");
		return false;
	}

	udp_client->server_address_size = sizeof(udp_client->server_address);
	memset(&udp_client->server_address, 0, udp_client->server_address_size);

	// Recieve buffer option
	int ret;
	uint32_t o_buf = 1000000;
	ret = setsockopt(udp_client->sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&o_buf,
					 sizeof(uint32_t));
	if (ret < 0) {
		log_err("Failed to set SO_RCVBUF on UDP client socket");
		return false;
	}

	// Timeout option
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	ret = setsockopt(udp_client->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv,
					 sizeof(tv));
	if (ret < 0) {
		log_err("Failed to set SO_RCVTIMEO on UDP client socket");
		return false;
	}

	// Access
	// char o_acc = '1';
	// ret = setsockopt(
	// udp_client->sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&o_acc,
	// sizeof(o_acc)); if (ret < 0) { 	log_err("Failed to set SO_BROADCAST on UDP
	//client socket"); 	return false;
	//}

	return true;
}

uint8_t Connect_UDP_CLIENT(UDP_CLIENT* udp_client, string ip_address,
						   uint32_t port) {
	Sleep_Milli(2000);
	int32_t size;
	uint8_t* test_buff;
	test_buff = new uint8_t[TEST_BUFF_SIZE];

	log_dbg("connecting to " + ip_address + ":" + to_string(port));

	udp_client->server_address.sin_family = AF_INET;
	udp_client->server_address.sin_port = htons(port);

	// memset(
	//(char*)&udp_client->server_address, 0,
	//sizeof(udp_client->server_address)); udp_client->server_address.sin_family
	// = AF_INET; udp_client->server_address.sin_port			= htons(port);
	// udp_client->server_address.sin_addr.s_addr	=
	// inet_addr(ip_address.c_str());

	if (inet_pton(AF_INET, ip_address.c_str(),
				  &(udp_client->server_address.sin_addr.s_addr)) < 1) {
		struct addrinfo hints, *res;
		struct addrinfo* itr;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_CANONNAME;

		if (getaddrinfo(ip_address.c_str(), NULL, &hints, &res) != 0) {
			return false;
		}

		itr = res;

		while (itr) {
			if (itr->ai_family == AF_INET) {
				log_tmp("got address");
				udp_client->server_address.sin_addr.s_addr =
					((struct sockaddr_in*)itr->ai_addr)->sin_addr.s_addr;

				freeaddrinfo(res);

				goto success;
			}
			itr = itr->ai_next;
		}

		freeaddrinfo(res);
		return false;
	}
success:

	// Bind handshake
	size = sendto(udp_client->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
				  (sockaddr*)&udp_client->server_address,
				  (socklen_t)udp_client->server_address_size);
	if (size != TEST_BUFF_SIZE) {
		log_err("There was an error sending test buffer to server");
		delete[] test_buff;
		return false;
	}
	size = recvfrom(udp_client->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
					(sockaddr*)&udp_client->server_address,
					(socklen_t*)&udp_client->server_address_size);
	if (size != TEST_BUFF_SIZE) {
		log_err("There was an error receiving test buffer from server");
		delete[] test_buff;
		return false;
	}
	delete[] test_buff;

	return true;
}

uint8_t Send_UDP_CLIENT(UDP_CLIENT* udp_client, uint8_t* buffer, int32_t size) {
	return size == sendto(udp_client->sockfd, (char*)buffer, size, 0,
						  (sockaddr*)&udp_client->server_address,
						  udp_client->server_address_size);
}

int32_t Receive_UDP_CLIENT(UDP_CLIENT* udp_client, uint8_t* buffer,
						   int32_t size) {
	return recvfrom(udp_client->sockfd, (char*)buffer, size, 0,
					(sockaddr*)&udp_client->server_address,
					(socklen_t*)&udp_client->server_address_size);
}

uint8_t Receive_Peek_UDP_CLIENT(UDP_CLIENT* udp_client, int32_t& size) {
	u_long bytes_available;
	if (ioctlsocket(udp_client->sockfd, FIONREAD, &bytes_available) != 0) {
		log_err("failed to read input buffer size with error code " +
				to_string(WSAGetLastError()));
		return false;
	}

	size = bytes_available;

	return true;
}

void Delete_UDP_CLIENT(UDP_CLIENT* udp_client) {
	if (udp_client->sockfd < 0) {
		WSACleanup();
	}
}
#endif
#ifdef __linux__
uint8_t Initialize_UDP_CLIENT(UDP_CLIENT* udp_client) {
	*udp_client = {};
	uint32_t input_var;

	udp_client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	udp_client->server_address_size = sizeof(udp_client->server_address);

	if (udp_client->sockfd < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to initialize UDP client socket"},
			JSON_TYPE_END}));
		return false;
	}

	memset(&udp_client->server_address, 0, udp_client->server_address_size);

	input_var = 1000000;
	if (setsockopt(udp_client->sockfd, SOL_SOCKET, SO_RCVBUF, &input_var,
				   sizeof(uint32_t)) == -1) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_RCVBUF on UDP client socket"},
			JSON_TYPE_END}));
		return false;
	}

	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	if (setsockopt(udp_client->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv,
				   sizeof(tv)) < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_RCVTIMEO on UDP client socket"},
			JSON_TYPE_END}));
		return false;
	}

	return true;
}

void Set_Timeout_UDP_CLIENT(UDP_CLIENT* udp_client, int s, int u) {
	timeval o_to = { s, u };
	if (setsockopt(udp_client->sockfd, SOL_SOCKET, SO_RCVTIMEO, &o_to,
		sizeof(timeval)) == -1) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to set SO_RCVTIMEO on UDP server socket"},
			JSON_TYPE_END}));
	}
}

uint8_t Connect_UDP_CLIENT(UDP_CLIENT* udp_client, string ip_address,
						   uint32_t port) {
	int32_t size;
	int connect_attempts = CONNECT_ATTEMPTS;
	uint8_t* test_buff;

	log_dbg(((const JSON_TYPE){{"message", "connecting to"}, JSON_TYPE_END}));

	test_buff = new uint8_t[TEST_BUFF_SIZE];

	udp_client->server_address.sin_family = AF_INET;
	udp_client->server_address.sin_port = htons(port);

	if (inet_pton(AF_INET, ip_address.c_str(),
				  &(udp_client->server_address.sin_addr.s_addr)) < 1) {
		struct addrinfo hints, *res;
		struct addrinfo* itr;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_CANONNAME;

		if (getaddrinfo(ip_address.c_str(), NULL, &hints, &res) != 0) {
			return false;
		}

		itr = res;

		while (itr) {
			if (itr->ai_family == AF_INET) {
				udp_client->server_address.sin_addr.s_addr =
					((struct sockaddr_in*)itr->ai_addr)->sin_addr.s_addr;

				freeaddrinfo(res);

				goto success;
			}
			itr = itr->ai_next;
		}

		freeaddrinfo(res);
		return false;
	}
success:

	Set_Timeout_UDP_CLIENT(udp_client, 0, 100000);

	while (connect_attempts-- > 0) {
		errno = 0;
		size = sendto(udp_client->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
					  (sockaddr*)&udp_client->server_address,
					  udp_client->server_address_size);

		if (size != TEST_BUFF_SIZE) {
			log_err(((const JSON_TYPE){
				{"message", "There was an error sending test buffer to server"},
				JSON_TYPE_END}));
			continue;
		}

		errno = 0;
		size = recvfrom(udp_client->sockfd, (char*)test_buff, TEST_BUFF_SIZE, 0,
						(sockaddr*)&udp_client->server_address,
						&udp_client->server_address_size);

		if (size != TEST_BUFF_SIZE) {
			log_err(((const JSON_TYPE){
				{"message", "There was an error receiving test buffer from server"},
				JSON_TYPE_END}));
			continue;
		}
		break;
	}

	delete[] test_buff;

	Set_Timeout_UDP_CLIENT(udp_client, 5, 0);

	return (connect_attempts > 0);
}

uint8_t Send_UDP_CLIENT(UDP_CLIENT* udp_client, uint8_t* buffer, int32_t size) {
	return size == sendto(udp_client->sockfd, (char*)buffer, size, 0,
						  (sockaddr*)&udp_client->server_address,
						  udp_client->server_address_size);
}

int32_t Receive_UDP_CLIENT(UDP_CLIENT* udp_client, uint8_t* buffer,
						   int32_t size) {
	return recvfrom(udp_client->sockfd, (char*)buffer, size, 0,
					(sockaddr*)&udp_client->server_address,
					(socklen_t*)&udp_client->server_address_size);
}

uint8_t Receive_Peek_UDP_CLIENT(UDP_CLIENT* udp_client, int32_t& size
								/*, uint8_t* buffer, int32_t size*/) {
	return recvfrom(udp_client->sockfd, (char*)&size, sizeof(int32_t), MSG_PEEK,
					(sockaddr*)&udp_client->server_address,
					(socklen_t*)&udp_client->server_address_size);
}

void Delete_UDP_CLIENT(UDP_CLIENT* udp_client) {
	if (udp_client->sockfd > 0) {
		shutdown(udp_client->sockfd, 2);
		close(udp_client->sockfd);
		udp_client->sockfd = NULL;
	}
}
#endif
