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
		log_err(((const JSON_TYPE){{"message", "hermes server not connected"},
								   JSON_TYPE_END}));
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

void Connect_HERMES_SERVER(HERMES_SERVER* hs, int port, int baseport) {
	if (hs->connected) {
		log_err(((const JSON_TYPE){
			{"message", "hermes server already connected"}, JSON_TYPE_END}));
		return;
	}

	hs->connected = true;

	for (int i = 0; i < HERMES_CONNECTIONS_MAX; i++) {
		hs->connections[i].port = baseport++;
	}

	Initialize_SERVER(&hs->server, hs->ctx, NETWORK_TYPE_TCP);
	Set_Name_SERVER(&hs->server, "hermes server");
	Set_Recv_Timeout_SERVER(&hs->server, 0, 100);

	log_dbg(((const JSON_TYPE){{"message", "Listening on"}, JSON_TYPE_END}));
	if (!Accept_SERVER(&hs->server, port)) {
		log_err(((const JSON_TYPE){{"message", "could not listen on port"},
								   JSON_TYPE_END}));
		hs->server_init_failed = true;
		hs->connected = false;
		return;
	}
	log_dbg(((const JSON_TYPE){{"message", "bound"}, JSON_TYPE_END}));

	uint8_t flag;
	HERMES_TYPE type;

	log_info({{"message", "starting hermes server loop"}, JSON_TYPE_END});
	while (hs->connected) {
		// FIX!!
		int attempts = HERMES_TIMEOUT_TRIES;
		while (!Receive_SERVER(&hs->server, (char*)&flag, sizeof(uint8_t)) &&
			   attempts-- >= 0 && hs->connected) {
			Sleep_Milli(1);
		}
		if (attempts < 0 || !hs->connected) {
			log_err(((const JSON_TYPE){{"message", "failed to receive flag"},
									   JSON_TYPE_END}));
			hs->server_init_failed = true;
			hs->err = EPIPE;

			break;
		}
		if (flag == HERMES_STATUS) {
			flag = hs->shouldexit;
			if (hs->shouldexit) {
				log_err(((const JSON_TYPE){{"message", "sending quit packet"},
										   JSON_TYPE_END}));
			}
			if (!Send_SERVER(&hs->server, (char*)&flag, sizeof(uint8_t))) {
				log_err(((const JSON_TYPE){{"message", "failed to send status"},
										   JSON_TYPE_END}));
				hs->shouldexit = false;
				hs->err = EPIPE;
				break;
			}
			if (hs->shouldexit) {
				log_err(((const JSON_TYPE){{"message", "quit packet sent"},
										   JSON_TYPE_END}));
			}
		} else if (flag == HERMES_EXIT) {
			log_dbg(((const JSON_TYPE){{"message", "Exiting"}, JSON_TYPE_END}));
			if (!Send_SERVER(&hs->server, (char*)&flag, sizeof(uint8_t))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to send exit confirmation"},
					JSON_TYPE_END}));
				hs->err = EPIPE;
				break;
			}
			hs->shouldexit = false;
			hs->connected = false;
		} else if (flag == HERMES_GET_CONNECTION) {
			if (!Receive_SERVER(&hs->server, (char*)&type, sizeof(HERMES_TYPE))) {
				log_err(((const JSON_TYPE){
					{"message", "failed to receive connection type"},
					JSON_TYPE_END}));
				hs->server_init_failed = true;
				hs->err = EPIPE;
				break;
			}

			int index = Get_Index_HERMES_SERVER(hs);
			if (index < 0) {
				log_err(((const JSON_TYPE){
					{"message", "max connections reached"}, JSON_TYPE_END}));
				hs->err = EPIPE;
				break;
			}

			if (!Send_SERVER(&hs->server, (char*)&hs->connections[index].port, sizeof(int))) {
				log_err(((const JSON_TYPE){{"message", "failed to send port"},
										   JSON_TYPE_END}));
				hs->err = EPIPE;
				break;
			}

			hs->cmutx.lock();

			Initialize_SERVER(&hs->connections[index].server, hs->ctx, type.type);
			Set_Name_SERVER(&hs->server, type.name);
			hs->connections[index].type = type;

			log_dbg(((const JSON_TYPE){{"message", "server listening on"},
									   JSON_TYPE_END}));
			if (!Accept_SERVER(&hs->connections[index].server, hs->connections[index].port)) {
				log_err(((const JSON_TYPE){
					{"message", "could not listen on port"}, JSON_TYPE_END}));
				Delete_SERVER(&hs->connections[index].server);
				hs->connections[index].active = false;
			} else {
				log_dbg(((const JSON_TYPE){{"message", "server bound on"},
										   JSON_TYPE_END}));
				hs->connections[index].active = true;
			}
			hs->cmutx.unlock();
		}
	}
	log_err(
		((const JSON_TYPE){{"message", "ending server loop"}, JSON_TYPE_END}));
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
	log_dbg(((const JSON_TYPE){{"message", "Hermes server initialized"},
							   JSON_TYPE_END}));
	return true;
}
