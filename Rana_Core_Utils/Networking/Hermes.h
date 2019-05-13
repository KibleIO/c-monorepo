//PLATFORMS: Linux, Windows, OSX

#ifndef HERMES_H_
#define HERMES_H_

#include <string>

#include "Client.h"
#include "Server.h"
#include "ENCRYPTION_ENGINE.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/Stuff.h"

#define HERMES_CONNECTIONS_MAX 20

#define HERMES_PORT_MIN 2230
#define HERMES_PORT_MAX 3329

#define HERMES_AUDIO	1
#define HERMES_DEVICE	2	
#define HERMES_VIDEO	3
#define HERMES_PRINT	4
#define HERMES_DATA		5

#define HERMES_GET_CONNECTION	1
#define HERMES_EXIT 			2
#define HERMES_STATUS 			3

#define HERMES_TIMEOUT_TRIES 1000

//
//  Server
//
struct SERVER_CONNECTION {
	Server* server;
	uint8_t type;
	bool active;
};

struct HERMES_SERVER {
	Server* server;
	ENCRYPTION_ENGINE* enc_eng;
	mutex* cmutx;

	uint16_t baseport;

	volatile bool connected;
	volatile bool shouldexit;

	SERVER_CONNECTION connections[HERMES_CONNECTIONS_MAX];

	uint8_t err;
	
	bool server_init_failed;
};

//Run this first on an allocated hs pointer
bool Initialize_HERMES_SERVER(
HERMES_SERVER* hs, ENCRYPTION_ENGINE* _enc_eng = NULL);
//Then this in a thread
void Connect_HERMES_SERVER(HERMES_SERVER* hs, int port);
//Then this to get servers
Server* Get_HERMES_SERVER(HERMES_SERVER* hs, uint8_t type);
//Or this for blocking behavior
Server* Get_Blocking_HERMES_SERVER(HERMES_SERVER* hs, uint8_t type);
//Closes all connections except main
void Close_Connections_HERMES_SERVER(HERMES_SERVER* hs);
//Deletes all heap resources
void Delete_HERMES_SERVER(HERMES_SERVER* hs);

//
//  Client
//
struct CLIENT_CONNECTION {
	Client* client;
	uint8_t type;
	bool active;
};

struct HERMES_CLIENT {
	Client* client;
	ENCRYPTION_ENGINE* enc_eng;
	mutex* cmutx;

	string ip;
	uint16_t baseport;

	volatile bool connected;

	CLIENT_CONNECTION connections[HERMES_CONNECTIONS_MAX];

	uint8_t err;
};

//Run this first on an allocated hc pointer
bool Initialize_HERMES_CLIENT(
HERMES_CLIENT* hc, ENCRYPTION_ENGINE* _enc_eng = NULL);
//Then this, but not in a thread
bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, string ip, int port, int* types);
//Then this to get clients
Client* Get_HERMES_CLIENT(HERMES_CLIENT* hc, uint8_t type);
//Or this for blocking behavior (though you shouldnt need it)
Client* Get_Blocking_HERMES_CLIENT(HERMES_CLIENT* hc, uint8_t type);
//And this once per main loop
bool Status_HERMES_CLIENT(HERMES_CLIENT* hc);
//Closes all connections except main
void Close_Connections_HERMES_CLIENT(HERMES_CLIENT* hc);
//Deletes all heap resources
void Delete_HERMES_CLIENT(HERMES_CLIENT* hc);

string Hermes_Error_Str(int n);

#endif
