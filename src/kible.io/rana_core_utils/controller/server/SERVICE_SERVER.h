#ifndef _SERVICE_SERVER_H_
#define _SERVICE_SERVER_H_

#include <stdarg.h>
#include "../../Utilities/KCONTEXT.h"
#include "../../Networking/base/server/SOCKET_SERVER.h"
#include "../../Networking/base/server/SOCKET_SERVER_REGISTRY.h"

#define MAX_SERVICES 30

struct SERVICE_SERVER_REGISTRY;

struct SERVICE_SERVER {
	virtual bool Initialize(KCONTEXT*, SERVICE_SERVER_REGISTRY*) = 0;
	virtual void Delete() = 0;
};

struct SERVICE_SERVER_REGISTRY {
	KCONTEXT *ctx;
	SOCKET_SERVER_REGISTRY socket_server_registry;
	SERVICE_SERVER *service_server[MAX_SERVICES];
	int service_count;
};

bool Initialize_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY*, KCONTEXT*,
	int count, ...);

template<typename T>
T Get_Instance_Of_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY*);


void Delete_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY*);

#endif