#ifndef CLIENT_CPP_
#define CLIENT_CPP_

#include "Client.h"

Client::Client() {
	Init();
}

bool Client::Init() {
	StartService();
	int iSetOption = 1;
	destination.sin_family = AF_INET; //ipv4
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption)); //potentially dangerous
	return (startedUp = (!(mainSocket < 0)));
}

bool Client::OpenConnection(int port, string ip) {
	if (!startedUp) return false;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = inet_addr(ip.c_str());
	bool r = connect(mainSocket, (sockaddr *) &destination, sizeof destination) == 0;
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_RCVBUF, &buff_size, (int) sizeof(buff_size));
	return r;
}

void Client::CloseConnection() {
	EndService();
	startedUp = false;
}

bool Client::Send(char *data, int size) {
	return (startedUp && send(mainSocket, data, size, MSG_WAITALL) == size);
}

bool Client::Receive(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_WAITALL) == size);
}

bool Client::Receive_b(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_DONTWAIT) == size);
}

#endif /* CLIENT_H_ */
