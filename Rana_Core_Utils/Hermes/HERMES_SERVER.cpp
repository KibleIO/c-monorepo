// PLATFORMS: Linux, Windows, OSX

#include "HERMES_SERVER.h"

void Close_Connections_HERMES_SERVER(HERMES_SERVER *hs) {
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (hs->connections[i].active) {
			Delete_SERVER(&hs->connections[i].server);
			hs->connections[i].active = false;
		}
	}
}

SERVER* Get_Blocking_HERMES_SERVER(HERMES_SERVER *hs, HERMES_TYPE type) {
	SERVER* server = NULL;
	while (!server && !hs->server_init_failed && hs->connected) {
		server = Get_HERMES_SERVER(hs, type);
	}
	return server;
}

SERVER* Get_HERMES_SERVER(HERMES_SERVER *hs, HERMES_TYPE type) {
	if (!hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "hermes server not connected");
		}
		return NULL;
	}
	hs->cmutx.lock();
	SERVER_CONNECTION* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (sc[i].active && sc[i].type.id == type.id) {
			SERVER* server = &sc[i].server;
			hs->cmutx.unlock();
			return server;
		}
	}
	hs->cmutx.unlock();
	return NULL;
}

int Get_Index_HERMES_SERVER(HERMES_SERVER* hs) {
	if (!hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "hermes server not connected");
		}
		return -1;
	}
	hs->cmutx.lock();
	SERVER_CONNECTION* sc = hs->connections;
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (!sc[i].active) {
			hs->cmutx.unlock();
			return i;
		}
	}
	hs->cmutx.unlock();
	return -1;
}

void Delete_HERMES_SERVER(HERMES_SERVER* hs) {
	Delete_SERVER(&hs->server);
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		Delete_SERVER(&hs->connections[i].server);
	}
}

void Epipe_HERMES_SERVER(HERMES_SERVER* hs) {
	hs->err = EPIPE;
	hs->connected = false;
}

bool Connect_HERMES_SERVER(HERMES_SERVER* hs, int port, int baseport) {
	if (hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message",
				"hermes server already connected");
		}
		return false;
	}

	hs->connected = true;

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].port = baseport++;
	}

	Initialize_SERVER(&hs->server, hs->ctx, NETWORK_TYPE_TCP);
	Set_Name_SERVER(&hs->server, "hermes server");
	Set_Recv_Timeout_SERVER(&hs->server, 0, 100);

	if (!Accept_SERVER(&hs->server, port)) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "could not listen on port");
			ADD_INT_LOG("port", port);
		}
		hs->server_init_failed = true;
		hs->connected = false;
		return false;
	}
	return true;
}

void Loop_HERMES_SERVER(HERMES_SERVER* hs) {
	uint8_t flag;
	HERMES_TYPE type;

	if (!hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message",
				"hermes server not connected");
		}
		return;
	}

	LOG_INFO_CTX((hs->ctx)) {
		ADD_STR_LOG("message", "starting hermes server loop");
	}
	while (hs->connected) {
		// FIX!!
		int attempts = HERMES_TIMEOUT_TRIES;
		while (!Receive_SERVER(&hs->server, (char*)&flag,
			sizeof(uint8_t)) && attempts-- >= 0 && hs->connected) {
			Sleep_Milli(1);
		}
		if (attempts < 0 || !hs->connected) {
			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message",
					"failed to receive flag");
			}
			hs->server_init_failed = true;
			hs->err = EPIPE;

			break;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (hs->shouldexit) {
				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"sending quit packet");
				}
			}
			if (!Send_SERVER(&hs->server, (char*)&flag,
				sizeof(uint8_t))) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"failed to send status");
				}
				hs->shouldexit = false;
				hs->err = EPIPE;
				break;
			}
			if (hs->shouldexit) {
				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"quit packet sent");
				}
			}
		} else if (flag == HERMES_EXIT) {
			LOG_INFO_CTX((hs->ctx)) {
				ADD_STR_LOG("message", "Exiting");
			}
			if (!Send_SERVER(&hs->server, (char*)&flag,
				sizeof(uint8_t))) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message", "failed to "
						"send exit confirmation");
				}
				hs->err = EPIPE;
				break;
			}
			hs->shouldexit = false;
			hs->connected = false;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!Receive_SERVER(&hs->server, (char*)&type,
				sizeof(HERMES_TYPE))) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message", "failed to "
						"receive connection type");
				}
				hs->server_init_failed = true;
				hs->err = EPIPE;
				break;
			}

			int index = Get_Index_HERMES_SERVER(hs);
			if (index < 0) {
				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message", "max "
						"connections reached");
				}
				hs->err = EPIPE;
				break;
			}

			if (!Send_SERVER(&hs->server,
				(char*)&hs->connections[index].port,
				sizeof(int))) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"failed to send port");
				}
				hs->err = EPIPE;
				break;
			}

			hs->cmutx.lock();

			Initialize_SERVER(&hs->connections[index].server,
				hs->ctx, type.type);
			Set_Name_SERVER(&hs->server, type.name);
			hs->connections[index].type = type;

			if (!Accept_SERVER(&hs->connections[index].server,
				hs->connections[index].port)) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"could not listen on port");
					ADD_INT_LOG("port",
						hs->connections[index].port);
					ADD_STR_LOG("type",
						hs->connections[index].type.
						name);
				}
				Delete_SERVER(&hs->connections[index].server);
				hs->connections[index].active = false;
			} else {
				LOG_INFO_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"server bound!");
					ADD_INT_LOG("port",
						hs->connections[index].port);
					ADD_STR_LOG("type",
						hs->connections[index].type.
						name);
				}
				hs->connections[index].active = true;
			}
			hs->cmutx.unlock();
		}
	}
	LOG_INFO_CTX((hs->ctx)) {
		ADD_STR_LOG("message", "ending server loop");
	}
	if (hs->shouldexit) {
		hs->shouldexit = false;
	}
	hs->connected = false;
	Close_Connections_HERMES_SERVER(hs);
}

bool Initialize_HERMES_SERVER(HERMES_SERVER *hs, CONTEXT *ctx) {
	hs->err = 0;
	hs->connected = false;
	hs->shouldexit = false;
	hs->server_init_failed = false;
	hs->ctx = ctx;
	return true;
}
