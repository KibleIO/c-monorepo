// PLATFORMS: Linux, Windows, OSX

#include "HERMES_SERVER.h"

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

void Disconnect_HERMES_SERVER(HERMES_SERVER* hs) {
	if (!hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "hermes server not connected");
		}
		return;
	}

	Delete_SERVER(&hs->server);
	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		if (hs->connections[i].active) {
			Delete_SERVER(&hs->connections[i].server);
			hs->connections[i].active = false;
		}
	}
	hs->connected = false;

	if (hs->loop_thread != NULL) {
		hs->loop_thread->join();
		delete hs->loop_thread;
		hs->loop_thread = NULL;
	}
}

void Delete_HERMES_SERVER(HERMES_SERVER* hs) {
	Disconnect_HERMES_SERVER(hs);
}

bool Create_SERVER_CONNECTION(HERMES_SERVER *hs, HERMES_TYPE type) {
	HERMES_TYPE type_in;
	int port;
	int index;

	if (!Receive_SERVER(&hs->server, (char*)&type_in,
		sizeof(HERMES_TYPE))) {

		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message",
				"failed to receive connection type");
		}
		return false;
	}

	if (type_in.id != type.id) {
		port = -1;

		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "type mismatch");
		}
	} else {
		index = Get_Index_HERMES_SERVER(hs);

		if (index < 0) {
			port = -1;

			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message",
					"max connections reached");
			}
		} else {
			port = hs->connections[index].port;
		}
	}

	if (!Send_SERVER(&hs->server, (char*)&port, sizeof(int))) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "failed to send port");
		}
		return false;
	}

	if (port < 0) {
		return false;
	}

	hs->cmutx.lock();

	//okay so why are we dividing HERMES_TIMEOUT_TRIES by 10? The answer is
	//a bit complex, but basically the timeout code for accept is a little
	//odd and if we allow it to retry a ton of times then the rate at which
	//it succeeds to connect is considerably low.
	int attempts = HERMES_TIMEOUT_TRIES / 10;
	hs->connections[index].type = type;
	hs->connections[index].active = false; //probably redundant

	while (--attempts >= 0) {
		Start_FPS_LIMITER(&hs->fps_limiter);

		Initialize_SERVER(&hs->connections[index].server, hs->ctx,
			type.type);
		Set_Name_SERVER(&hs->connections[index].server, type.name);

		if (Accept_SERVER(&hs->connections[index].server,
			hs->connections[index].port)) {

			hs->connections[index].active = true;
			break;
		}

		Delete_SERVER(&hs->connections[index].server);

		Stop_FPS_LIMITER(&hs->fps_limiter);
	}

	hs->cmutx.unlock();

	return (attempts >= 0);
}

void Loop_HERMES_SERVER(HERMES_SERVER* hs) {
	uint8_t flag;

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
		if (!Receive_SERVER(&hs->server, (char*)&flag,
			sizeof(uint8_t))) {

			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message",
					"failed to receive flag");
			}
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
				break;
			}
			hs->shouldexit = false;
			Disconnect_HERMES_SERVER(hs);
		}
	}
	LOG_INFO_CTX((hs->ctx)) {
		ADD_STR_LOG("message", "ending server loop");
	}
	if (hs->shouldexit) {
		hs->shouldexit = false;
	}
	Disconnect_HERMES_SERVER(hs);
}

bool Connect_HERMES_SERVER(HERMES_SERVER *hs, int port, int baseport,
	HERMES_TYPE *types) {

	if (hs->connected) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message",
				"hermes server already connected");
		}
		return false;
	}

	if (hs->loop_thread != NULL) {
		hs->loop_thread->join();
		delete hs->loop_thread;
		hs->loop_thread = NULL;
	}

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].active = false;
		hs->connections[i].port = baseport++;
	}

	int attempts = HERMES_TIMEOUT_TRIES / 10;

	while (--attempts >= 0) {
		Start_FPS_LIMITER(&hs->fps_limiter);

		Initialize_SERVER(&hs->server, hs->ctx, NETWORK_TYPE_TCP);
		Set_Name_SERVER(&hs->server, "hermes server");

		if (Accept_SERVER(&hs->server, port)) {
			break;
		}

		Delete_SERVER(&hs->server);

		Stop_FPS_LIMITER(&hs->fps_limiter);
	}

	if (attempts < 0) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "could not listen on port");
			ADD_INT_LOG("port", port);
		}
		return false;
	}

	LOG_INFO_CTX((hs->ctx)) {
		ADD_STR_LOG("message", "Connected!");
		ADD_STR_LOG("name", "hermes server");
	}

	hs->connected = true;

	while ((*types).id != 0) {
		if (!Create_SERVER_CONNECTION(hs, *types)) {
			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message", "failed to connect");
				ADD_STR_LOG("name", (*types).name);
			}

			Disconnect_HERMES_SERVER(hs);
			return false;
		}
		types++;
	}
	LOG_INFO_CTX((hs->ctx)) {
		ADD_STR_LOG("message", "Hermes connected!");
	}

	hs->loop_thread = new thread(Loop_HERMES_SERVER, hs);

	return true;
}

bool Initialize_HERMES_SERVER(HERMES_SERVER *hs, CONTEXT *ctx) {
	hs->connected = false;
	hs->shouldexit = false;
	hs->ctx = ctx;
	hs->loop_thread = NULL;

	if (!Initialize_FPS_LIMITER(&hs->fps_limiter, 10, false)) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "failed to init fps limiter");
		}
		return false;
	}

	return true;
}
