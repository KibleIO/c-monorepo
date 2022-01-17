#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <Utilities/LOGGING.h>

#define TEST_BUFF_SIZE 4

struct UDP_SERVER {
	int32_t		sockfd;
	sockaddr_in	server_address;
	sockaddr_in	client_address;
	uint32_t	server_address_size;
	uint32_t	client_address_size;
};

bool Initialize_UDP_SERVER(UDP_SERVER*, uint32_t);
void Set_Timeout_UDP_SERVER(UDP_SERVER* udp_server, int s, int u = 0);
bool Accept_UDP_SERVER(UDP_SERVER*);
bool Send_UDP_SERVER(UDP_SERVER*, uint8_t*, int32_t);
int32_t Receive_UDP_SERVER(UDP_SERVER*, uint8_t*, int32_t);
uint8_t Receive_Peek_UDP_SERVER(UDP_SERVER*, int32_t&);
void Delete_UDP_SERVER(UDP_SERVER*);

#endif
