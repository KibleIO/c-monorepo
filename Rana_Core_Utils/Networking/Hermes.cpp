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

void Close_Connections_HERMES_SERVER(HERMES_SERVER* hs) {
	SERVER_CONNECTION* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (sc[i].active) {
			sc[i].server->CloseConnection();
		}
	}
}

void Close_Connections_HERMES_CLIENT(HERMES_CLIENT* hc) {
	CLIENT_CONNECTION* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (cc[i].active) {
			cc[i].client->CloseConnection();
		}
	}
}

Client* Get_Blocking_HERMES_CLIENT(HERMES_CLIENT* hc, uint8_t type) {
	Client* client = NULL;
	while (!client) {
		client = Get_HERMES_CLIENT(hc, type);
	}
	return client;
}

Client* Get_HERMES_CLIENT(HERMES_CLIENT* hc, uint8_t type) {
	if (!hc->connected) {
		log_err("hermes server not connected");
		return NULL;
	}
	hc->cmutx->lock();
	CLIENT_CONNECTION* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (cc[i].active && cc[i].type == type) {
			Client* client = cc[i].client;
			hc->cmutx->unlock();
			return client;
		}
	}
	hc->cmutx->unlock();
	return NULL;
}

Server* Get_Blocking_HERMES_SERVER(HERMES_SERVER* hs, uint8_t type) {
	Server* server = NULL;
	while (!server && !hs->server_init_failed) {
		server = Get_HERMES_SERVER(hs, type);
	}
	return server;
}

Server* Get_HERMES_SERVER(HERMES_SERVER* hs, uint8_t type) {
	if (!hs->connected) {
		return NULL;
	}
	hs->cmutx->lock();
	SERVER_CONNECTION* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (sc[i].active && sc[i].type == type) {
			Server* server = sc[i].server;
			hs->cmutx->unlock();
			return server;
		}
	}
	hs->cmutx->unlock();
	return NULL;
}

int Get_Index_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err("hermes client not connected");
		return -1;
	}
	hc->cmutx->lock();
	CLIENT_CONNECTION* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!cc[i].active) {
			hc->cmutx->unlock();
			return i;
		}
	}
	hc->cmutx->unlock();
	return -1;
}

int Get_Index_HERMES_SERVER(HERMES_SERVER* hs) {
	if (!hs->connected) {
		log_err("hermes server not connected");
		return -1;
	}
	hs->cmutx->lock();
	SERVER_CONNECTION* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!sc[i].active) {
			hs->cmutx->unlock();
			return i;
		}
	}
	hs->cmutx->unlock();
	return -1;
}

void Delete_SERVER_CONNECTION(SERVER_CONNECTION &sc) {
	delete sc.server;
	sc = NULLIFY;
}

void Delete_HERMES_SERVER(HERMES_SERVER* hs) {
	delete hs->server;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		Delete_SERVER_CONNECTION(hs->connections[i]);
	}
	delete hs->cmutx;
	*hs = NULLIFY;
}

void Delete_CLIENT_CONNECTION(CLIENT_CONNECTION &cc) {
	delete cc.client;
	cc = NULLIFY;
}

void Delete_HERMES_CLIENT(HERMES_CLIENT* hc) {
	delete hc->client;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		Delete_CLIENT_CONNECTION(hc->connections[i]);
	}
	delete hc->cmutx;
	*hc = NULLIFY;
}

void Connect_HERMES_SERVER(HERMES_SERVER* hs, int port) {
	if (hs->connected) {
		log_err("hermes server already connected");
		return;
	}

	hs->baseport = port;
	hs->connected = true;
	hs->server = new Server();
	hs->server->Set_Name("hermes server");
	hs->server->Set_Recv_Timeout(5);

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
			hs->server_init_failed = true;
			hs->err = EPIPE;
			break;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (hs->shouldexit) {
				log_dbg("sending quit packet");
			}
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to send status");
				hs->shouldexit = false;
				hs->err = EPIPE;
				break;
			}
			if (hs->shouldexit) {
				log_dbg("quit packet sent");
			}
		} else if (flag == HERMES_EXIT) {
			log_dbg("Exiting");
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to send exit confirmation");
				hs->err = EPIPE;
				break;
			}
			hs->shouldexit = false;
			hs->connected = false;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
				log_err("failed to receive connection type");
				hs->server_init_failed = true;
				hs->err = EPIPE;
				break;
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
				hs->err = EPIPE;
				break;
			}

			if (!hs->server->Send((char*)&port, sizeof(uint16_t))) {
				log_err("failed to send port");
				delete server;
				hs->err = EPIPE;
				break;
			}

			int index = Get_Index_HERMES_SERVER(hs);
			if (index < 0) {
				log_err("max connections reached");
				delete server;
				hs->err = EPIPE;
				break;
			}

			hs->cmutx->lock();

			hs->connections[index].server = server;
			hs->connections[index].server->Set_Name(Connection_Name(flag));
			if (hs->enc_eng) {
				if (!Add_Profile_ENCRYPTION_ENGINE(
				hs->enc_eng, Connection_Name(flag),
				hs->enc_eng->active_profile)) {
					log_err("could not add encryption profile " +
					Connection_Name(flag));
					hs->err = EPIPE;
					break;
				}

				ENCRYPTION_PROFILE* enc_prof = 
				hs->enc_eng->profiles_available[
				hs->enc_eng->number_of_profiles - 1];

				if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
					log_err("failed to load poly1305 key and nonce");
					hs->err = EPIPE;
					break;
				}

				hs->connections[index].server->Set_Encryption_Profile(enc_prof); 
			}
			hs->connections[index].active = true;
			hs->connections[index].type = flag;

			log_dbg("server listening on " + to_string(port));
			if (!hs->connections[index].server->ListenBound()) {
				log_err("could not listen on port " + to_string(port));
				Delete_SERVER_CONNECTION(hs->connections[index]);
			} else {
				log_dbg("server bound on " + to_string(port));
			}
			hs->cmutx->unlock();
		}
	}
	if (hs->shouldexit) {
		hs->shouldexit = false;
	}
	hs->connected = false;
	Close_Connections_HERMES_SERVER(hs);
}

bool Create_CLIENT_CONNECTION(HERMES_CLIENT* hc, uint8_t type) {
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

	int index = Get_Index_HERMES_CLIENT(hc);
	if (index < 0) {
		log_err("max connections reached");
	}

	hc->cmutx->lock();

	hc->connections[index].client = new Client;
	hc->connections[index].client->Set_Name(Connection_Name(type));
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

	hc->cmutx->unlock();

	return true;
}

bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, string ip, int port, int* types) {
	if (hc->connected) {
		log_err("hermes client already connected");
		return false;
	}

	hc->client = new Client();
	hc->client->Set_Recv_Timeout(5);
	hc->client->Set_Name("hermes client");
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
		Create_CLIENT_CONNECTION(hc, *types);
		types++;
	}
	log_dbg("hermes connected");

	return true;
}

void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc) {
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

bool Status_HERMES_CLIENT(HERMES_CLIENT* hc) {
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
		log_err("could not receive status flag");
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (flag) { //shouldexit
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	return true;
}

bool Initialize_HERMES_CLIENT(HERMES_CLIENT* hc, ENCRYPTION_ENGINE* _enc_eng) {
	if (hc->client || hc->enc_eng || hc->cmutx || hc->baseport ||
	hc->connected || hc->err) {
		log_err("hermes client struct not properly nullified");
		return false;
	}
	hc->enc_eng		= _enc_eng;
	hc->err			= 0;
	hc->client		= NULL;
	hc->connected	= false;
	hc->cmutx = new mutex;
	hc->cmutx->lock();
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hc->connections[i] = NULLIFY;
	}
	hc->cmutx->unlock();
	log_dbg("Hermes client initialized");
	return true;
}

bool Initialize_HERMES_SERVER(HERMES_SERVER* hs, ENCRYPTION_ENGINE* _enc_eng) {
	if (hs->server || hs->enc_eng || hs->cmutx || hs->baseport ||
	hs->connected || hs->shouldexit || hs->err | hs->server_init_failed) {
		log_err("hermes client struct not properly nullified");
		return false;
	}
	hs->enc_eng		= _enc_eng;
	hs->err			= 0;
	hs->connected	= false;
	hs->shouldexit	= false;
	hs->server_init_failed = false;
	hs->cmutx = new mutex;
	hs->cmutx->lock();
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i] = NULLIFY;
	}
	hs->cmutx->unlock();
	log_dbg("Hermes server initialized");
	return true;
}
