#ifndef UDP_SERVER_MASTER_H
#define UDP_SERVER_MASTER_H

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "NETWORK.h"
#include "../Utilities/CONTEXT.h"
#include "../Utilities/CONCURRENT_QUEUE.h"

using namespace std;

struct UDP_SERVER_MASTER {
	CONTEXT *ctx;
	char name[MAX_NAME_SIZE];
	int32_t	sockfd;

	volatile bool running;
	bool accepted;

	sockaddr_in client_address;
	int32_t client_address_size;

	thread *recv_thread;

	Queue<UDP_PACKET*> *recv_pool;
	Queue<UDP_PACKET*> *recv_queues[MAX_UDP_CONNECTIONS];
};

bool Initialize_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, CONTEXT*, int);
void Set_Name_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, char*);
bool Set_Recv_Timeout_UDP_SERVER_MASTER(UDP_SERVER_MASTER*, int, int);
bool Set_High_Priority_UDP_SERVER_MASTER(UDP_SERVER_MASTER*);
void Delete_UDP_SERVER_MASTER(UDP_SERVER_MASTER*);

void Recv_Loop_UDP_SERVER_MASTER(UDP_SERVER_MASTER*);

#endif
