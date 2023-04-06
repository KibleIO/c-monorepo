//PLATFORMS: Linux, Windows, OSX

#ifndef HERMES_CLIENT_H_
#define HERMES_CLIENT_H_

#include <Utilities/FPS_LIMITER.h>
#include <string>
#include "HERMES.h"
#include "CLIENT.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/UTILS.h"

//
//  Client
//
struct CLIENT_CONNECTION {
	CLIENT client;
	HERMES_TYPE type;
	bool active;
};

struct HERMES_CLIENT {
	CLIENT_MASTER master;
	CLIENT client;
	mutex cmutx;
	int port;
        bool use_tcp;

	char ip[MAX_NAME_SIZE];
	FPS_LIMITER fps_limiter;
	volatile bool connected;

	CLIENT_CONNECTION connections[HERMES_CONNECTIONS_MAX];

	KCONTEXT *ctx;
};

//Run this first on an allocated hc pointer
bool Initialize_HERMES_CLIENT(HERMES_CLIENT* hc, KCONTEXT *ctx, char *ip,
	int port);
//Then this, but not in a thread
bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, HERMES_TYPE* types);
//Then this to get clients
CLIENT* Get_HERMES_CLIENT(HERMES_CLIENT* hc, HERMES_TYPE type);
//And this once per main loop
uint8_t Status_HERMES_CLIENT(HERMES_CLIENT* hc);
void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc);
//Deletes all heap resources
void Delete_HERMES_CLIENT(HERMES_CLIENT* hc);

#endif
