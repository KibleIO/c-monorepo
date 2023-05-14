#ifndef TCP_SERVER_MASTER_H_
#define TCP_SERVER_MASTER_H_

#include <signal.h>
#include "../NETWORK.h"
#include "../../Utilities/KCONTEXT.h"

struct TCP_SERVER_MASTER {
	KCONTEXT *ctx;
	char name[MAX_NAME_SIZE];

	int lSocket;
};

bool Initialize_TCP_SERVER_MASTER(TCP_SERVER_MASTER*, KCONTEXT*, int);
void Set_Name_TCP_SERVER_MASTER(TCP_SERVER_MASTER*, char*);
bool Set_Recv_Timeout_TCP_SERVER_MASTER(TCP_SERVER_MASTER*, int, int);
bool Set_High_Priority_TCP_SERVER_MASTER(TCP_SERVER_MASTER*);
void Delete_TCP_SERVER_MASTER(TCP_SERVER_MASTER*);

#endif
