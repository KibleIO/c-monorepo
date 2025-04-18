#include "TCP_SERVER.h"

bool Initialize_TCP_SERVER(TCP_SERVER *server, KCONTEXT *ctx,
	TCP_SERVER_MASTER *tcp_master, int id) {

	server->ctx = ctx;
	server->tcp_master = tcp_master;
	server->cSocket = NULL;
	Set_Name_TCP_SERVER(server, "unknown");

	#ifndef _WIN64

	signal(SIGPIPE, SIG_IGN);

	#endif

	return true;
}

void Set_Name_TCP_SERVER(TCP_SERVER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_TCP_SERVER(TCP_SERVER *server, int sec, int usec) {
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;

	if (setsockopt(server->cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
		sizeof tv) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: timeout"},
			{"name", server->name},
		});
		return false;
	}
	return true;
}

bool Set_High_Priority_TCP_SERVER(TCP_SERVER *server) {
        #ifdef linux

	int32_t o;
	o = 6;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "bad setsockopt: priority"},
			{"name", server->name},
		});
		return false;
	}

        #endif

	return true;
}

bool Accept_TCP_SERVER(TCP_SERVER *server) {
	long arg;
	timeval tv;
	fd_set myset;
	socklen_t lon;
	int valopt;
	int32_t res;
	int err;
	int o;
	sockaddr_in cAddress;
	socklen_t cSize = sizeof(cAddress);

	tv.tv_sec = DEFAULT_ACCEPT_TIMEOUT;
	tv.tv_usec = 0;
	FD_ZERO(&myset);
	FD_SET(server->tcp_master->lSocket, &myset);
	if (select(server->tcp_master->lSocket + 1, &myset, NULL, NULL,
		&tv) > 0) {

		lon = sizeof(int);
		getsockopt(server->tcp_master->lSocket, SOL_SOCKET, SO_ERROR,
			(char*)(&valopt), (socklen_t*)&lon);

		if (valopt) {
			LOGGER_ERROR(server->ctx, {
				{"message", "Failed to accept: Error in listen()"},
				{"error_code", valopt},
				{"name", server->name},
			});
			return false;
		}

		if ((server->cSocket = accept(server->tcp_master->lSocket,
			(sockaddr*)&cAddress,
			(socklen_t*)&cSize)) < 0) {

			LOGGER_ERROR(server->ctx, {
				{"message", "Failed to accept: Error in accept()"},
				{"name", server->name},
			});
			return false;
		}
	} else {
		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: Timeout or Error select()"},
			{"name", server->name},
		});
		return false;
	}

	#ifndef _WIN64

	if ((arg = fcntl(server->cSocket, F_GETFL, NULL)) < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: Error fcntl(..., F_GETFL)"},
			{"name", server->name},
		});
		return false;
	}
	arg &= (~O_NONBLOCK);
	if (fcntl(server->cSocket, F_SETFL, arg) < 0) {
		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: Error fcntl(..., F_SETFL)"},
			{"name", server->name},
		});
		return false;
	}

	#else

	u_long mode = 0;  // 1 to enable non-blocking socket
	ioctlsocket(server->cSocket, FIONBIO, &mode);

	#endif

	o = 1;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#ifndef _WIN64

	o = 1;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_REUSEPORT, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: bad setsockopt: reuseaddr"},
			{"name", server->name},
		});
		return false;
	}

	#endif

	if (setsockopt(server->cSocket, IPPROTO_TCP, TCP_NODELAY, (const char*) &o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: bad setsockopt: nodelay"},
			{"name", server->name},
		});
		return false;
	}

	o = 70000000;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_SNDBUF, (const char*) &o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: bad setsockopt: sndbuf"},
			{"name", server->name},
		});
		return false;
	}

        #ifdef linux

	o = 1;
	if (setsockopt(server->cSocket, IPPROTO_TCP, TCP_QUICKACK, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(server->ctx, {
			{"message", "Failed to accept: bad setsockopt: quickack"},
			{"name", server->name},
		});
		return false;
	}

        #endif

	if (!Set_Recv_Timeout_TCP_SERVER(server, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	return true;
}

bool Send_TCP_SERVER(TCP_SERVER *server, char *buffer, int size) {
	return send(server->cSocket, buffer, size, MSG_WAITALL) == size;
}

bool Receive_TCP_SERVER(TCP_SERVER *server, char *buffer, int size) {
	return recv(server->cSocket, buffer, size, MSG_WAITALL) == size;
}

int Receive_Unsafe_TCP_SERVER(TCP_SERVER *server, char *buffer) {
	return recv(server->cSocket, buffer, ARBITRARILY_LARGE_PACKET, 0);
}

void Delete_TCP_SERVER(TCP_SERVER *server) {
	if (server->cSocket > 0) {
		shutdown(server->cSocket, 2);
		close(server->cSocket);

		server->cSocket = NULL;
		return;
	}
	LOGGER_WARN(server->ctx, {
		{"message", "Client connection has already been closed"},
		{"name", server->name},
	});
}
