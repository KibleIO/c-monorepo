#ifndef SERVER_H_
#define SERVER_H_

#include "SERVER_MASTER.h"
#include "../Networking/NETWORK.h"
#include "../Utilities/KCONTEXT.h"
#include "../Networking/TCP_SERVER.h"
#include "../Networking/UDP_SERVER.h"

struct SERVER {
	KCONTEXT *ctx;
	int type;
	SERVER_MASTER *master;
	//just tryin to be fancy here, this is totally not needed
	union {
		TCP_SERVER tcp_server;
		UDP_SERVER udp_server;
	};
};

bool Initialize_SERVER(SERVER*, KCONTEXT*, SERVER_MASTER*, int);
void Set_Name_SERVER(SERVER*, char*);
bool Set_Recv_Timeout_SERVER(SERVER*, int, int);
bool Set_High_Priority_SERVER(SERVER*);
bool Accept_SERVER(SERVER*);
bool Send_SERVER(SERVER*, char*, int);
bool Receive_SERVER(SERVER*, char*, int);
int Receive_Unsafe_SERVER(SERVER*, char*);
void Delete_SERVER(SERVER*);

#endif
