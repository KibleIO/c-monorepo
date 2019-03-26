#ifndef SERVER_CPP_
#define SERVER_CPP_

#include "Server.h"

void Server::Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc) {
	if (_enc) {
		enc = _enc;
		if (enc_buf_auth) {
			delete enc_buf_auth;
		}
		enc_buf_auth = new char[3000];
		enc_buf_data = enc_buf_auth + crypto_onetimeauth_BYTES;
	}
}

Server::Server() {
	Init();
}

bool Server::Init() {
	int iSetOption = 1;
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;
	destination.sin_family = AF_INET;
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
    sizeof(iSetOption)); //potentially dangerous

	startedUp = (!(mainSocket < 0));
	if (startedUp){
		log_dbg("Server initialized successfully");
	} else {
		log_err("Server socked failed to open");
	}

	return (startedUp);
}

bool Server::Bind(int port) {
//	log_dbg("Server listening on port: " + to_string(port));
	if (!startedUp) {
//		log_err_no_kill("Server cannot listen because it is not started up");
		return false;
	}
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;
	if (bind(mainSocket, (sockaddr *) &destination, sizeof(destination)) < 0){
//		log_err_no_kill("Unable to bind socket on port:" + to_string(port));
		return false;
	}
	return true;
}

bool Server::ListenBound() {
	if (listen(mainSocket, 5) < 0){
//		log_err_no_kill("Unable to listen to socket on port:" + to_string(port));
		return false;
	}
	sockaddr_in clientAddress;
	int clientSize = sizeof(clientAddress);
	int tempSocket = mainSocket;
	mainSocket = accept(mainSocket, (sockaddr *) &clientAddress, (socklen_t *) &clientSize);
	close(tempSocket);
	if (mainSocket < 0) {
//		log_err_no_kill("Unable to accept on port:" + to_string(port));
		return false;
	}
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_SNDBUF, &buff_size, (int) sizeof(buff_size));
	int one = 1;
	setsockopt(mainSocket, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
//	log_dbg("Connecting accepted on port: " + to_string(port));
	return true;
}

bool Server::Listen(int port) {
//	log_dbg("Server listening on port: " + to_string(port));
	if (!startedUp) {
//		log_err_no_kill("Server cannot listen because it is not started up");
		return false;
	}
	int one = 1;
	int ret;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;
	ret = bind(mainSocket, (sockaddr *) &destination, sizeof(destination));
	if (ret < 0){
		log_err("Unable to bind socket on port: " + to_string(port) + " " + strerror(errno));
//		log_err_no_kill("Unable to bind socket on port:" + to_string(port));
		return false;
	}
	if (listen(mainSocket, 5) < 0) {
//		log_err_no_kill("Unable to listen to socket on port:" + to_string(port));
		return false;
	}
	sockaddr_in clientAddress;
	int clientSize = sizeof(clientAddress);
	int tempSocket = mainSocket;
	mainSocket = accept(mainSocket, (sockaddr *) &clientAddress, (socklen_t *) &clientSize);
	close(tempSocket);
	if (mainSocket < 0) {
//		log_err_no_kill("Unable to accept on port:" + to_string(port));
		return false;
	}
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_SNDBUF, &buff_size, (int) sizeof(buff_size));
	one = 1;
	setsockopt(mainSocket, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
//	log_dbg("Connecting accepted on port: " + to_string(port));
	return true;
}

void Server::CloseConnection() {
	//EndService();
	close(mainSocket);
	startedUp = false;
}

Server::~Server() {
	CloseConnection();
}

void print_bytes(uint8_t* bytes, int n) {
	string str;
	for (int i = 0; i < n; i++) {
		str += to_string(bytes[i]) + " ";
	}
	log_dbg(str);
}

bool Server::Send(char *data, int size) {
	if (!startedUp) {
		return false;
	}

	if (enc) {
		//log_dbg(enc->identifier);
		//print_bytes(enc->poly1305_key, crypto_onetimeauth_KEYBYTES);
		//print_bytes(enc->poly1305_nonce, crypto_onetimeauth_KEYBYTES);
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

	return send(mainSocket, data, size, MSG_WAITALL) == size;
}

void print_nonce5(uint8_t* nonce, int n = 20) {
	string nonce_str;
	for (int i = 0; i < n; i++) {
		nonce_str += to_string(nonce[i]) + " ";
	}
	log_dbg(nonce_str);
}

bool Server::Receive(char *data, int size) {
	if (!startedUp) {
		return false;
	}

	if (enc) {
		//log_dbg(enc->identifier);
		//print_bytes(enc->poly1305_key, crypto_onetimeauth_KEYBYTES);
		//print_bytes(enc->poly1305_nonce, crypto_onetimeauth_KEYBYTES);
		size += crypto_onetimeauth_BYTES;
		if (recv(mainSocket, enc_buf_auth, size, MSG_WAITALL) != size) {
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
		if (recv(mainSocket, data, size, MSG_WAITALL) != size) {
			log_err("unable to receive");
			return false;
		}
	}

	return true;
}

//bool Server::ReceiveNonBlocking(char *data, int size) {
//	return (startedUp && recv(mainSocket, data, size, MSG_DONTWAIT) == size);
//}

#endif
