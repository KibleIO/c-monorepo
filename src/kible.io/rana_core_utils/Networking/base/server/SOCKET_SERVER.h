#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

#include "../../WS/WS_SERVER.h"
#include "../../../Utilities/KCONTEXT.h"
#include "SOCKET_SERVER_REGISTRY.h"
#include "RECEIVE_CALLBACK_SOCKET_SERVER.h"

struct SOCKET_SERVER {
	int type;
	union {
		WS_SERVER ws_server;
	};
};

bool Initialize_SOCKET_SERVER(SOCKET_SERVER*, Receive_Callback_SOCKET_SERVER,
	SOCKET_SERVER_REGISTRY*, KCONTEXT*, void*);
bool Send_SOCKET_SERVER(SOCKET_SERVER*, char*, int);
void Delete_SOCKET_SERVER(SOCKET_SERVER*);

#endif