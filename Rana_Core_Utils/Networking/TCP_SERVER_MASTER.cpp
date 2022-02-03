#include "TCP_SERVER_MASTER.h"

bool Initialize_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server, CONTEXT *ctx,
	int port) {

	int o;
	sockaddr_in destination;
	long arg;

	server->ctx = ctx;
	Set_Name_TCP_SERVER_MASTER(server, "master");

	signal(SIGPIPE, SIG_IGN);

	server->lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->lSocket < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Server socket failed to open");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	o = 1;
	if (setsockopt(server->lSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	o = 1;
	if (setsockopt(server->lSocket, SOL_SOCKET, SO_REUSEPORT, (char*)&o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;

	if (bind(server->lSocket, (sockaddr*)&destination,
		sizeof(destination)) < 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Unable to bind socket on port");
			ADD_STR_LOG("name", server->name);
			ADD_INT_LOG("port", port);
		}
		return false;
	}

	if ((arg = fcntl(server->lSocket, F_GETFL, NULL)) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Error fcntl(..., F_GETFL)");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	arg |= O_NONBLOCK;
	if (fcntl(server->lSocket, F_SETFL, arg) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Error fcntl(..., F_SETFL)");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if (listen(server->lSocket, 50) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
		       ADD_STR_LOG("message", "listen() threw an error");
		       ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	return true;
}

void Set_Name_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server, int sec,
	int usec) {
	//this is an absurd situation...
	return true;
}

bool Set_High_Priority_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server) {
	return true;
}

void Delete_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server) {
	if (server->lSocket > 0) {
		shutdown(server->lSocket, 2);
		close(server->lSocket);

		server->lSocket = NULL;
	} else {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message",
				"Server connection has already been closed");
			ADD_STR_LOG("name", server->name);
		}
	}

	LOG_INFO_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Server connection closed");
		ADD_STR_LOG("name", server->name);
	}
}
