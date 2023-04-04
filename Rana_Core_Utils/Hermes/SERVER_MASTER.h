#ifndef SERVER_MASTER_H_
#define SERVER_MASTER_H_

#include "../Networking/NETWORK.h"
#include "../Utilities/KCONTEXT.h"
#include "../Networking/TCP/TCP_SERVER_MASTER.h"
#include "../Networking/UDP/UDP_SERVER_MASTER.h"

struct SERVER_MASTER {
	KCONTEXT *ctx;
	int type;
	//just tryin to be fancy here, this is totally not needed
	union {
		TCP_SERVER_MASTER tcp_server_master;
		UDP_SERVER_MASTER udp_server_master;
	};
};

bool Initialize_SERVER_MASTER(SERVER_MASTER*, KCONTEXT*, int, int);
void Set_Name_SERVER_MASTER(SERVER_MASTER*, char*);
bool Set_Recv_Timeout_SERVER_MASTER(SERVER_MASTER*, int, int);
bool Set_High_Priority_SERVER_MASTER(SERVER_MASTER*);
void Delete_SERVER_MASTER(SERVER_MASTER*);

#endif
