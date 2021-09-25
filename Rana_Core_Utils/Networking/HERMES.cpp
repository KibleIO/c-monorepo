//PLATFORMS: Linux, Windows, OSX

#include "HERMES.h"

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
		log_err(((const JSON_TYPE){
			{"message", "no name for connection"},
			JSON_TYPE_END}));
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
		log_err(((const JSON_TYPE){
			{"message", "hermes server not connected"},
			JSON_TYPE_END}));
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
		log_err(((const JSON_TYPE){
			{"message", "hermes client not connected"},
			JSON_TYPE_END}));
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
		log_err(((const JSON_TYPE){
			{"message", "hermes server not connected"},
			JSON_TYPE_END}));
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

void Epipe_HERMES_SERVER(HERMES_SERVER* hs) {
	hs->err = EPIPE;
	hs->connected = false;
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

void Epipe_HERMES_CLIENT(HERMES_CLIENT* hc) {
	hc->err = EPIPE;
	hc->connected = false;
}

void Connect_HERMES_SERVER(HERMES_SERVER* hs, int port, int baseport) {
	if (hs->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes server already connected"},
			JSON_TYPE_END}));
		return;
	}

	hs->baseport = port;
	hs->connected = true;
	hs->server = new Server();
	hs->server->Set_Name("hermes server");
	hs->server->Set_Recv_Timeout(3000);

	if (hs->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hs->enc_eng, "hermes_init", hs->enc_eng->active_profile)) {
			log_err(((const JSON_TYPE){
				{"message", "could not add encryption profile hermes_init"},
				JSON_TYPE_END}));
			return;
		}

		ENCRYPTION_PROFILE* enc_prof =
		hs->enc_eng->profiles_available[hs->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err(((const JSON_TYPE){
				{"message", "failed to load poly1305 key and nonce"},
				JSON_TYPE_END}));
			return;
		}

		hs->server->Set_Encryption_Profile(enc_prof);
	}

	log_dbg(((const JSON_TYPE){
		{"message", "Listening on"},
		JSON_TYPE_END}));
	if (!hs->server->Listen(hs->baseport)) {
		log_err(((const JSON_TYPE){
			{"message", "could not listen on port"},
			JSON_TYPE_END}));
		delete hs->server;
		hs->server_init_failed = true;
		hs->connected = false;
		return;
	}
	log_dbg(((const JSON_TYPE){
		{"message", "bound"},
		JSON_TYPE_END}));

	uint8_t flag;

	log_dbg(((const JSON_TYPE){
		{"message", "starting server loop"},
		JSON_TYPE_END}));
	while (hs->connected) {
		if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
			log_err(((const JSON_TYPE){
				{"message", "failed to receive flag"},
				JSON_TYPE_END}));
			hs->server_init_failed = true;
			hs->err = EPIPE;
			break;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (hs->shouldexit) {
				log_err(((const JSON_TYPE){
					{"message", "sending quit packet"},
					JSON_TYPE_END}));
			}
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to send status"},
					JSON_TYPE_END}));
				hs->shouldexit = false;
				hs->err = EPIPE;
				break;
			}
			if (hs->shouldexit) {
				log_err(((const JSON_TYPE){
					{"message", "quit packet sent"},
					JSON_TYPE_END}));
			}
		} else if (flag == HERMES_EXIT) {
			log_dbg(((const JSON_TYPE){
				{"message", "Exiting"},
				JSON_TYPE_END}));
			if (!hs->server->Send((char*)&flag, sizeof(uint8_t))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to send exit confirmation"},
					JSON_TYPE_END}));
				hs->err = EPIPE;
				break;
			}
			hs->shouldexit = false;
			hs->connected = false;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!hs->server->Receive((char*)&flag, sizeof(uint8_t))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to receive connection type"},
					JSON_TYPE_END}));
				hs->server_init_failed = true;
				hs->err = EPIPE;
				break;
			}

			uint16_t port = baseport;
			uint16_t max_port = baseport + 1000;

			Server* server = new Server();

			while (port <= max_port) {
				if (server->Bind(port)) {
					break;
				}
				port++;
			}

			if (port > max_port) {
				log_err(((const JSON_TYPE){
					{"message", "out of ports"},
					JSON_TYPE_END}));
				delete server;
				hs->err = EPIPE;
				break;
			}

			if (!hs->server->Send((char*)&port, sizeof(uint16_t))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to send port"},
					JSON_TYPE_END}));
				delete server;
				hs->err = EPIPE;
				break;
			}

			int index = Get_Index_HERMES_SERVER(hs);
			if (index < 0) {
				log_err(((const JSON_TYPE){
					{"message", "max connections reached"},
					JSON_TYPE_END}));
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
					log_err(((const JSON_TYPE){
						{"message", "could not add encryption profile"},
						JSON_TYPE_END}));
					hs->err = EPIPE;
					break;
				}

				ENCRYPTION_PROFILE* enc_prof =
				hs->enc_eng->profiles_available[
				hs->enc_eng->number_of_profiles - 1];

				if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
					log_err(((const JSON_TYPE){
						{"message", "failed to load poly1305 key and nonce"},
						JSON_TYPE_END}));
					hs->err = EPIPE;
					break;
				}

				hs->connections[index].server->Set_Encryption_Profile(enc_prof);
			}
			hs->connections[index].active = true;
			hs->connections[index].type = flag;

			log_dbg(((const JSON_TYPE){
				{"message", "server listening on"},
				JSON_TYPE_END}));
			if (!hs->connections[index].server->ListenBound()) {
				log_err(((const JSON_TYPE){
					{"message", "could not listen on port"},
					JSON_TYPE_END}));
				Delete_SERVER_CONNECTION(hs->connections[index]);
			} else {
				log_dbg(((const JSON_TYPE){
					{"message", "server bound on"},
					JSON_TYPE_END}));
			}
			hs->cmutx->unlock();
		}
	}
	log_err(((const JSON_TYPE){
		{"message", "ending server loop"},
		JSON_TYPE_END}));
	if (hs->shouldexit) {
		hs->shouldexit = false;
	}
	hs->connected = false;
	Close_Connections_HERMES_SERVER(hs);
}

bool Create_CLIENT_CONNECTION(HERMES_CLIENT* hc, uint8_t type) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client not connected"},
			JSON_TYPE_END}));
		return false;
	}
	//cout << "obligatory cout" << endl;
	uint8_t flag = HERMES_GET_CONNECTION;
	//cout << "hello" << endl;
	log_dbg(((const JSON_TYPE){
		{"message", "sending flag"},
		JSON_TYPE_END}));
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not send flag"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	flag = type;
	log_err(((const JSON_TYPE){
		{"message", "sending type"},
		JSON_TYPE_END}));
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not send type"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	uint16_t port;
	log_dbg(((const JSON_TYPE){
		{"message", "receiving port"},
		JSON_TYPE_END}));
	if (!hc->client->Receive((char*)&port, sizeof(uint16_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not receive port"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}

	int index = Get_Index_HERMES_CLIENT(hc);
	if (index < 0) {
		log_err(((const JSON_TYPE){
			{"message", "max connections reached"},
			JSON_TYPE_END}));
	}

	hc->cmutx->lock();

	hc->connections[index].client = new Client;
	hc->connections[index].client->Set_Name(Connection_Name(type));
	if (hc->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hc->enc_eng, Connection_Name(type), hc->enc_eng->active_profile)) {
			log_err(((const JSON_TYPE){
				{"message", "could not add encryption profile"},
				JSON_TYPE_END}));
			delete hc->connections[index].client;
			return false;
		}

		ENCRYPTION_PROFILE* enc_prof =
		hc->enc_eng->profiles_available[hc->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err(((const JSON_TYPE){
				{"message", "failed to load poly1305 key and nonce"},
				JSON_TYPE_END}));
			delete hc->connections[index].client;
			return false;
		}

		hc->connections[index].client->Set_Encryption_Profile(enc_prof);
	}
	hc->connections[index].type = type;
	hc->connections[index].active = true;

	log_dbg(((const JSON_TYPE){
		{"message", "connecting"},
		JSON_TYPE_END}));
	int attempts = HERMES_TIMEOUT_TRIES;
	while (!hc->connections[index].client->OpenConnection(port, hc->ip) &&
	attempts-- > 0) {
		hc->connections[index].client->CloseConnection();
		hc->connections[index].client->Init();
	}
	log_dbg(((const JSON_TYPE){
		{"message", "connected"},
		JSON_TYPE_END}));

	hc->cmutx->unlock();

	return true;
}

bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, string ip, int port, int* types) {
	if (hc->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client already connected"},
			JSON_TYPE_END}));
		return false;
	}

	hc->client = new Client();
	hc->client->Set_Recv_Timeout(30);
	hc->client->Set_Name("hermes client");
	hc->baseport = port;
	hc->ip = ip;

	if (hc->enc_eng) {
		if (!Add_Profile_ENCRYPTION_ENGINE(
		hc->enc_eng, "hermes_init", hc->enc_eng->active_profile)) {
			log_err(((const JSON_TYPE){
				{"message", "could not add encryption profile hermes_init"},
				JSON_TYPE_END}));
			return false;
		}

		ENCRYPTION_PROFILE* enc_prof =
		hc->enc_eng->profiles_available[hc->enc_eng->number_of_profiles - 1];

		if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
			log_err(((const JSON_TYPE){
				{"message", "failed to load poly1305 key and nonce"},
				JSON_TYPE_END}));
			return false;
		}

		hc->client->Set_Encryption_Profile(enc_prof);
	}

	log_dbg(((const JSON_TYPE){
		{"message", "connecting to"},
		JSON_TYPE_END}));
	//come on man - Joe Biden
	int attempts = HERMES_TIMEOUT_TRIES * HERMES_TIMEOUT_TRIES;
	while (
	!hc->client->OpenConnection(hc->baseport, hc->ip) && attempts-- > 0) {
		hc->client->CloseConnection();
		hc->client->Init();
	}
	if (attempts < 0) {
		log_err(((const JSON_TYPE){
			{"message", "failed to connect to"},
			JSON_TYPE_END}));
		return false;
	}
	log_dbg(((const JSON_TYPE){
		{"message", "connected to"},
		JSON_TYPE_END}));

	hc->connected = true;

	log_dbg(((const JSON_TYPE){
		{"message", "creating connections"},
		JSON_TYPE_END}));
	while (*types != 0) {
		log_dbg(((const JSON_TYPE){
			{"message", "to_string(*types)"},
			JSON_TYPE_END}));
		Create_CLIENT_CONNECTION(hc, *types);
		types++;
	}
	log_dbg(((const JSON_TYPE){
		{"message", "hermes connected"},
		JSON_TYPE_END}));

	return true;
}

void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client not connected"},
			JSON_TYPE_END}));
		return;
	}

	uint8_t flag = HERMES_EXIT;

	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not send exit flag"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (!hc->client->Receive((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not receive exit flag"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (flag != HERMES_EXIT) {
		log_err(((const JSON_TYPE){
			{"message", "flag received not exit"},
			JSON_TYPE_END}));
		hc->err = EIO;
	}
}

bool Status_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client not connected"},
			JSON_TYPE_END}));
		return false;
	}

	uint8_t flag = HERMES_STATUS;
	if (!hc->client->Send((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not send status flag"},
			JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (!hc->client->Receive((char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){
			{"message", "could not receive status flag"},
			JSON_TYPE_END}));
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
		log_err(((const JSON_TYPE){
			{"message", "hermes client struct not properly nullified"},
			JSON_TYPE_END}));
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
	log_dbg(((const JSON_TYPE){
		{"message", "Hermes client initialized"},
		JSON_TYPE_END}));
	return true;
}

bool Initialize_HERMES_SERVER(HERMES_SERVER* hs, ENCRYPTION_ENGINE* _enc_eng) {
	if (hs->server || hs->enc_eng || hs->cmutx || hs->baseport ||
	hs->connected || hs->shouldexit || hs->err | hs->server_init_failed) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client struct not properly nullified"},
			JSON_TYPE_END}));
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
	log_dbg(((const JSON_TYPE){
		{"message", "Hermes server initialized"},
		JSON_TYPE_END}));
	return true;
}
