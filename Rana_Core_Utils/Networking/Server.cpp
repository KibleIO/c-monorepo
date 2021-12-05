#include "Server.h"

Server::Server() {
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;

	Init();
}

void Server::Init() {
#ifdef __linux__
	int o = 1;

	signal(SIGPIPE, SIG_IGN);

	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	cSocket = -1;

	if (setsockopt(
	lSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&o, sizeof o) != 0) {
		log_dbg(((const JSON_TYPE){
			{"message", "bad setsockopt: reuseaddr"},
			JSON_TYPE_END}));
	}

	if (lSocket < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Server socked failed to open"},
			{"name", "name"},
			JSON_TYPE_END}));
	}

	o_pr = 4;
#endif
#ifdef _WIN64
	WSADATA wsaData;
	int ret;

	lSocket = INVALID_SOCKET;
	cSocket = INVALID_SOCKET;

	if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		log_err(name + ": WSAStartup failed with error " + to_string(ret));
	}
#endif

	connected = false;
	Set_Recv_Timeout(30);
}

void Server::Set_Opts() {
	// Fixed options
#ifdef __linux__
	int o_ra = 1;
	int o_nd = 1;
	int o_sb = 70000000;
	int o_qa = 1;
#endif
#ifdef _WIN64
	bool o_ra = true;
	bool o_nd = true;
	DWORD o_sb = 70000000;
#endif

	if (setsockopt(cSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&o_ra,
		sizeof o_ra) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: reuseaddr"},
			{"name", "name"},
			JSON_TYPE_END}));
	}

	if (setsockopt(cSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&o_nd,
		sizeof o_nd) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: nodelay"},
			{"name", "name"},
			JSON_TYPE_END}));
	}

	if (setsockopt(cSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&o_sb,
		sizeof o_sb) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: sndbuf"},
			{"name", "name"},
			JSON_TYPE_END}));
	}

#ifdef __linux__
	if (setsockopt(cSocket, IPPROTO_TCP, TCP_QUICKACK, (const char*)&o_qa,
		sizeof o_qa) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: quickack"},
			{"name", "name"},
			JSON_TYPE_END}));
	}
#endif

	// User set options
	if (setsockopt(cSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&o_to,
		sizeof o_to) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: timeout"},
			{"name", "name"},
			JSON_TYPE_END}));
	}

#ifdef __linux__
	if (setsockopt(cSocket, SOL_SOCKET, SO_PRIORITY, (const char*)&o_pr,
		sizeof o_pr) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "bad setsockopt: priority"},
			{"name", "name"},
			JSON_TYPE_END}));
	}
#endif

}

void Server::Set_Name(string _name) {
	name = _name;
}

void Server::Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc) {
	if (_enc) {
		enc = _enc;
		if (enc_buf_auth) {
			delete [] enc_buf_auth;
		}
		enc_buf_auth = new char[NETWORKING_BUFFER_SIZE];
		enc_buf_data = enc_buf_auth + crypto_onetimeauth_BYTES;
	}
}

void Server::Set_Recv_Timeout(int seconds, int useconds) {
#ifdef __linux__
	o_to.tv_sec = seconds;
	o_to.tv_usec = useconds;
	if (cSocket > 0) {
		Set_Opts();
	}
#endif
#ifdef _WIN64
	o_to = seconds * 1000 + useconds / 1000;
	if (cSocket != INVALID_SOCKET) {
		Set_Opts();
	}
#endif
}

void Server::Set_High_Priority() {
#ifdef __linux__
	o_pr = 6;
	if (cSocket > 0) {
		Set_Opts();
	}
#endif
}

bool Server::Bind(int port) {
#ifdef __linux__
	sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;

	if (bind(lSocket, (sockaddr*)&destination, sizeof(destination)) < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Unable to bind socket on port"},
			{"name", "name"},
			JSON_TYPE_END}));
		return false;
	}
#endif
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
		log_err(name + ": getaddrinfo failed with error " + to_string(ret));
		WSACleanup();
		return false;
	}

	lSocket = socket(
	result->ai_family, result->ai_socktype, result->ai_protocol);
	if (lSocket == INVALID_SOCKET) {
		log_err(
		name + ": socket failed with error " + to_string(WSAGetLastError()));
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	ret = ::bind(lSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR) {
		log_err(
		name + ": bind failed with error: " + to_string(WSAGetLastError()));
		freeaddrinfo(result);
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);
#endif

	log_dbg(((const JSON_TYPE){
		{"message", "bound on port"},
		{"name", "name"},
		JSON_TYPE_END}));
	c_port = port;

	return true;
}

bool Server::ListenBound() {
	log_dbg(((const JSON_TYPE){
		{"message", "listening on port"},
		{"name", "name"},
		JSON_TYPE_END}));
	uint8_t lconnected = false;

#ifdef __linux__
	if (listen(lSocket, 5) < 0) {
		log_err(((const JSON_TYPE){
			{"message", "could not listen on socket"},
			{"name", "name"},
			JSON_TYPE_END}));
		return false;
	}

	sockaddr_in cAddress;
	socklen_t cSize = sizeof(cAddress);

	// connection timeout
	thread connect_timeo([&lconnected, this]() {
		uint8_t counter = 100;
		while (!lconnected && counter-- > 0) {
			Sleep_Milli(100);
		}
		if (!lconnected) {
			CloseConnection();
		}
	});

	if ((cSocket = accept(lSocket, (sockaddr*)&cAddress,
		(unsigned int*)&cSize)) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "could not accept connection on socket"},
			{"name", "name"},
			JSON_TYPE_END}));
		connect_timeo.join();
		return false;
	} else {
		lconnected = true;
	}

	connect_timeo.join();

	close(lSocket);
#endif
#ifdef _WIN64
	int ret = listen(lSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR) {
		log_err(name + ": listen failed with error " + to_string(WSAGetLastError()));
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	cSocket = accept(lSocket, (sockaddr*)NULL, (int*)NULL);
	if (cSocket == INVALID_SOCKET) {
		log_err(name + ": accept failed with error " + to_string(WSAGetLastError()));
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	closesocket(lSocket);
	/*
	if (listen(lSocket, SOMAXCONN) == SOCKET_ERROR) {
		log_err(name + ": listen failed with error " + to_string(WSAGetLastError()));
		closesocket(lSocket);
		WSACleanup();
		return false;
	}

	// connection timeout
	thread connect_timeo([&lconnected, this]() {
		uint8_t counter = 100;
		while (!lconnected && counter-- > 0) {
			Sleep_Milli(100);
		}
		if (!lconnected) {
			CloseConnection();
		}
	});

	cSocket = accept(lSocket, (sockaddr*)NULL, (int*)NULL);
	if ((cSocket = accept(lSocket, (sockaddr*)NULL, (int*)NULL)) ==
	INVALID_SOCKET) {
		log_err(name + ": accept failed with error " + to_string(WSAGetLastError()));
		connect_timeo.join();
		closesocket(lSocket);
		WSACleanup();
		return false;
	} else {
		lconnected = true;
	}

	connect_timeo.join();

	closesocket(lSocket);
	*/
#endif

	log_dbg(((const JSON_TYPE){
		{"message", "connection accepted on port"},
		{"name", "name"},
		JSON_TYPE_END}));
	connected = true;
	Set_Opts();

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
#ifdef __linux__
	shutdown(lSocket, 2);
	close(lSocket);
	shutdown(cSocket, 2);
	close(cSocket);
#endif
#ifdef _WIN64
	closesocket(lSocket);
	closesocket(cSocket);
	WSACleanup();
#endif

	connected = false;
}

bool Server::Send(char *data, int size) {
	if (!connected) {
		log_err(((const JSON_TYPE){
			{"message", "server not connected, can't send"},
			{"name", "name"},
			JSON_TYPE_END}));
		return false;
	}

	if (enc) {
		if (!Encrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)data, size, (uint8_t*)enc_buf_data)) {
			log_err(((const JSON_TYPE){
				{"message", "unable to encrypt data"},
				{"name", "name"},
				JSON_TYPE_END}));
			return false;
		}
		if (!Generate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err(((const JSON_TYPE){
				{"message", "unable to generate auth code"},
				{"name", "name"},
				JSON_TYPE_END}));
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

bool Server::Receive(char *data, int size) {
	if (!connected) {
		log_err(((const JSON_TYPE){
			{"message", "server not connected, can't receive"},
			{"name", "name"},
			JSON_TYPE_END}));
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
			log_error({
				{"message", "unable to receive"},
				{"name", STRING_TO(name)},
				{"error_code", TO_STRING(log_str, errno)},
				JSON_TYPE_END});
			return false;
		}

		size -= crypto_onetimeauth_BYTES;

		if (!Authenticate_Auth_Code_ENCRYPTION_PROFILE(enc,
			(uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {

			log_err(((const JSON_TYPE){
				{"message", "unable to authenticate data"},
				{"name", "name"},
				JSON_TYPE_END}));
			return false;
		}

		if (!Decrypt_Data_ENCRYPTION_PROFILE(enc,
			(uint8_t*) enc_buf_data, size, (uint8_t*)data)) {

			log_err(((const JSON_TYPE){
				{"message", "unable to decrypt data"},
				{"name", "name"},
				JSON_TYPE_END}));
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
			log_error({
				{"message", "unable to receive"},
				{"name", STRING_TO(name)},
				{"error_code", TO_STRING(log_str, errno)},
				JSON_TYPE_END});

			return false;
		}
	}

	return true;
}

Server::~Server() {
	CloseConnection();
	if (enc_buf_auth) {
		delete [] enc_buf_auth;
	}
}
