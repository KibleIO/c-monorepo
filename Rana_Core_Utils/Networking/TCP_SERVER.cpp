#include "TCP_SERVER.h"

bool Initialize_TCP_SERVER(TCP_SERVER *server, CONTEXT *ctx) {
	int o;

	server->ctx = ctx;
	server->cSocket = NULL;
	Set_Name_TCP_SERVER(server, "unknown");

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

	return true;
}

void Set_Name_TCP_SERVER(TCP_SERVER *server, char *name) {
	strcpy(server->name, name);
}

bool Set_Recv_Timeout_TCP_SERVER(TCP_SERVER *server, int sec, int usec) {
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;

	if (setsockopt(server->cSocket, SOL_SOCKET, SO_RCVTIMEO, &tv,
		sizeof tv) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: timeout");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}
	return true;
}

bool Set_High_Priority_TCP_SERVER(TCP_SERVER *server) {
	int32_t o;
	o = 6;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: priority");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}
	return true;
}

bool Accept_TCP_SERVER(TCP_SERVER *server, int port) {
	long arg;
	timeval tv;
	fd_set myset;
	socklen_t lon;
	int valopt;
	int32_t res;
	int err;
	int o;
	sockaddr_in destination;
	sockaddr_in cAddress;
	socklen_t cSize = sizeof(cAddress);

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

	LOG_INFO_CTX((server->ctx)) {
		ADD_STR_LOG("message", "bound on port");
		ADD_STR_LOG("name", server->name);
		ADD_INT_LOG("port", port);
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

	if (listen(server->lSocket, 50) >= 0) {
		tv.tv_sec = DEFAULT_CONNECT_TIMEOUT / 100000; //so ugly
		tv.tv_usec = 0;
		FD_ZERO(&myset);
		FD_SET(server->lSocket, &myset);
		if (select(server->lSocket + 1, &myset, NULL, NULL, &tv) > 0) {
			lon = sizeof(int);
			getsockopt(server->lSocket, SOL_SOCKET, SO_ERROR,
				(void*)(&valopt), (unsigned int*)&lon);

			if (valopt) {
				LOG_ERROR_CTX((server->ctx)) {
					ADD_STR_LOG("message",
						"Error in listen()");

					ADD_STR_LOG("name", server->name);
					ADD_INT_LOG("port", port);
					ADD_INT_LOG("error_code", valopt);
				}
				return false;
			}

			if ((server->cSocket = accept(server->lSocket,
				(sockaddr*)&cAddress,
				(unsigned int*)&cSize)) < 0) {

				LOG_ERROR_CTX((server->ctx)) {
					ADD_STR_LOG("message",
						"Error in accept()");

					ADD_STR_LOG("name", server->name);
					ADD_INT_LOG("port", port);
				}
				return false;
			}
		} else {
			LOG_ERROR_CTX((server->ctx)) {
				ADD_STR_LOG("message",
					"Timeout or Error select()");

				ADD_STR_LOG("name", server->name);
			}
			return false;
		}
 	} else {
		LOG_ERROR_CTX((server->ctx)) {
		       ADD_STR_LOG("message", "listen() threw an error");
		       ADD_STR_LOG("name", server->name);
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
	arg &= (~O_NONBLOCK);
	if (fcntl(server->lSocket, F_SETFL, arg) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Error fcntl(..., F_SETFL)");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if ((arg = fcntl(server->cSocket, F_GETFL, NULL)) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Error fcntl(..., F_GETFL)");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}
	arg &= (~O_NONBLOCK);
	if (fcntl(server->cSocket, F_SETFL, arg) < 0) {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "Error fcntl(..., F_SETFL)");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	LOG_INFO_CTX((server->ctx)) {
		ADD_STR_LOG("message", "connection accepted on port");
		ADD_STR_LOG("name", server->name);
		ADD_INT_LOG("port", port);
	}

	o = 1;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_REUSEADDR, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	o = 1;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_REUSEPORT, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if (setsockopt(server->cSocket, IPPROTO_TCP, TCP_NODELAY, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: nodelay");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	o = 70000000;
	if (setsockopt(server->cSocket, SOL_SOCKET, SO_SNDBUF, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: sndbuf");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	o = 1;
	if (setsockopt(server->cSocket, IPPROTO_TCP, TCP_QUICKACK, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: quickack");
			ADD_STR_LOG("name", server->name);
		}
		return false;
	}

	if (!Set_Recv_Timeout_TCP_SERVER(server, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	LOG_INFO_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Connection successful");
		ADD_STR_LOG("name", server->name);
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

	if (server->cSocket > 0) {
		shutdown(server->cSocket, 2);
		close(server->cSocket);

		server->cSocket = NULL;
	} else {
		LOG_ERROR_CTX((server->ctx)) {
			ADD_STR_LOG("message",
				"Client connection has already been closed");
			ADD_STR_LOG("name", server->name);
		}
	}

	LOG_INFO_CTX((server->ctx)) {
		ADD_STR_LOG("message", "Server connection closed");
		ADD_STR_LOG("name", server->name);
	}
}
