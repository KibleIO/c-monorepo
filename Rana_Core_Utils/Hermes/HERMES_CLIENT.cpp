// PLATFORMS: Linux, Windows, OSX

#include "HERMES_CLIENT.h"

CLIENT* Get_Blocking_HERMES_CLIENT(HERMES_CLIENT* hc, HERMES_TYPE type) {
	CLIENT* client = NULL;
	while (!client && hc->connected) {
		client = Get_HERMES_CLIENT(hc, type);
	}
	return client;
}

CLIENT* Get_HERMES_CLIENT(HERMES_CLIENT* hc, HERMES_TYPE type) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){{"message", "hermes server not connected"},
								   JSON_TYPE_END}));
		return NULL;
	}
	hc->cmutx.lock();
	CLIENT_CONNECTION* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (cc[i].active && cc[i].type.id == type.id) {
			CLIENT* client = &cc[i].client;
			hc->cmutx.unlock();
			return client;
		}
	}
	hc->cmutx.unlock();
	return NULL;
}

int Get_Index_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){{"message", "hermes client not connected"},
								   JSON_TYPE_END}));
		return -1;
	}
	hc->cmutx.lock();
	CLIENT_CONNECTION* cc = hc->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!cc[i].active) {
			hc->cmutx.unlock();
			return i;
		}
	}
	hc->cmutx.unlock();
	return -1;
}

void Delete_HERMES_CLIENT(HERMES_CLIENT* hc) {
	Delete_CLIENT(&hc->client);
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (hc->connections[i].active) {
			Delete_CLIENT(&hc->connections[i].client);
			hc->connections[i].active = false;
		}
	}
}

void Epipe_HERMES_CLIENT(HERMES_CLIENT* hc) {
	hc->err = EPIPE;
	hc->connected = false;
}

bool Create_CLIENT_CONNECTION(HERMES_CLIENT* hc, HERMES_TYPE type) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){{"message", "hermes client not connected"},
								   JSON_TYPE_END}));
		return false;
	}
	uint8_t flag = HERMES_GET_CONNECTION;
	log_dbg(((const JSON_TYPE){{"message", "sending flag"}, JSON_TYPE_END}));
	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){{"message", "could not send flag"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}

	log_dbg(((const JSON_TYPE){{"message", "sending type"}, JSON_TYPE_END}));
	if (!Send_CLIENT(&hc->client, (char*)&type, sizeof(HERMES_TYPE))) {
		log_err(((const JSON_TYPE){{"message", "could not send type"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	int port;
	log_dbg(((const JSON_TYPE){{"message", "receiving port"}, JSON_TYPE_END}));
	if (!Receive_CLIENT(&hc->client, (char*)&port, sizeof(int))) {
		log_err(((const JSON_TYPE){{"message", "could not receive port"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}

	//umm... I don't think this is thread safe
	int index = Get_Index_HERMES_CLIENT(hc);
	if (index < 0) {
		log_err(((const JSON_TYPE){{"message", "max connections reached"},
								   JSON_TYPE_END}));
	}

	hc->cmutx.lock();
	Initialize_CLIENT(&hc->connections[index].client, hc->ctx, type.type);
	Set_Name_CLIENT(&hc->connections[index].client, type.name);
	hc->connections[index].type = type;
	hc->connections[index].active = true;

	log_dbg(((const JSON_TYPE){{"message", "connecting"}, JSON_TYPE_END}));
	int attempts = HERMES_TIMEOUT_TRIES;

	while (!Connect_CLIENT(&hc->connections[index].client, port, hc->ip) &&
		attempts-- >= 0);

	hc->cmutx.unlock();

	if (attempts < 0) {
		log_err(((const JSON_TYPE){{"message", "failed to connect to"},
								   JSON_TYPE_END}));
		return false;
	}

	log_dbg(((const JSON_TYPE){{"message", "connected"}, JSON_TYPE_END}));

	return true;
}

bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, char *ip, int port,
	HERMES_TYPE *types) {

	if (hc->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes client already connected"}, JSON_TYPE_END}));
		return false;
	}

	Initialize_CLIENT(&hc->client, hc->ctx, NETWORK_TYPE_TCP);
	Set_Name_CLIENT(&hc->client, "hermes client");
	Set_Recv_Timeout_CLIENT(&hc->client, 1, 0);
	hc->baseport = port;
	strcpy(hc->ip, ip);

	log_dbg(((const JSON_TYPE){{"message", "connecting to"}, JSON_TYPE_END}));

	// come on man - Joe Biden
	int attempts = HERMES_TIMEOUT_TRIES / 10;
	while (!Connect_CLIENT(&hc->client, hc->baseport, hc->ip) &&
		attempts-- >= 0);

	if (attempts < 0) {
		log_err(((const JSON_TYPE){{"message", "failed to connect to"},
								   JSON_TYPE_END}));

		return false;
	}

	log_dbg(((const JSON_TYPE){{"message", "connected to"}, JSON_TYPE_END}));

	hc->connected = true;

	log_dbg((
		(const JSON_TYPE){{"message", "creating connections"}, JSON_TYPE_END}));
	while ((*types).id != 0) {
		log_dbg(((const JSON_TYPE){{"message", "to_string(*types)"},
								   JSON_TYPE_END}));

		if (!Create_CLIENT_CONNECTION(hc, *types)) {
			log_err(((const JSON_TYPE){{"message", "failed to connect to"},
									   JSON_TYPE_END}));

			return false;
		}
		types++;
	}
	log_dbg(
		((const JSON_TYPE){{"message", "hermes connected"}, JSON_TYPE_END}));

	return true;
}

void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){{"message", "hermes client not connected"},
								   JSON_TYPE_END}));
		return;
	}

	uint8_t flag = HERMES_EXIT;

	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){{"message", "could not send exit flag"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){{"message", "could not receive exit flag"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return;
	}
	if (flag != HERMES_EXIT) {
		log_err(((const JSON_TYPE){{"message", "flag received not exit"},
								   JSON_TYPE_END}));
		hc->err = EIO;
	}
}

bool Status_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		log_err(((const JSON_TYPE){{"message", "hermes client not connected"},
								   JSON_TYPE_END}));
		return false;
	}

	uint8_t flag = HERMES_STATUS;
	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){{"message", "could not send status flag"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		log_err(((const JSON_TYPE){{"message", "could not receive status flag"},
								   JSON_TYPE_END}));
		hc->connected = false;
		hc->err = EPIPE;
		return false;
	}
	if (flag) {	 // shouldexit
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	return true;
}

bool Initialize_HERMES_CLIENT(HERMES_CLIENT* hc, CONTEXT *ctx) {
	hc->err = 0;
	hc->connected = false;
	hc->ctx = ctx;
	log_dbg(((const JSON_TYPE){{"message", "Hermes client initialized"},
							   JSON_TYPE_END}));
	return true;
}
