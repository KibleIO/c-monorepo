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

	volatile bool connected;
	volatile bool shouldexit;

	SERVER_CONNECTION connections[HERMES_CONNECTIONS_MAX];

	uint8_t err;

	bool server_init_failed;
	CONTEXT *ctx;
};

//Run this first on an allocated hs pointer
bool Initialize_HERMES_SERVER(HERMES_SERVER* hs, CONTEXT *ctx);
//Then this in a thread
void Connect_HERMES_SERVER(HERMES_SERVER* hs, int port, int baseport);
//Then this to get servers
SERVER* Get_HERMES_SERVER(HERMES_SERVER* hs, HERMES_TYPE type);
//Or this for blocking behavior
SERVER* Get_Blocking_HERMES_SERVER(HERMES_SERVER* hs, HERMES_TYPE type);
//Closes all connections except main
void Close_Connections_HERMES_SERVER(HERMES_SERVER* hs);
//Deletes all heap resources
void Delete_HERMES_SERVER(HERMES_SERVER* hs);
//Shuts down hermes and sets err to EPIPE
void Epipe_HERMES_SERVER(HERMES_SERVER* hs);

#endif
