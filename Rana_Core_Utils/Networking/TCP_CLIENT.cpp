#include "TCP_CLIENT.h"

bool Initialize_TCP_CLIENT(TCP_CLIENT *client, CONTEXT *ctx,
	TCP_CLIENT_MASTER *master, int id) {

	int o; //yes! best variable name evar

	client->ctx = ctx;
	client->tcp_master = master;
	Set_Name_TCP_CLIENT(client, "unknown");

	signal(SIGPIPE, SIG_IGN);

        #ifdef linux

	//https://stackoverflow.com/questions/38191726/c-how-to-prevent-child-process-binding-port-after-fork-on-linux
	client->cSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (client->cSocket < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client socket failed to open");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #else

        client->cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client->cSocket < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Client socket failed to open");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #endif

	o = 1;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_REUSEADDR, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	o = 1;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_REUSEPORT, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: reuseaddr");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	if (setsockopt(client->cSocket, IPPROTO_TCP, TCP_NODELAY, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: nodelay");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #ifdef linux

	if (setsockopt(client->cSocket, IPPROTO_TCP, TCP_QUICKACK, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: quickack");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        o = 70000000;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_RCVBUF, &o,
		sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: rcvbuf");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #endif

	if (!Set_Recv_Timeout_TCP_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	return true;
}

void Set_Name_TCP_CLIENT(TCP_CLIENT *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_TCP_CLIENT(TCP_CLIENT *client, int sec, int usec) {
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_RCVTIMEO, &tv,
		sizeof tv) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: rcvtimeo");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}
	return true;
}

bool Set_High_Priority_TCP_CLIENT(TCP_CLIENT *client) {
        #ifdef linux

	int32_t o;
	o = 6;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_PRIORITY,
		(const char*)&o, sizeof o) != 0) {

		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "bad setsockopt: priority");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

        #endif
	return true;
}

bool Connect_TCP_CLIENT(TCP_CLIENT *client) {
	long arg;
	timeval tv;
	fd_set myset;
	socklen_t lon;
	int valopt;
	int32_t res;
	int err;

	// Set up server destination
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(client->tcp_master->port);

	if (inet_pton(AF_INET, client->tcp_master->ip,
		&(destination.sin_addr.s_addr)) < 1) {

		if (!getaddrinfo_k(&destination.sin_addr.s_addr,
			client->tcp_master->ip, 2)) {

			LOG_ERROR_CTX((client->ctx)) {
				ADD_STR_LOG("message",
					"Failed to connect: getaddrinfo_k() "
					"failed");

				ADD_STR_LOG("ip", client->tcp_master->ip);
				ADD_STR_LOG("name", client->name);
			}
			return false;
		}
	}

	if ((arg = fcntl(client->cSocket, F_GETFL, NULL)) < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error fcntl(..., F_GETFL)");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	arg |= O_NONBLOCK;
	if (fcntl(client->cSocket, F_SETFL, arg) < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error fcntl(..., F_SETFL)");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	res = connect(client->cSocket, (sockaddr*)&destination,
		sizeof(destination));
	err = errno;

	if (res < 0) {
		if (err == EINPROGRESS) {
			tv.tv_sec = 0;
			tv.tv_usec = DEFAULT_CONNECT_TIMEOUT;
			FD_ZERO(&myset);
			FD_SET(client->cSocket, &myset);
			if (select(client->cSocket + 1, NULL, &myset, NULL,
				&tv) > 0) {

				lon = sizeof(int);
				if (getsockopt(client->cSocket, SOL_SOCKET,
					SO_ERROR, (void*)(&valopt),
					(unsigned int*)&lon) < 0) {

					LOG_ERROR_CTX((client->ctx)) {
						ADD_STR_LOG("message",
							"Failed to connect: Error in getsockopt");
						ADD_STR_LOG("name",
							client->name);
					}
				}

				if (valopt != 0) {
					LOG_ERROR_CTX((client->ctx)) {
						ADD_STR_LOG("message",
							"Failed to connect: Error in "
							"connection()");

						ADD_STR_LOG("name",
							client->name);

						ADD_INT_LOG("error_code",
							valopt);
					}
					return false;
				}
			} else {
				LOG_ERROR_CTX((client->ctx)) {
					ADD_STR_LOG("message",
						"Failed to connect: Timeout or Error select()");

					ADD_STR_LOG("name", client->name);
				}
				return false;
			}
		} else {
			LOG_ERROR_CTX((client->ctx)) {
				ADD_STR_LOG("message", "Failed to connect: Error connecting");
				ADD_STR_LOG("name", client->name);
				ADD_INT_LOG("error_code", err);
			}
			return false;
		}
 	} else {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Connect() threw an error");
			ADD_INT_LOG("err", err);
			ADD_INT_LOG("res", res);
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	if ((arg = fcntl(client->cSocket, F_GETFL, NULL)) < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error fcntl(..., F_GETFL)");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}
	arg &= (~O_NONBLOCK);
	if (fcntl(client->cSocket, F_SETFL, arg) < 0) {
		LOG_ERROR_CTX((client->ctx)) {
			ADD_STR_LOG("message", "Failed to connect: Error fcntl(..., F_SETFL)");
			ADD_STR_LOG("name", client->name);
		}
		return false;
	}

	return true;
}

bool Send_TCP_CLIENT(TCP_CLIENT *client, char *buffer, int size) {
	//we are omitting a check here to see if client is connected... don't be
	//stupid stupid
	return send(client->cSocket, buffer, size, MSG_WAITALL) == size;
}

bool Receive_TCP_CLIENT(TCP_CLIENT *client, char *buffer, int size) {
	return recv(client->cSocket, buffer, size, MSG_WAITALL) == size;
}

int Receive_Unsafe_TCP_CLIENT(TCP_CLIENT *client, char *buffer) {
	return recv(client->cSocket, buffer, ARBITRARILY_LARGE_PACKET, 0);
}

void Delete_TCP_CLIENT(TCP_CLIENT *client) {
	shutdown(client->cSocket, 2);
	close(client->cSocket);
}
