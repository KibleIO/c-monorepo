#include "TCP_CLIENT.h"

bool Initialize_TCP_CLIENT(TCP_CLIENT *client, KCONTEXT *ctx,
	TCP_CLIENT_MASTER *master, int id) {

	int o; //yes! best variable name evar

	client->ctx = ctx;
	client->tcp_master = master;
	Set_Name_TCP_CLIENT(client, "unknown");

	#ifndef _WIN64

	signal(SIGPIPE, SIG_IGN);

	#endif

        #ifdef linux

	//https://stackoverflow.com/questions/38191726/c-how-to-prevent-child-process-binding-port-after-fork-on-linux
	client->cSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (client->cSocket < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Client socket failed to open"},
			{"name", name},
		});
		return false;
	}

	#elif _WIN64

	WSADATA wsaData;
	int ret;

	if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "WSAStartup failed"},
			{"name", client->name},
		});
		return false;
	}

        #else

	client->cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client->cSocket < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Client socket failed to open"},
			{"name", client->name},
		});
		return false;
	}

        #endif

	#ifndef _WIN64

	o = 1;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_REUSEADDR, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", client->name},
		});
		return false;
	}

	o = 1;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_REUSEPORT, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: reuseaddr"},
			{"name", client->name},
		});
		return false;
	}

	if (setsockopt(client->cSocket, IPPROTO_TCP, TCP_NODELAY, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: nodelay"},
			{"name", client->name},
		});
		return false;
	}

        #ifdef linux

	if (setsockopt(client->cSocket, IPPROTO_TCP, TCP_QUICKACK, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: quickack"},
			{"name", client->name},
		});
		return false;
	}

        o = 70000000;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_RCVBUF, &o,
		sizeof o) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: rcvbuf"},
			{"name", client->name},
		});
		return false;
	}

        #endif

	if (!Set_Recv_Timeout_TCP_CLIENT(client, DEFAULT_RECV_TIMEOUT, 0)) {
		return false;
	}

	#endif

	return true;
}

void Set_Name_TCP_CLIENT(TCP_CLIENT *client, char *name) {
	strcpy(client->name, name);
}

bool Set_Recv_Timeout_TCP_CLIENT(TCP_CLIENT *client, int sec, int usec) {
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	if (setsockopt(client->cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv,
		sizeof tv) != 0) {

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: rcvtimeo"},
			{"name", client->name},
		});
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

		LOGGER_ERROR(client->ctx, {
			{"message", "bad setsockopt: priority"},
			{"name", client->name},
		});
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

	#ifndef _WIN64
	// Set up server destination
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(client->tcp_master->port);

	if (inet_pton(AF_INET, client->tcp_master->ip,
		&(destination.sin_addr.s_addr)) < 1) {

		if (!getaddrinfo_k(&destination.sin_addr.s_addr,
			client->tcp_master->ip, 2)) {

			LOGGER_ERROR(client->ctx, {
				{"message","Failed to connect: getaddrinfo_k() "
					"failed"},
				{"ip", client->tcp_master->ip},
				{"name", client->name},
			});
			return false;
		}
	}

	if ((arg = fcntl(client->cSocket, F_GETFL, NULL)) < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Failed to connect: Error fcntl(..., F_GETFL)"},
			{"name", client->name},
		});
		return false;
	}

	arg |= O_NONBLOCK;
	if (fcntl(client->cSocket, F_SETFL, arg) < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Failed to connect: Error fcntl(..., F_SETFL)"},
			{"name", client->name},
		});
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

					LOGGER_ERROR(client->ctx, {
						{"message",
							"Failed to connect: "
							"Error in getsockopt"},
						{"name", client->name},
					});
				}

				if (valopt != 0) {
					LOGGER_ERROR(client->ctx, {
						{"message", "Failed to "
							"connect: Error in "
							"connection()"},
						{"name", client->name},
						{"error_code", valopt},
					});
					return false;
				}
			} else {
				LOGGER_ERROR(client->ctx, {
					{"message", "Failed to connect: "
						"Timeout or Error select()"},
					{"name", client->name},
				});
				return false;
			}
		} else {
			LOGGER_ERROR(client->ctx, {
				{"message", "Failed to connect: Error connecting"},
				{"name", client->name},
				{"error_code", err},
			});
			return false;
		}
 	} else {
		LOGGER_ERROR(client->ctx, {
			{"message", "Failed to connect: Connect() threw an error"},
			{"err", err},
			{"res", res},
			{"name", client->name},
		});
		return false;
	}

	if ((arg = fcntl(client->cSocket, F_GETFL, NULL)) < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Failed to connect: Error fcntl(..., F_GETFL)"},
			{"name", client->name},
		});
		return false;
	}
	arg &= (~O_NONBLOCK);
	if (fcntl(client->cSocket, F_SETFL, arg) < 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Failed to connect: Error fcntl(..., F_SETFL)"},
			{"name", client->name},
		});
		return false;
	}

	#else

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(client->tcp_master->ip, to_string(client->tcp_master->port).c_str(), &hints, &result);
	if (ret != 0) {
		LOGGER_ERROR(client->ctx, {
			{"message", "Unknown type"},
			ADD_STR_LOG("message", "getaddrinfo failed");
			ADD_STR_LOG("name", client->name);
		});
		return false;
	}

	client->cSocket = INVALID_SOCKET;

	// Connect to server
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		client->cSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (client->cSocket == INVALID_SOCKET) {
			LOGGER_ERROR(client->ctx, {
				{"message", "Unknown type"},
				ADD_STR_LOG("message", "socket failed withd");
				ADD_STR_LOG("name", to_string(WSAGetLastError()).c_str());
			});
			return false;
		}

		ret = connect(client->cSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (ret == SOCKET_ERROR) {
			closesocket(client->cSocket);
			client->cSocket = INVALID_SOCKET;
		} else {
			break;
		}
	}

	bool o_b = true;
	if (setsockopt(
	client->cSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&o_b, sizeof o_b) != 0 ) {
		log_err("bad setsockopt: reuseaddr");
	}

	if (setsockopt(
	client->cSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&o_b, sizeof o_b) != 0) {
		log_err("bad setsockopt: nodelay");
	}

	int o_d = 700000;
	if (setsockopt(
	client->cSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&o_d, sizeof o_d) != 0) {
		log_err("bad setsockopt: rcvbuf");
	}

	freeaddrinfo(result);

	if (client->cSocket == INVALID_SOCKET) {
		return false;
	}
	#endif

	return true;
}

bool Send_TCP_CLIENT(TCP_CLIENT *client, char *buffer, int size) {
	//we are omitting a check here to see if client is connected... don't be
	//stupid stupid

	#ifdef _WIN64
	return send(client->cSocket, buffer, size, 0) == size;
	#else
	return send(client->cSocket, buffer, size, MSG_WAITALL) == size;
	#endif
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
