#include "TCP_SERVER_MASTER.h"

bool Initialize_TCP_SERVER_MASTER(TCP_SERVER_MASTER *server, KCONTEXT *ctx,
	int port) {

	int o;
	sockaddr_in destination;
	long arg;

	server->ctx = ctx;
	Set_Name_TCP_SERVER_MASTER(server, "master");

	#ifndef _WIN64

	signal(SIGPIPE, SIG_IGN);

	#endif

        #ifdef linux

	server->lSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (server->lSocket < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Server socket failed to open"},
			{"name", server->name},
		});
		return false;
	}

        #else

        server->lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->lSocket < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Server socket failed to open"},
			{"name", server->name},
		});
		return false;
	}

        #endif

	o = 1;
	if (setsockopt(server->lSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#ifndef _WIN64

	o = 1;
	if (setsockopt(server->lSocket, SOL_SOCKET, SO_REUSEPORT, (char*)&o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#endif

	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;

	if (::bind(server->lSocket, (sockaddr*)&destination,
		sizeof(destination)) < 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Unable to bind socket on port"},
			{"name", server->name},
			{"port", port},
		});
		return false;
	}

	#ifndef _WIN64

	if ((arg = fcntl(server->lSocket, F_GETFL, NULL)) < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Error fcntl(..., F_GETFL)"},
			{"name", server->name},
		});
		return false;
	}

	arg |= O_NONBLOCK;
	if (fcntl(server->lSocket, F_SETFL, arg) < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Error fcntl(..., F_SETFL)"},
			{"name", server->name},
		});
		return false;
	}

	#else

	u_long mode = 1;  // 1 to enable non-blocking socket
	ioctlsocket(server->lSocket, FIONBIO, &mode);

	#endif

	if (listen(server->lSocket, 50) < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "listen() threw an error"},
			{"name", server->name},
		});
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
		return;
	}
	LOGGER_WARN(server->ctx, {
		{"message", "Server connection has already been closed"},
		{"name", server->name},
	});
}
