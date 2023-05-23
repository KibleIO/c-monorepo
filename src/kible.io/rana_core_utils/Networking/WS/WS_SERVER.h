#ifndef WS_SERVER_H_
#define WS_SERVER_H_

#include "../../Utilities/KCONTEXT.h"
#include "../NETWORK.h"
#include "WS_SERVER_MASTER.h"
#include "../base/server/RECEIVE_CALLBACK_SOCKET_SERVER.h"
#include "../base/server/SOCKET_SERVER_REGISTRY.h"

#define TEST_BUFF_SIZE 4

struct WS_SERVER {
	char name[100];

	KCONTEXT *ctx;
	WS_SERVER_MASTER *ws_master;
	int recv_timeout;
	uint8_t server_id;
};

bool Initialize_WS_SERVER(WS_SERVER*, Receive_Callback_SOCKET_SERVER,
	SOCKET_SERVER_REGISTRY*, KCONTEXT*, void*);
void Delete_WS_SERVER(WS_SERVER*);
bool Send_WS_SERVER(WS_SERVER*, char*, int);
void Set_Name_WS_SERVER(WS_SERVER*, char*);
bool Accept_WS_SERVER(WS_SERVER*);
bool Send_WS_SERVER(WS_SERVER*, char*, int);

bool Set_Recv_Timeout_WS_SERVER(WS_SERVER*, int, int);
bool Set_High_Priority_WS_SERVER(WS_SERVER*);

#endif
