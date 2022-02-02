//PLATFORMS: Linux, Windows, OSX

#ifndef HERMES_SERVER_H_
#define HERMES_SERVER_H_

#include <string>
#include "HERMES.h"
#include "SERVER.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/UTILS.h"

//
//  Server
//
struct SERVER_CONNECTION {
	SERVER server;
	HERMES_TYPE type;
	bool active;
	int port;
};

struct HERMES_SERVER {
	SERVER server;
	mutex cmutx;
	thread *loop_thread;

	volatile bool connected;
	volatile bool shouldexit;

	SERVER_CONNECTION connections[HERMES_CONNECTIONS_MAX];

	CONTEXT *ctx;
};

//Run this first on an allocated hs pointer
bool Initialize_HERMES_SERVER(HERMES_SERVER* hs, CONTEXT *ctx);
//Then this
bool Connect_HERMES_SERVER(HERMES_SERVER* hs, int port, int baseport,
	HERMES_TYPE* types);
//Then this to get servers
SERVER* Get_HERMES_SERVER(HERMES_SERVER* hs, HERMES_TYPE type);
void Disconnect_HERMES_SERVER(HERMES_SERVER* hs);
//Deletes all heap resources
void Delete_HERMES_SERVER(HERMES_SERVER* hs);

#endif
