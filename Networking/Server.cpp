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
	return (startedUp = (!(mainSocket < 0)));
}

bool Server::Listen(int port) {
	if (!startedUp) return false;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;
	if (bind(mainSocket, (sockaddr *) &destination, sizeof(destination)) < 0){
		return false;
	}
	if (listen(mainSocket, 5) < 0){
		return false;
	}
	sockaddr_in clientAddress;
	int clientSize = sizeof(clientAddress);
	mainSocket = accept(mainSocket, (sockaddr *) &clientAddress, (socklen_t *) &clientSize);
	if (mainSocket < 0) {
		return false;
	}
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_SNDBUF, &buff_size, (int) sizeof(buff_size));
	int one = 1;
	setsockopt(mainSocket, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
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

#endif
