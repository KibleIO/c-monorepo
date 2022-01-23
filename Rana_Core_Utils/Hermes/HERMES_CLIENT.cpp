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
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
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
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
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
	Disconnect_HERMES_CLIENT(hc);
}

bool Create_CLIENT_CONNECTION(HERMES_CLIENT* hc, HERMES_TYPE type) {
	if (!hc->connected) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
		return false;
	}
	int port;
	uint8_t flag = HERMES_GET_CONNECTION;

	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send flag");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	if (!Send_CLIENT(&hc->client, (char*)&type, sizeof(HERMES_TYPE))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send type");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&port, sizeof(int))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive port");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}

	//umm... I don't think this is thread safe
	int index = Get_Index_HERMES_CLIENT(hc);
	if (index < 0) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "max connections reached");
		}
	}

	hc->cmutx.lock();

	int attempts = HERMES_TIMEOUT_TRIES;

	while (attempts-- >= 0) {
		Initialize_CLIENT(&hc->connections[index].client, hc->ctx, type.type);
		Set_Name_CLIENT(&hc->connections[index].client, type.name);

		if (Connect_CLIENT(&hc->connections[index].client, port, hc->ip)) {
			hc->connections[index].type = type;
			hc->connections[index].active = true;

			break;
		}

		Delete_CLIENT(&hc->connections[index].client);
	}

	hc->cmutx.unlock();

	if (attempts < 0) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "failed to connect");
			ADD_STR_LOG("name", type.name);
		}
		return false;
	}
	LOG_INFO_CTX((hc->ctx)) {
		ADD_STR_LOG("message", "Connected!");
		ADD_STR_LOG("name", type.name);
	}

	return true;
}

bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, char *ip, int port,
	HERMES_TYPE *types) {

	if (hc->connected) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message",
				"hermes client already connected");
		}
		return false;
	}

	// come on man - Joe Biden
	int attempts = HERMES_TIMEOUT_TRIES / 10;
	hc->baseport = port;
	strcpy(hc->ip, ip);

	while (attempts-- >= 0) {
		Initialize_CLIENT(&hc->client, hc->ctx, NETWORK_TYPE_TCP);
		Set_Name_CLIENT(&hc->client, "hermes client");
		Set_Recv_Timeout_CLIENT(&hc->client, 1, 0);

		if (Connect_CLIENT(&hc->client, hc->baseport, hc->ip)) {
			break;
		}

		Delete_CLIENT(&hc->client);
	}

	if (attempts < 0) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "failed to connect");
			ADD_STR_LOG("name", "hermes client");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}

	LOG_INFO_CTX((hc->ctx)) {
		ADD_STR_LOG("message", "Connected!");
		ADD_STR_LOG("name", "hermes client");
	}

	hc->connected = true;

	while ((*types).id != 0) {
		if (!Create_CLIENT_CONNECTION(hc, *types)) {
			LOG_ERROR_CTX((hc->ctx)) {
				ADD_STR_LOG("message", "failed to connect");
				ADD_STR_LOG("name", (*types).name);
			}

			return false;
		}
		types++;
	}
	LOG_INFO_CTX((hc->ctx)) {
		ADD_STR_LOG("message", "Hermes connected!");
	}

	return true;
}

void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
		return;
	}

	uint8_t flag = HERMES_EXIT;

	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send exit flag");
		}
		goto cleanup;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive exit flag");
		}
		goto cleanup;
	}
	if (flag != HERMES_EXIT) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "flag received not exit");
			ADD_INT_LOG("flag", flag);
		}

		goto cleanup;
	}

	cleanup:

	Delete_CLIENT(&hc->client);
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (hc->connections[i].active) {
			Delete_CLIENT(&hc->connections[i].client);
			hc->connections[i].active = false;
		}
	}
	hc->connected = false;
}

bool Status_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
		return false;
	}

	uint8_t flag = HERMES_STATUS;
	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send status flag");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive status flag");
		}
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	if (flag) {	 // shouldexit
		Disconnect_HERMES_CLIENT(hc);
		return false;
	}
	return true;
}

bool Initialize_HERMES_CLIENT(HERMES_CLIENT* hc, CONTEXT *ctx) {
	hc->connected = false;
	hc->ctx = ctx;

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hc->connections[i].active = false;
	}

	return true;
}
