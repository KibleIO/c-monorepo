#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include "NETWORK.h"
#include "../Utilities/CONTEXT.h"
#include "DNS_WRAPPER.h"

#define CONNECT_ATTEMPTS 50
//#define BUFLEN 1500

struct UDP_CLIENT {
	int32_t sockfd;
	char name[MAX_NAME_SIZE];
	CONTEXT *ctx;
};

bool Initialize_UDP_CLIENT(UDP_CLIENT*, CONTEXT*);
void Set_Name_UDP_CLIENT(UDP_CLIENT*, char*);
bool Set_Recv_Timeout_UDP_CLIENT(UDP_CLIENT*, int, int);
bool Set_High_Priority_UDP_CLIENT(UDP_CLIENT*);
bool Connect_UDP_CLIENT(UDP_CLIENT*, int, char*);
bool Send_UDP_CLIENT(UDP_CLIENT*, char*, int);
bool Receive_UDP_CLIENT(UDP_CLIENT*, char*, int);
int Receive_Unsafe_UDP_CLIENT(UDP_CLIENT*, char*);
void Delete_UDP_CLIENT(UDP_CLIENT*);

#endif
