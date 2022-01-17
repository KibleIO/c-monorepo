#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "NETWORK.h"
#include "../Utilities/CONTEXT.h"

struct UDP_SERVER {
	CONTEXT *ctx;
	char name[MAX_NAME_SIZE];

	int32_t		sockfd;
	sockaddr_in	server_address;
	sockaddr_in	client_address;
	uint32_t	server_address_size;
	uint32_t	client_address_size;
};

bool Initialize_UDP_SERVER(UDP_SERVER*, CONTEXT*);
void Set_Name_UDP_SERVER(UDP_SERVER*, char*);
bool Set_Recv_Timeout_UDP_SERVER(UDP_SERVER*, int, int);
bool Set_High_Priority_UDP_SERVER(UDP_SERVER*);
bool Accept_UDP_SERVER(UDP_SERVER*, int);
bool Send_UDP_SERVER(UDP_SERVER*, char*, int);
bool Receive_UDP_SERVER(UDP_SERVER*, char*, int);
int Receive_Unsafe_UDP_SERVER(UDP_SERVER*, char*);
void Delete_UDP_SERVER(UDP_SERVER*);

#endif
