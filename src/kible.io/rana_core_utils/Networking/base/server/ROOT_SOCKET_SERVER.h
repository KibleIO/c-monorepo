#ifndef _ROOT_SOCKET_SERVER_H_
#define _ROOT_SOCKET_SERVER_H_

#include "../../WS/WS_SERVER_MASTER.h"

struct ROOT_SOCKET_SERVER {
	int type;
	bool initialized;
	union {
		WS_SERVER_MASTER ws_server_master;
	};
};

bool Initialize_ROOT_SOCKET_SERVER(ROOT_SOCKET_SERVER*, KCONTEXT*, int);
void Delete_ROOT_SOCKET_SERVER(ROOT_SOCKET_SERVER*);

#endif