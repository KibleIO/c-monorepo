// PLATFORMS: Linux, Windows, OSX

#include "HERMES_CLIENT.h"

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
	int ack;

	if (!Send_CLIENT(&hc->client, (char*)&type, sizeof(HERMES_TYPE))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send type");
		}
		return false;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&ack, sizeof(int))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive port");
		}
		return false;
	}

	if (ack < 0) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "Server side error");
		}
		return false;
	}

	//umm... I don't think this is thread safe
	int index = Get_Index_HERMES_CLIENT(hc);
	if (index < 0) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "max connections reached");
		}
	}

        if (hc->use_tcp) {
                type.type = NETWORK_TYPE_TCP;
        }

	hc->cmutx.lock();

        connect:

	int attempts = HERMES_TIMEOUT_TRIES;

	while (--attempts >= 0 && hc->connected) {
		Start_FPS_LIMITER(&hc->fps_limiter);

		Initialize_CLIENT(&hc->connections[index].client, hc->ctx,
			((type.type == NETWORK_TYPE_TCP) ? &hc->tcp_master :
			&hc->udp_master), type.id);
		Set_Name_CLIENT(&hc->connections[index].client, (char*) type.name);

		if (Connect_CLIENT(&hc->connections[index].client)) {

			hc->connections[index].type = type;
			hc->connections[index].active = true;

			break;
		}

		Delete_CLIENT(&hc->connections[index].client);

		Stop_FPS_LIMITER(&hc->fps_limiter);
	}

	hc->cmutx.unlock();
        /*
        if (hc->connected && attempts < 0 && type.type == NETWORK_TYPE_UDP) {
                type.type = NETWORK_TYPE_TCP;
                hc->use_tcp = true;

                LOG_WARN_CTX((hc->ctx)) {
                        ADD_STR_LOG("message", "UDP failed... reverting to TCP. Performance will be degraded");
                }

                goto connect;
        }
        */

	return (attempts >= 0 && hc->connected);
}

bool Connect_HERMES_CLIENT(HERMES_CLIENT* hc, HERMES_TYPE *types) {
	if (hc->connected) {
		LOG_WARN_CTX((hc->ctx)) {
			ADD_STR_LOG("message",
				"hermes client already connected");
		}
		return false;
	}

	int attempts = HERMES_TIMEOUT_TRIES;

	if (!Initialize_CLIENT_MASTER(&hc->tcp_master, hc->ctx, NETWORK_TYPE_TCP,
		hc->port, hc->ip)) {
		return false;
	}
	/*
	if (!Initialize_CLIENT_MASTER(&hc->udp_master, hc->ctx, NETWORK_TYPE_UDP,
		hc->port, hc->ip)) {
		return false;
	}
	*/

	while (--attempts >= 0) {
		Start_FPS_LIMITER(&hc->fps_limiter);

		Initialize_CLIENT(&hc->client, hc->ctx, &hc->tcp_master,
			HERMES_CLIENT_T.id);
		Set_Name_CLIENT(&hc->client, "hermes client");
		Set_Recv_Timeout_CLIENT(&hc->client, HERMES_CORE_TIMEOUT, 0);

		if (Connect_CLIENT(&hc->client)) {
			break;
		}

		Delete_CLIENT(&hc->client);

		Stop_FPS_LIMITER(&hc->fps_limiter);
	}

	if (attempts < 0) {
		return false;
	}

	hc->connected = true;

	while ((*types).id != 0) {
		if (!Create_CLIENT_CONNECTION(hc, *types)) {
			Disconnect_HERMES_CLIENT(hc);
			return false;
		}
		types++;
	}

	return true;
}

void Disconnect_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		//this is only a warning because disconnecting something that is
		//already disconnected
		LOG_WARN_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
		return;
	}

	uint8_t flag = HERMES_EXIT;

	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_WARN_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send exit flag");
		}
		goto cleanup;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_WARN_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive exit flag");
		}
		goto cleanup;
	}
	if (flag != HERMES_EXIT) {
		LOG_WARN_CTX((hc->ctx)) {
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

	Delete_CLIENT_MASTER(&hc->tcp_master);
	//Delete_CLIENT_MASTER(&hc->udp_master);

	hc->connected = false;
        hc->use_tcp = false;
}

uint8_t Status_HERMES_CLIENT(HERMES_CLIENT* hc) {
	if (!hc->connected) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "hermes client not connected");
		}
		return HERMES_STATUS_DISCONNECTED;
	}

	uint8_t flag = HERMES_STATUS;
	if (!Send_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not send status flag");
		}
		Disconnect_HERMES_CLIENT(hc);
		return HERMES_STATUS_UNEXPECTED_DISCONNECT;
	}
	if (!Receive_CLIENT(&hc->client, (char*)&flag, sizeof(uint8_t))) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "could not receive status flag");
		}
		Disconnect_HERMES_CLIENT(hc);
		return HERMES_STATUS_UNEXPECTED_DISCONNECT;
	}
	if (flag != HERMES_STATUS_NORMAL) { // shouldexit
		Disconnect_HERMES_CLIENT(hc);
		return HERMES_STATUS_EXPECTED_DISCONNECT;
	}
	return HERMES_STATUS_NORMAL;
}

bool Initialize_HERMES_CLIENT(HERMES_CLIENT* hc, CONTEXT *ctx,
	char *ip, int port) {

	hc->connected = false;
	hc->ctx = ctx;
        hc->use_tcp = false;
	hc->port = port;
	strcpy(hc->ip, ip);

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hc->connections[i].active = false;
	}

	if (!Initialize_FPS_LIMITER(&hc->fps_limiter, 10, false)) {
		LOG_ERROR_CTX((hc->ctx)) {
			ADD_STR_LOG("message", "failed to init fps limiter");
		}
		return false;
	}

	return true;
}
