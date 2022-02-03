#ifndef UDP_SERVER_MASTER_H
#define UDP_SERVER_MASTER_H

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "NETWORK.h"
#include "../Utilities/CONTEXT.h"

struct UDP_SERVER_MASTER {
	CONTEXT *ctx;
	char name[MAX_NAME_SIZE];
	int port;
};

bool Initialize_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, CONTEXT*, int);
void Set_Name_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, char*);
bool Set_Recv_Timeout_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, int, int);
bool Set_High_Priority_UDP_SERVER_MASTER(UDP_SERVER_MASTER*);
void Delete_UDP_SERVER_MASTER(UDP_SERVER_MASTER*);

#endif
