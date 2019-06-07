//PLATFORMS: Linux, Windows, OSX (TODO)

#include "Client.h"

Client::Client() {
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;

	Init();
}

void Client::Init() {
	connected = false;

	// Linux specific code {{{
	#ifdef __linux__
	cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int o;
	o = 1;
	setsockopt(cSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&o, sizeof o);

	setsockopt(cSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&o, sizeof o);

	setsockopt(cSocket, IPPROTO_TCP, TCP_QUICKACK, (char*)&o, sizeof o);

	if (cSocket < 0) {
		log_err(name + ": Client socket failed to open");
	}
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	cSocket = INVALID_SOCKET;

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		log_err(name + ": WSAStartup failed with error " + to_string(ret));
	}
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	Set_Recv_Timeout(1);
}

void Client::Set_Name(string _name) {
	name = _name;
}

void Client::Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc) {
	if (_enc) {
		enc = _enc;
		if (enc_buf_auth) {
			delete [] enc_buf_auth;
		}
		enc_buf_auth = new char[NETWORKING_BUFFER_SIZE];
		enc_buf_data = enc_buf_auth + crypto_onetimeauth_BYTES;
	}
}

void Client::Set_Recv_Timeout(int seconds, int useconds) {
	// Linux specific code {{{
	#ifdef __linux__
	uint8_t rv;
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	rv =
	setsockopt(cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	if (rv != 0) {
		log_err("bad setsockopt");
	}

	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	DWORD tv = seconds * 1000 + useconds / 1000;
	setsockopt(cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	setsockopt(cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	#endif
	// }}}
}

void Client::Set_High_Priority() {
	#ifdef __linux__
	int32_t o;
	o = 6;
	setsockopt(cSocket, SOL_SOCKET, SO_PRIORITY, (const char*)&o, sizeof o);
	#endif
}

bool Client::OpenConnection(int port, string ip) {
	// Linux specific code {{{
	#ifdef __linux__
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &(destination.sin_addr.s_addr)) < 1) {
		int ret;
		int reps = 5;
		gaicb* reqs;

		reqs = new gaicb;

		memset(reqs, 0, sizeof (gaicb));
		reqs->ar_name = ip.c_str();

		ret = getaddrinfo_a(GAI_NOWAIT, &reqs, 1, NULL);
	    if (ret != 0) {
			log_err(
			name + ": getaddrinfo_a() failed " + ip + ":" + to_string(port));
			return false;
	    }

		while (reps-- > 0 && gai_error(reqs) != 0) {
			Sleep_Milli(100);
		}

		if (reps >= 0) {
			destination.sin_addr.s_addr = (
			(struct sockaddr_in *) reqs->ar_result->ai_addr)->sin_addr.s_addr;
		} else {
			log_err(
			name + ": gai_error() failed " + ip + ":" + to_string(port));
			return false;
		}

		gai_cancel(reqs);
		freeaddrinfo(reqs->ar_result);

		delete reqs;
	}

	long arg;
	timeval tv;
	fd_set myset;
	socklen_t lon;
	int valopt;

	if ((arg = fcntl(cSocket, F_GETFL, NULL)) < 0) {
		log_err(
		name + ": Error fcntl(..., F_GETFL) " + ip + ":" + to_string(port));
		return false;
	}
	arg |= O_NONBLOCK;
	if (fcntl(cSocket, F_SETFL, arg) < 0) {
		log_err(
		name + ": Error fcntl(..., F_SETFL) " + ip + ":" + to_string(port));
		return false;
	}

	int32_t res =
	connect(cSocket, (sockaddr*)&destination, sizeof(destination));

	if (res < 0) {
		if (errno == EINPROGRESS) {
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			FD_ZERO(&myset);
			FD_SET(cSocket, &myset);
			if (select(cSocket + 1, NULL, &myset, NULL, &tv) > 0) {
				lon = sizeof(int);
				getsockopt(
				cSocket, SOL_SOCKET, SO_ERROR, (void*)(&valopt),
				(unsigned int*)&lon);
				if (valopt) {
					log_err(
					name + ": Error in connection() " + ip + ":" +
					to_string(port));
					return false;
				}
			} else {
				log_err(
				name + ": Timeout or Error select() " + ip + ":" +
				to_string(port));
				return false;
			}
		} else {
			log_err(
			name + ": Error connecting " + ip + ":" + to_string(port));
			return false;
		}
 	}

	if ((arg = fcntl(cSocket, F_GETFL, NULL)) < 0) {
		log_err(
		name + ": Error fcntl(..., F_GETFL) " + ip + ":" + to_string(port));
		return false;
	}
	arg &= (~O_NONBLOCK);
	if (fcntl(cSocket, F_SETFL, arg) < 0) {
		log_err(
		name + ": Error fcntl(..., F_SETFL) " + ip + ":" + to_string(port));
		return false;
	}

	int o_rcvbuf = 700000;
	setsockopt(cSocket, SOL_SOCKET, SO_RCVBUF, &o_rcvbuf, sizeof(o_rcvbuf));

	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(ip.c_str(), to_string(port).c_str(), &hints, &result);
	if (ret != 0) {
		log_dbg(name + ": getaddrinfo failed with error " + to_string(ret));
		return false;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		cSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (cSocket == INVALID_SOCKET) {
			log_err(
			name + ": socket failed with error " +
			to_string(WSAGetLastError()));
			return false;
		}

		ret = connect(cSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		ULONG o_rcvbuf = 1500000;
		setsockopt(
		cSocket, SOL_SOCKET, SO_RCVBUF, (char*)&o_rcvbuf, sizeof(o_rcvbuf));

		if (ret == SOCKET_ERROR) {
			closesocket(cSocket);
			cSocket = INVALID_SOCKET;
		} else {
			break;
		}
	}

	freeaddrinfo(result);

	if (cSocket == INVALID_SOCKET) {
		log_err(name + ": Could not connect to " + ip + ":" + to_string(port));
		return false;
	}
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	log_dbg(name + ": Connection successful " + ip + ":" + to_string(port));
	connected = true;

	return true;
}

void Client::CloseConnection() {
	// Linux specific code {{{
	#ifdef __linux__
	shutdown(cSocket, 2);
	close(cSocket);
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	closesocket(cSocket);
	WSACleanup();
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}
	log_dbg(name + ": Client connection closed");
	connected = false;
}

bool Client::Send(char *data, int size) {
	if (!connected) {
		log_dbg(name + ": client not connected, can't send");
		return false;
	}

	if (enc) {
		if (!Encrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)data, size, (uint8_t*)enc_buf_data)) {
			log_err(name + ": unable to encrypt data");
			return false;
		}

		if (!Generate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err(name + ": unable to generate auth code");
			return false;
		}

		size += crypto_onetimeauth_BYTES;
		data = enc_buf_auth;
	}

	// Linux specific code {{{
	#ifdef __linux__
	return send(cSocket, data, size, MSG_WAITALL) == size;
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	return send(cSocket, data, size, 0) == size;
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}
}

bool Client::Receive(char *data, int size) {
	if (!connected) {
		log_dbg(name + ": client not connected, can't receive");
		return false;
	}

	if (enc) {
		size += crypto_onetimeauth_BYTES;

		// Linux specific code {{{
		#ifdef __linux__
		bool recvd = recv(cSocket, enc_buf_auth, size, MSG_WAITALL) == size;
		#endif
		// }}} Windows specific code {{{
		#ifdef _WIN64
		bool recvd = recv(cSocket, enc_buf_auth, size, 0) == size;
		#endif
		// }}} OSX specific code {{{
		#ifdef __APPLE__
		//TODO apple code
		#endif
		// }}}

		if (!recvd) {
			log_err(name + ": unable to receive");
			return false;
		}

		size -= crypto_onetimeauth_BYTES;

		if (!Authenticate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err(name + ": unable to authenticate data");
			return false;
		}

		if (!Decrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)data)) {
			log_err(name + ": unable to decrypt data");
			return false;
		}
	} else {
		// Linux specific code {{{
		#ifdef __linux__
		bool recvd = recv(cSocket, data, size, MSG_WAITALL) == size;
		#endif
		// }}} Windows specific code {{{
		#ifdef _WIN64
		bool recvd = recv(cSocket, data, size, 0) == size;
		#endif
		// }}} OSX specific code {{{
		#ifdef __APPLE__
		//TODO apple code
		#endif
		// }}}

		if (!recvd) {
			log_err(name + ": unable to receive");
			return false;
		}
	}
	return true;
}

Client::~Client() {
	CloseConnection();
	if (enc_buf_auth) {
		delete [] enc_buf_auth;
	}
}
