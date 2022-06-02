#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#ifndef _WIN64
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "NETWORK.h"
#include "../Utilities/KCONTEXT.h"
#include "UDP_SERVER_MASTER.h"

struct UDP_SERVER {
	KCONTEXT *ctx;
	char name[MAX_NAME_SIZE];
	char buffer[MAX_UDP_PACKET_SIZE + 1]; //plus 1 for the type

	UDP_SERVER_MASTER *udp_master;
	timeval timeout;
	volatile int id;
};

bool Initialize_UDP_SERVER(UDP_SERVER*, KCONTEXT*, UDP_SERVER_MASTER*, int);
void Set_Name_UDP_SERVER(UDP_SERVER*, char*);
bool Set_Recv_Timeout_UDP_SERVER(UDP_SERVER*, int, int);
bool Set_High_Priority_UDP_SERVER(UDP_SERVER*);
bool Accept_UDP_SERVER(UDP_SERVER*);
bool Send_UDP_SERVER(UDP_SERVER*, char*, int);
bool Receive_UDP_SERVER(UDP_SERVER*, char*, int);
int Receive_Unsafe_UDP_SERVER(UDP_SERVER*, char*);
void Delete_UDP_SERVER(UDP_SERVER*);

#endif
