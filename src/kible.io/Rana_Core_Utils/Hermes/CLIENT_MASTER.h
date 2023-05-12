#ifndef CLIENT_MASTER_H_
#define CLIENT_MASTER_H_

#include "../Utilities/KCONTEXT.h"
#include "../Networking/TCP/TCP_CLIENT_MASTER.h"
#include "../Networking/UDP/UDP_CLIENT_MASTER.h"
#include "../Networking/WS/WS_CLIENT_MASTER.h"

struct CLIENT_MASTER {
	KCONTEXT *ctx;
	int type;
	//just tryin to be fancy here, this is totally not needed
	union {
		TCP_CLIENT_MASTER tcp_client_master;
		UDP_CLIENT_MASTER udp_client_master;
		WS_CLIENT_MASTER ws_client_master;
	};
};

bool Initialize_CLIENT_MASTER(CLIENT_MASTER*, KCONTEXT*, int, int, char*);
void Set_Name_CLIENT_MASTER(CLIENT_MASTER*, char*);
bool Set_Recv_Timeout_CLIENT_MASTER(CLIENT_MASTER*, int, int);
bool Set_High_Priority_CLIENT_MASTER(CLIENT_MASTER*);
void Delete_CLIENT_MASTER(CLIENT_MASTER*);

#endif
