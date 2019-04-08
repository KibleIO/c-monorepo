//PLATFORMS: Linux, Windows, OSX (TODO)

#include "Server.h"

Server::Server() {
	Init();
}

void Server::Init() {
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;

	connected = false;

	// Linux specific code {{{
	#ifdef __linux__
	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	cSocket = -1;

	int o_raddr = 1;
	setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, &o_raddr, sizeof(o_raddr));

	if (lSocket < 0) {
		log_err("Server socked failed to open");
	}
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	lSocket = INVALID_SOCKET;
	cSocket = INVALID_SOCKET;

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		log_err("WSAStartup failed with error " + to_string(ret));
	}
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}
}

void Server::Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc) {
	if (_enc) {
		enc = _enc;
		if (enc_buf_auth) {
			delete enc_buf_auth;
		}
		enc_buf_auth = new char[NETWORKING_BUFFER_SIZE];
		enc_buf_data = enc_buf_auth + crypto_onetimeauth_BYTES;
	}
}

bool Server::Bind(int port) {
	// Linux specific code {{{
	#ifdef __linux__
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;

	if (bind(lSocket, (sockaddr*)&destination, sizeof(destination)) < 0) {
		log_err("Unable to bind socket on port: " + to_string(port));
		return false;
	}
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	struct addrinfo *result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int ret = getaddrinfo(
	NULL, (PCSTR)to_string(port).c_str(), &hints, &result);
	if (ret != 0) {
		log_err("getaddrinfo failed with error " + to_string(ret));
		WSACleanup();
		return false;
	}

	lSocket = socket(
	result->ai_family, result->ai_socktype, result->ai_protocol);
	if (lSocket == INVALID_SOCKET) {
		log_err("socket failed with error " + to_string(WSAGetLastError()));
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	ret = bind(lSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR) {
		log_err("bind failed with error: " + to_string(WSAGetLastError()));
		freeaddrinfo(result);
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	log_dbg("sever bound on port " + to_string(port));
	c_port = port;

	return true;
}

bool Server::ListenBound() {
	// Linux specific code {{{
	#ifdef __linux__
	if (listen(lSocket, 5) < 0) {
		log_dbg("could not listen on socket");
		return false;
	}

	sockaddr_in cAddress;
	socklen_t cSize = sizeof(cAddress);
	cSocket = accept(lSocket, (sockaddr*)&cAddress, &cSize);

	close(lSocket);

	if (cSocket < 0) {
		log_dbg("could not accept connection on socket");
		return false;
	}

	int o_sndbuf = 700000, o_nodelay = 1;
	setsockopt(cSocket, SOL_SOCKET, SO_SNDBUF, &o_sndbuf, sizeof(o_sndbuf));
	setsockopt(cSocket, SOL_TCP, TCP_NODELAY, &o_nodelay, sizeof(o_nodelay));
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	int ret = listen(lSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR) {
		log_err("listen failed with error " + to_string(WSAGetLastError()));
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	cSocket = accept(lSocket, (sockaddr*)NULL, (int*)NULL);
	if (cSocket == INVALID_SOCKET) {
		log_err("accept failed with error " + to_string(WSAGetLastError()));
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	closesocket(lSocket);

	int o_sndbuff = 0;
	if (setsockopt(cSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&o_sndbuff,
	sizeof(o_sndbuff)) == SOCKET_ERROR) {
		log_err("setsockopt failed with error " + to_string(WSAGetLastError()));
		closesocket(cSocket);
		WSACleanup();
		return 1;
	}

	DWORD o_nodelay = 1;
	if (setsockopt(cSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&o_nodelay,
	sizeof(o_nodelay)) == SOCKET_ERROR) {
		log_err("setsockopt failed with error " + to_string(WSAGetLastError()));
		closesocket(cSocket);
		WSACleanup();
		return 1;
	}
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	log_dbg("connection accepted on port " + to_string(c_port));
	connected = true;

	return true;
}

bool Server::Listen(int port) {
	if (connected) {
		CloseConnection();
	}

	if (!Bind(port)) {
		return false;
	}

	return ListenBound();
}

void Server::CloseConnection() {
	// Linux specific code {{{
	#ifdef __linux__
	shutdown(lSocket, 2);
	close(lSocket);
	shutdown(cSocket, 2);
	close(cSocket);
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	closesocket(lSocket);
	closesocket(cSocket);
	WSACleanup();
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	connected = false;
}

bool Server::Send(char *data, int size) {
	if (!connected) {
		log_dbg("server not connected, can't send");
		return false;
	}

	if (enc) {
		if (!Encrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)data, size, (uint8_t*)enc_buf_data)) {
			log_err("unable to encrypt data");
			return false;
		}
		if (!Generate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err("unable to generate auth code");
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

bool Server::Receive(char *data, int size) {
	if (!connected) {
		log_dbg("server not connected, can't receive");
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
			log_err("unable to receive");
			return false;
		}

		size -= crypto_onetimeauth_BYTES;

		if (!Authenticate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err("unable to authenticate data");
			return false;
		}

		if (!Decrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)data)) {
			log_err("unable to decrypt data");
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
			log_err("unable to receive");
			return false;
		}
	}

	return true;
}

Server::~Server() {
	CloseConnection();
	if (enc_buf_auth) {
		delete enc_buf_auth;
	}
}
