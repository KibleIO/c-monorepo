#ifndef HERMES_H_
#define HERMES_H_

#include <string>

#include <Networking/Client.h>
#include <Networking/Server.h>
#include <Utilities/CONCURRENT_QUEUE.h>
#include <Utilities/LOGGING.h>

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

//
//  Server
//
struct ServerConnection {
	Server* server;
	uint8_t type;
	bool active;
};

struct HermesServer {
	uint16_t baseport;

	Server* server;
	volatile bool connected;
	volatile bool shouldexit;

	ServerConnection connections[HERMES_CONNECTIONS_MAX];
	mutex cmutx;

	uint8_t err;
};

//Run this first on an allocated hs pointer
void Hermes_Server_Init(HermesServer* hs);
//Then this in a thread
void Hermes_Server_Connect(HermesServer* hs, int port);
//Then this to get servers
Server* Hermes_Get_Server(HermesServer* hs, uint8_t type);
//Or this for blocking behavior
Server* Hermes_Get_Server_Blocking(HermesServer* hs, uint8_t type);
//Closes all connections except main
void Hermes_Server_Close_Connections(HermesServer* hs);
//Deletes all heap resources
void Hermes_Delete_Server(HermesServer* hs);

//
//  Client
//
struct ClientConnection {
	Client* client;
	uint8_t type;
	bool active;
};

struct HermesClient {
	string ip;
	uint16_t baseport;

	Client* client;
	volatile bool connected;

	ClientConnection connections[HERMES_CONNECTIONS_MAX];
	mutex cmutx;

	uint8_t err;
};

//Run this first on an allocated hc pointer
void Hermes_Client_Init(HermesClient* hc);
//Then this, but not in a thread
void Hermes_Client_Connect(HermesClient* hc, string ip, int port, int* types);
//Then this to get clients
Client* Hermes_Get_Client(HermesClient* hc, uint8_t type);
//Or this for blocking behavior (though you shouldnt need it)
Client* Hermes_Get_Client_Blocking(HermesClient* hc, uint8_t type);
//And this once per main loop
bool Hermes_Client_Status(HermesClient* hc);
//Closes all connections except main
void Hermes_Client_Close_Connections(HermesClient* hc);
//Deletes all heap resources
void Hermes_Delete_Client(HermesClient* hc);


string Hermes_Error_Str(int n);

#endif
