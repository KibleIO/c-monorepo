#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <signal.h>
#include "NETWORK.h"
#include "../Utilities/CONTEXT.h"

struct TCP_SERVER {
	CONTEXT *ctx;
	char name[MAX_NAME_SIZE];

	int lSocket;
	int cSocket;
};

bool Initialize_TCP_SERVER(TCP_SERVER*, CONTEXT*);
void Set_Name_TCP_SERVER(TCP_SERVER*, char*);
bool Set_Recv_Timeout_TCP_SERVER(TCP_SERVER*, int, int);
bool Set_High_Priority_TCP_SERVER(TCP_SERVER*);
bool Accept_TCP_SERVER(TCP_SERVER*, int);
bool Send_TCP_SERVER(TCP_SERVER*, char*, int);
bool Receive_TCP_SERVER(TCP_SERVER*, char*, int);
int Receive_Unsafe_TCP_SERVER(TCP_SERVER*, char*);
void Delete_TCP_SERVER(TCP_SERVER*);

#endif
