#include "UDPServer.h"

UDPServer::UDPServer() {
	Init();
}

bool UDPServer::Init() {
	int iSetOption = 1;
	destination.sin_family = AF_INET;
	mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption)); //potentially dangerous
	return (startedUp = (!(mainSocket < 0)));
}

bool UDPServer::Listen(int port) {
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
	return true;
}

void UDPServer::CloseConnection() {
	EndService();
	startedUp = false;
}

UDPServer::~UDPServer() {
	CloseConnection();
}

bool UDPServer::Send(char *data, int size) {
	return (startedUp && send(mainSocket, data, size, MSG_WAITALL) == size);
}

bool UDPServer::Receive(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_WAITALL) == size);
}
