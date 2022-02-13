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
		//this is only a warning because disconnecting something that is
		//already disconnected
		LOG_WARN_CTX((hs->ctx)) {
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

	Delete_SERVER_MASTER(&hs->tcp_master);
	Delete_SERVER_MASTER(&hs->udp_master);
}

bool Create_SERVER_CONNECTION(HERMES_SERVER *hs, HERMES_TYPE type) {
	HERMES_TYPE type_in;
	int ack;
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
		ack = -1;

		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "type mismatch");
		}
	} else {
		index = Get_Index_HERMES_SERVER(hs);

		if (index < 0) {
			ack = -1;

			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message",
					"max connections reached");
			}
		} else {
			ack = 1;
		}
	}

	if (!Send_SERVER(&hs->server, (char*)&ack, sizeof(int))) {
		LOG_ERROR_CTX((hs->ctx)) {
			ADD_STR_LOG("message", "failed to send ack");
		}
		return false;
	}

	if (ack < 0) {
		return false;
	}

	hs->cmutx.lock();

	hs->connections[index].type = type;
	hs->connections[index].active = false; //probably redundant

	Initialize_SERVER(&hs->connections[index].server, hs->ctx,
		((type.type == NETWORK_TYPE_TCP) ? &hs->tcp_master :
		&hs->udp_master));
	Set_Name_SERVER(&hs->connections[index].server, type.name);

	if (Accept_SERVER(&hs->connections[index].server)) {
		hs->connections[index].active = true;
	} else {
		Delete_SERVER(&hs->connections[index].server);
	}

	hs->cmutx.unlock();

	return hs->connections[index].active;
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
	while (hs->connected) {
		if (!Receive_SERVER(&hs->server, (char*)&flag,
			sizeof(uint8_t))) {

			LOG_ERROR_CTX((hs->ctx)) {
				ADD_STR_LOG("message",
					"failed to receive flag");
			}
			hs->shouldexit = true;
			break;
		}
		if (flag == HERMES_STATUS) {
			flag = false;
			if (!Send_SERVER(&hs->server, (char*)&flag,
				sizeof(uint8_t))) {

				LOG_ERROR_CTX((hs->ctx)) {
					ADD_STR_LOG("message",
						"failed to send status");
				}
				hs->shouldexit = true;
				break;
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
				hs->shouldexit = true;
				break;
			}
			hs->shouldexit = true;
		}
	}
}

bool Connect_HERMES_SERVER(HERMES_SERVER *hs, HERMES_TYPE *types) {
	if (hs->connected) {
		LOG_WARN_CTX((hs->ctx)) {
			ADD_STR_LOG("message",
				"hermes server already connected");
		}
		return false;
	}

	hs->shouldexit = false;

	if (hs->loop_thread != NULL) {
		hs->loop_thread->join();
		delete hs->loop_thread;
		hs->loop_thread = NULL;
	}

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].active = false;
	}

	Initialize_SERVER(&hs->server, hs->ctx, &hs->tcp_master);
	Set_Name_SERVER(&hs->server, "hermes server");

	if (!Accept_SERVER(&hs->server)) {
		Delete_SERVER(&hs->server);
		return false;
	}

	hs->connected = true;

	while ((*types).id != 0) {
		if (!Create_SERVER_CONNECTION(hs, *types)) {
			Disconnect_HERMES_SERVER(hs);
			return false;
		}
		types++;
	}

	hs->loop_thread = new thread(Loop_HERMES_SERVER, hs);

	return true;
}

bool Initialize_HERMES_SERVER(HERMES_SERVER *hs, CONTEXT *ctx, int port) {
	hs->connected = false;
	hs->shouldexit = false;
	hs->ctx = ctx;
	hs->loop_thread = NULL;
	hs->port = port;

	if (!Initialize_SERVER_MASTER(&hs->tcp_master, hs->ctx, NETWORK_TYPE_TCP,
		port)) {
		return false;
	}
	if (!Initialize_SERVER_MASTER(&hs->udp_master, hs->ctx, NETWORK_TYPE_UDP,
		port)) {
		return false;
	}

	return true;
}
