//PLATFORMS: Linux, Windows, OSX

#include "Hermes.h"

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

string Connection_Name(int n) {
	switch (n) {
	case 1:
		return "audio";
	case 2:
		return "device";
	case 3:
		return "video";
	case 4:
		return "print";
	case 5:
		return "data";
	default:
		log_err("no name for connection " + to_string(n));
		return "ERROR";
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
	while (!server && !hs->server_init_failed) {
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
		log_tmp("deleting connection " + to_string(i));
		Hermes_Delete_Client_Connection(hc->connections[i]);
	}
}

void Hermes_Server_Connect(HermesServer* hs, int port) {
	if (hs->connected) {
		log_err("hermes server already connected");
		return;
	}

	hs->baseport = port;
	hs->connected = true;
	hs->server = new Server();

	if (hs->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hs->enc_eng, "hermes_init", hs->enc_eng->active_profile)) {
			log_err("could not add encryption profile hermes_init");
			return;
		}

		ENCRYPTION_PROFILE* enc_prof = 
		hs->enc_eng->profiles_available[hs->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err("failed to load poly1305 key and nonce");
			return;
		}

		hs->server->Set_Encryption_Profile(enc_prof);
	}

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
			log_err("failed to receive flag");
			hs->connected = false;
			hs->server_init_failed = true;
			hs->err = EPIPE;
			continue;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to send status");
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}
		} else if (flag == HERMES_EXIT) {
			log_dbg("Exiting");
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to send exit confirmation");
				hs->connected = false;
				hs->err = EPIPE;
				continue;
			}
			hs->connected = false;
			hs->exiting = true;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to receive connection type");
				hs->connected = false;
				hs->server_init_failed = true;
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

			hs->connections[index].server = server;
			if (hs->enc_eng) {
				if (!Add_Profile_ENCRYPTION_ENGINE(
				hs->enc_eng, Connection_Name(flag),
				hs->enc_eng->active_profile)) {
					log_err("could not add encryption profile " +
					Connection_Name(flag));
					continue;
				}

				ENCRYPTION_PROFILE* enc_prof = 
				hs->enc_eng->profiles_available[
				hs->enc_eng->number_of_profiles - 1];

				if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
					log_err("failed to load poly1305 key and nonce");
					continue;
				}

				hs->connections[index].server->Set_Encryption_Profile(enc_prof); 
			}
			hs->connections[index].active = true;
			hs->connections[index].type = flag;

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
	//cout << "obligatory cout" << endl;
	uint8_t flag = HERMES_GET_CONNECTION;
	//cout << "hello" << endl;
	log_dbg("sending flag");
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send flag");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	flag = type;
	log_dbg("sending type");
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err("could not send type");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	uint16_t port;
	log_dbg("receiving port");
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
	if (hc->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hc->enc_eng, Connection_Name(type), hc->enc_eng->active_profile)) {
			log_err("could not add encryption profile " +
			Connection_Name(type));
			delete hc->connections[index].client;
			return false;
		}

		ENCRYPTION_PROFILE* enc_prof = 
		hc->enc_eng->profiles_available[hc->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err("failed to load poly1305 key and nonce");
			delete hc->connections[index].client;
			return false;
		}

		hc->connections[index].client->Set_Encryption_Profile(enc_prof); 
	}
	hc->connections[index].type = type;
	hc->connections[index].active = true;

	log_dbg("connecting " + to_string(port));
	int attempts = HERMES_TIMEOUT_TRIES;
	while (!hc->connections[index].client->OpenConnection(port, hc->ip) &&
	attempts-- > 0);
	log_dbg("connected " + to_string(port));

	hc->cmutx.unlock();

	return true;
}

bool Hermes_Client_Connect(HermesClient* hc, string ip, int port, int* types) {
	if (hc->connected) {
		log_err("hermes client already connected");
		return false;
	}

	hc->client = new Client();
	hc->client->Set_Recv_Timeout(1);
	hc->baseport = port;
	hc->ip = ip;

	if (hc->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hc->enc_eng, "hermes_init", hc->enc_eng->active_profile)) {
			log_err("could not add encryption profile hermes_init");
			return false;
		}

		ENCRYPTION_PROFILE* enc_prof = 
		hc->enc_eng->profiles_available[hc->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err("failed to load poly1305 key and nonce");
			return false;
		}

		hc->client->Set_Encryption_Profile(enc_prof); 
	}

	log_dbg("connecting to " + hc->ip + ":" + to_string(hc->baseport));
	int attempts = HERMES_TIMEOUT_TRIES;
	while (!hc->client->OpenConnection(hc->baseport, hc->ip) && attempts-- > 0);
	if (attempts < 0) {
		log_err("failed to connect to " + hc->ip + ":" +
		to_string(hc->baseport));
		return false;
	}
	log_dbg("connected to " + hc->ip + ":" + to_string(hc->baseport));

	hc->connected = true;

	log_dbg("creating connections");
	while (*types != 0) {
		log_dbg(to_string(*types));
		Hermes_Client_Create(hc, *types);
		types++;
	}
	log_dbg("hermes connected");

	return true;
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

void Hermes_Client_Init(HermesClient* hc, ENCRYPTION_ENGINE* _enc_eng) {
	hc->enc_eng		= _enc_eng;
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

void Hermes_Server_Init(HermesServer* hs, ENCRYPTION_ENGINE* _enc_eng) {
	hs->enc_eng		= _enc_eng;
	hs->err			= 0;
	hs->server		= NULL;
	hs->connected	= false;
	hs->shouldexit	= false;
	hs->exiting		= false;
	hs->server_init_failed = false;
	hs->cmutx.lock();
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].server = NULL;
		hs->connections[i].active = false;
	}
	hs->cmutx.unlock();
	log_dbg("Hermes server initialized");
}
