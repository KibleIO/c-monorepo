#include "Client.h"

Client::Client() {
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;

	Init();
}

void Client::Init() {
#ifdef __linux__
	int o;

	cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cSocket < 0) {
		log_err(name + ": Client socket failed to open");
	}

	o = 1;
	if (
	setsockopt(cSocket, SOL_SOCKET, SO_REUSEADDR, &o, sizeof o) != 0 ) {
		log_err("bad setsockopt: reuseaddr");
	}

	if (
	setsockopt(cSocket, IPPROTO_TCP, TCP_NODELAY, &o, sizeof o) != 0) {
		log_err("bad setsockopt: nodelay");
	}

	if (
	setsockopt(cSocket, IPPROTO_TCP, TCP_QUICKACK, &o, sizeof o) != 0) {
		log_err("bad setsockopt: quickack");
	}

	o = 700000;
	if (
	setsockopt(cSocket, SOL_SOCKET, SO_RCVBUF, &o, sizeof o) != 0) {
		log_err("bad setsockopt: rcvbuf");
	}

#endif
#ifdef _WIN64
	WSADATA wsaData;
	int ret;

	cSocket = INVALID_SOCKET;

	if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		log_err(name + ": WSAStartup failed with error " + to_string(ret));
	}
#endif

	connected = false;
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
#ifdef __linux__
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	if (setsockopt(
	cSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv) != 0) {
		log_err("bad setsockopt: rcvtimeo");
	}
#endif
#ifdef _WIN64
	DWORD tv = seconds * 1000 + useconds / 1000;
	if (setsockopt(
	cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
		log_err("bad setsockopt: rcvtimeo");
	}
#endif
}

void Client::Set_High_Priority() {
#ifdef __linux__
	int32_t o;
	o = 6;
	if (setsockopt(
	cSocket, SOL_SOCKET, SO_PRIORITY, (const char*)&o, sizeof o) != 0) {
		log_err("bad setsockopt: priority");
	}
#endif
}

bool Client::OpenConnection(int port, string ip) {
#ifdef __linux__
	// Set up server destination
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &(destination.sin_addr.s_addr)) < 1) {
		struct addrinfo hints, *res;

		memset (&hints, 0, sizeof (hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_CANONNAME;

		if (getaddrinfo (ip.c_str(), NULL, &hints, &res) != 0) {
			log_err(
			name + ": Could not connect to " + ip + ":" + to_string(port));
			return false;
		}

		while (res) {
			if (res->ai_family == AF_INET) {
				destination.sin_addr.s_addr =
				((struct sockaddr_in *) res->ai_addr)->sin_addr.s_addr;
				goto success;
			}
			res = res->ai_next;
	    }
		log_err(name + ": Could not connect to " + ip + ":" + to_string(port));
		return false;
	}

	success:

	// Connect to server
	if (connect(cSocket, (sockaddr*)&destination, sizeof(destination)) != 0) {
		log_err(name + ": Could not connect to " + ip + ":" + to_string(port));
		return false;
	}

#endif
#ifdef _WIN64
	// Set up server destination
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

	// Connect to server
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		cSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (cSocket == INVALID_SOCKET) {
			log_err(
			name + ": socket failed with error " +
			to_string(WSAGetLastError()));
			return false;
		}

		ret = connect(cSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (ret == SOCKET_ERROR) {
			closesocket(cSocket);
			cSocket = INVALID_SOCKET;
		} else {
			break;
		}
	}

	bool o_b = true;
	if (setsockopt(
	cSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&o_b, sizeof o_b) != 0 ) {
		log_err("bad setsockopt: reuseaddr");
	}
	
	if (setsockopt(
	cSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&o_b, sizeof o_b) != 0) {
		log_err("bad setsockopt: nodelay");
	}

	int o_d = 700000;
	if (setsockopt(
	cSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&o_d, sizeof o_d) != 0) {
		log_err("bad setsockopt: rcvbuf");
	}

	freeaddrinfo(result);

	if (cSocket == INVALID_SOCKET) {
		log_err(name + ": Could not connect to " + ip + ":" + to_string(port));
		return false;
	}
#endif

	log_dbg(name + ": Connection successful " + ip + ":" + to_string(port));
	connected = true;

	return true;
}

void Client::CloseConnection() {
#ifdef __linux__
	shutdown(cSocket, 2);
	close(cSocket);
#endif
#ifdef _WIN64
	closesocket(cSocket);
	WSACleanup();
#endif
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

#ifdef __linux__
	return send(cSocket, data, size, MSG_WAITALL) == size;
#endif
#ifdef _WIN64
	return send(cSocket, data, size, 0) == size;
#endif
}

bool Client::Receive(char *data, int size) {
	if (!connected) {
		log_dbg(name + ": client not connected, can't receive");
		return false;
	}

	if (enc) {
		size += crypto_onetimeauth_BYTES;

#ifdef __linux__
		bool recvd = recv(cSocket, enc_buf_auth, size, MSG_WAITALL) == size;
#endif
#ifdef _WIN64
		bool recvd = recv(cSocket, enc_buf_auth, size, 0) == size;
#endif
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
#ifdef __linux__
		bool recvd = recv(cSocket, data, size, MSG_WAITALL) == size;
#endif
#ifdef _WIN64
		bool recvd = recv(cSocket, data, size, 0) == size;
#endif
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
