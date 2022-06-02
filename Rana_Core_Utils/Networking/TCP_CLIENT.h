#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

//#include <netdb.h>
#include <signal.h>
#include <string>
#include <string.h>
#include "NETWORK.h"
#include "TCP_CLIENT_MASTER.h"
#include "../Utilities/TIMER.h"
#include "../Utilities/KCONTEXT.h"
#include "DNS_WRAPPER.h"

struct TCP_CLIENT {
	KCONTEXT *ctx;
	char name[MAX_NAME_SIZE];
	#ifdef _WIN64

	SOCKET cSocket;

	#else

	int cSocket;

	#endif
	TCP_CLIENT_MASTER *tcp_master;
};

bool Initialize_TCP_CLIENT(TCP_CLIENT*, KCONTEXT*, TCP_CLIENT_MASTER*, int);
void Set_Name_TCP_CLIENT(TCP_CLIENT*, char*);
bool Set_Recv_Timeout_TCP_CLIENT(TCP_CLIENT*, int, int);
bool Set_High_Priority_TCP_CLIENT(TCP_CLIENT*);
bool Connect_TCP_CLIENT(TCP_CLIENT*);
bool Send_TCP_CLIENT(TCP_CLIENT*, char*, int);
bool Receive_TCP_CLIENT(TCP_CLIENT*, char*, int);
int Receive_Unsafe_TCP_CLIENT(TCP_CLIENT*, char*);
void Delete_TCP_CLIENT(TCP_CLIENT*);

#endif
