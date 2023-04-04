#ifndef WS_CLIENT_H_
#define WS_CLIENT_H_

#include "WS_CLIENT_MASTER.h"

#define TEST_BUFF_SIZE 4
#define DEFAULT_RECV_TIMEOUT 1

struct WS_CLIENT {
	char name[100];
	WS_CLIENT_MASTER *ws_master;
	KCONTEXT *ctx;
	int recv_timeout;
	uint8_t client_id;
};

bool Initialize_WS_CLIENT(WS_CLIENT*, KCONTEXT*, WS_CLIENT_MASTER*, int);
void Set_Name_WS_CLIENT(WS_CLIENT*, char*);
bool Set_Recv_Timeout_WS_CLIENT(WS_CLIENT*, int, int);
bool Set_High_Priority_WS_CLIENT(WS_CLIENT*);
bool Connect_WS_CLIENT(WS_CLIENT*);
bool Send_WS_CLIENT(WS_CLIENT*, char*, int);
bool Receive_WS_CLIENT(WS_CLIENT*, char*, int);
int Receive_Unsafe_WS_CLIENT(WS_CLIENT*, char*);
void Delete_WS_CLIENT(WS_CLIENT*);

#endif