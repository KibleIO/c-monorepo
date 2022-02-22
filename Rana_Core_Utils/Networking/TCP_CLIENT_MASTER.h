#ifndef TCP_CLIENT_MASTER_H_
#define TCP_CLIENT_MASTER_H_

#include "../Utilities/CONTEXT.h"
#include "NETWORK.h"

struct TCP_CLIENT_MASTER {
	char name[MAX_NAME_SIZE];
	char ip[IP_ADDR_STR_LEN];
	int port;
};

bool Initialize_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER*, CONTEXT*, int, char*);
void Set_Name_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER*, char*);
bool Set_Recv_Timeout_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER*, int, int);
bool Set_High_Priority_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER*);
void Delete_TCP_CLIENT_MASTER(TCP_CLIENT_MASTER*);

#endif
