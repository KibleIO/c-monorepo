#ifndef _UDP_CLIENT_MASTER_H_
#define _UDP_CLIENT_MASTER_H_

#include <thread>
#include "NETWORK.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Utilities/KCONTEXT.h"
#include "DNS_WRAPPER.h"

using namespace std;

struct UDP_CLIENT_MASTER {
	int32_t sockfd;
	char name[MAX_NAME_SIZE];
	KCONTEXT *ctx;

	volatile bool running;
	bool connected;

	thread *recv_thread;

	sockaddr_in server_address;
	uint32_t server_address_size;

	Queue<UDP_PACKET*> *recv_pool;
	Queue<UDP_PACKET*> *recv_queues[MAX_UDP_CONNECTIONS];
};

bool Initialize_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*, KCONTEXT*, int, char*);
void Set_Name_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*, char*);
bool Set_Recv_Timeout_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*, int, int);
bool Set_High_Priority_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*);
void Delete_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*);

void Recv_Loop_UDP_CLIENT_MASTER(UDP_CLIENT_MASTER*);

#endif
