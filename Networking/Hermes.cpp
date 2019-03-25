#include <Networking/Hermes.h>

string Hermes_Error_Str(int n) {
	switch (n) {
	case 0:
		return "successful exit";
	case EPIPE:
		return "broken pipe";
	case EIO:
		return "corrupt connection";
	default:
		return "unknown error";
	}
}

void Hermes_Server_Close_Connections(HermesServer* hs) {
	ServerConnection* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (sc[i].active) {
			sc[i].server->CloseConnection();
		}
	}
}

void Hermes_Client_Close_Connections(HermesClient* hc) {
	ClientConnection* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (cc[i].active) {
			cc[i].client->CloseConnection();
		}
	}
}

Client* Hermes_Get_Client_Blocking(HermesClient* hc, uint8_t type) {
	Client* client = NULL;
	while (!client) {
		client = Hermes_Get_Client(hc, type);
	}
	return client;
}

Client* Hermes_Get_Client(HermesClient* hc, uint8_t type) {
	if (!hc->connected) {
		log_err("hermes server not connected");
		return NULL;
	}
	hc->cmutx.lock();
	ClientConnection* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (cc[i].active && cc[i].type == type) {
			Client* client = cc[i].client;
			hc->cmutx.unlock();
			return client;
		}
	}
	hc->cmutx.unlock();
	return NULL;
}

Server* Hermes_Get_Server_Blocking(HermesServer* hs, uint8_t type) {
	Server* server = NULL;
	while (!server) {
		server = Hermes_Get_Server(hs, type);
	}
	return server;
}

Server* Hermes_Get_Server(HermesServer* hs, uint8_t type) {
	if (!hs->connected) {
		return NULL;
	}
	hs->cmutx.lock();
	ServerConnection* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (sc[i].active && sc[i].type == type) {
			Server* server = sc[i].server;
			hs->cmutx.unlock();
			return server;
		}
	}
	hs->cmutx.unlock();
	return NULL;
}


int Hermes_Client_Get_Index(HermesClient* hc) {
	if (!hc->connected) {
		log_err("hermes client not connected");
		return -1;
	}
	hc->cmutx.lock();
	ClientConnection* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!cc[i].active) {
			hc->cmutx.unlock();
			return i;
		}
	}
	hc->cmutx.unlock();
	return -1;
}

int Hermes_Server_Get_Index(HermesServer* hs) {
	if (!hs->connected) {
		log_err("hermes server not connected");
		return -1;
	}
	hs->cmutx.lock();
	ServerConnection* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!sc[i].active) {
			hs->cmutx.unlock();
			return i;
		}
	}
	hs->cmutx.unlock();
	return -1;
}


void Hermes_Delete_Server_Connection(ServerConnection &sc) {
	if (!sc.active) {
		return;
	}
	sc.server->CloseConnection();
	delete sc.server;
	sc.server = NULL;
	sc.active = false;
}

void Hermes_Delete_Server(HermesServer* hs) {
	if (!hs->connected) {
		log_err("hermes server not connected");
	}
	hs->connected = false;
	hs->server->CloseConnection();
	delete hs->server;
	hs->server = NULL;
	hs->shouldexit = false;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		Hermes_Delete_Server_Connection(hs->connections[i]);
	}
	delete hs;
}

void Hermes_Delete_Client_Connection(ClientConnection &cc) {
	if (!cc.active) {
		return;
	}
	cc.client->CloseConnection();
	delete cc.client;
	cc.client = NULL;
	cc.active = false;
}

void Hermes_Delete_Client(HermesClient* hc) {
	hc->connected = false;
	hc->client->CloseConnection();
	delete hc->client;
	hc->client= NULL;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		Hermes_Delete_Client_Connection(hc->connections[i]);
	}
	delete hc;
}
void Hermes_Server_Connect(HermesServer* hs, int port) {
	if (hs->connected) {
		log_err("hermes server already connected");
		return;
	}

	hs->baseport = port;
	hs->connected = true;
	hs->server = new Server();

	log_dbg("Listening on " + to_string(hs->baseport));
	if (!hs->server->Listen(hs->baseport)) {
		log_err("could not listen on port " + to_string(hs->baseport));
		delete hs->server;
		hs->connected = false;
		return;
	}
	log_dbg("bound");

	uint8_t flag;

	while (hs->connected) {
		if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
			hs->connected = false;
			hs->err = EPIPE;
			continue;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}
		} else if (flag == HERMES_EXIT) {
			log_dbg("Exiting");
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}
			hs->connected = false;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}

			uint16_t port = HERMES_PORT_MIN;
			Server* server = new Server();

			while (port <= HERMES_PORT_MAX) {
				if (server->Bind(port)) {
					break;
				}
				port++;
			}

			if (port > HERMES_PORT_MAX) {
				log_err("out of ports");
				delete server;
				continue;
			}

			if (!hs->server->Send((char*)&port, sizeof(uint16_t))) {
				delete server;
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}

			int index = Hermes_Server_Get_Index(hs);
			if (index < 0) {
				log_err("max connections reached");
				delete server;
				continue;
			}

			hs->cmutx.lock();
			hs->connections[index].active = true;
			hs->connections[index].type = flag;
			hs->connections[index].server = server;

			log_dbg("server listening on " + to_string(port));
			if (!hs->connections[index].server->ListenBound()) {
				log_err("could not listen on port " + to_string(port));
				Hermes_Delete_Server_Connection(hs->connections[index]);
			} else {
				log_dbg("server bound on " + to_string(port));
			}
			hs->cmutx.unlock();
		}
	}
	hs->shouldexit = true;
	Hermes_Server_Close_Connections(hs);
}

bool Hermes_Client_Create(HermesClient* hc, uint8_t type) {
	if (!hc->connected) {
		log_err("hermes client not connected");
		return false;
	}
	cout << "obligatory cout" << endl;
	uint8_t flag = HERMES_GET_CONNECTION;
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send flag");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	flag = type;
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send type");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	uint16_t port;
	if (!hc->client->Receive((char*)&port, sizeof(uint16_t))) {
		log_err("could not receive port");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}

	int index = Hermes_Client_Get_Index(hc);
	if (index < 0) {
		log_err("max connections reached");
	}

	hc->cmutx.lock();

	hc->connections[index].client = new Client;
	hc->connections[index].type = type;
	hc->connections[index].active = true;

	log_dbg("connecting " + to_string(port));
	while (!hc->connections[index].client->OpenConnection(port, hc->ip));
	log_dbg("connected " + to_string(port));

	hc->cmutx.unlock();
}

void Hermes_Client_Connect(HermesClient* hc, string ip, int port, int* types) {
	if (hc->connected) {
		log_err("hermes client already connected");
		return;
	}

	hc->client = new Client;
	hc->baseport = port;
	hc->ip = ip;

	log_dbg("connecting to " + hc->ip + ":" + to_string(hc->baseport));
	while (!hc->client->OpenConnection(hc->baseport, hc->ip));
	log_dbg("connected to " + hc->ip + ":" + to_string(hc->baseport));

	hc->connected = true;

	log_dbg("creating connections");
	while (*types != 0) {
		Hermes_Client_Create(hc, *types);
		types++;
	}
	log_dbg("hermes connected");
}

void Hermes_Client_Disconnect(HermesClient* hc) {
	if (!hc->connected) {
		log_err("hermes client not connected");
		return;
	}

	uint8_t flag = HERMES_EXIT;

	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send exit flag");
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (!hc->client->Receive((char*)&flag, sizeof(uint8_t))) {
		log_err("could not receive exit flag");
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (flag != HERMES_EXIT) {
		log_err("flag received not exit");
		hc->err = EIO;
	}
}

bool Hermes_Client_Status(HermesClient* hc) {
	if (!hc->connected) {
		log_err("hermes client not connected");
		return false;
	}

	uint8_t flag = HERMES_STATUS;
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send status flag");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (!hc->client->Receive((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send status flag");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (flag) { //shouldexit
		Hermes_Client_Disconnect(hc);
		return false;
	}
	return true;
}

void Hermes_Client_Init(HermesClient* hc) {
	hc->err			= 0;
	hc->client		= NULL;
	hc->connected	= false;
	hc->cmutx.lock();
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hc->connections[i].client = NULL;
		hc->connections[i].active = false;
	}
	hc->cmutx.unlock();
	log_dbg("Hermes client initialized");
}

void Hermes_Server_Init(HermesServer* hs) {
	hs->err			= 0;
	hs->server		= NULL;
	hs->connected	= false;
	hs->shouldexit	= false;
	hs->cmutx.lock();
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].server = NULL;
		hs->connections[i].active = false;
	}
	hs->cmutx.unlock();
	log_dbg("Hermes server initialized on");
}
