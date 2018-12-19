#ifndef SERVER_CPP_
#define SERVER_CPP_

#include "Server.h"

Server::Server() {
	Init();
}

bool Server::Init() {
	int iSetOption = 1;
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

bool Server::Listen(int port) {
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
	if (listen(mainSocket, 5) < 0){
//		log_err_no_kill("Unable to listen to socket on port:" + to_string(port));
		return false;
	}
	sockaddr_in clientAddress;
	int clientSize = sizeof(clientAddress);
	mainSocket = accept(mainSocket, (sockaddr *) &clientAddress, (socklen_t *) &clientSize);
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

void Server::CloseConnection() {
	EndService();
	startedUp = false;
}

Server::~Server() {
	CloseConnection();
}

bool Server::Send(char *data, int size) {
	return (startedUp && send(mainSocket, data, size, MSG_WAITALL) == size);
}

bool Server::Receive(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_WAITALL) == size);
}

bool Server::ReceiveNonBlocking(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_DONTWAIT) == size);
}

#endif
