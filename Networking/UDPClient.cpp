#include "UDPClient.h"

UDPClient::UDPClient() {
	Init();
}

bool UDPClient::Init() {
	StartService();
	int iSetOption = 1;
	destination.sin_family = AF_INET; //ipv4
	mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption)); //potentially dangerous
	return (startedUp = (!(mainSocket < 0)));
}

bool UDPClient::OpenConnection(int port, string ip) {
	if (!startedUp) return false;
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = inet_addr(ip.c_str());
	bool r = connect(mainSocket, (sockaddr *) &destination, sizeof destination) == 0;
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_RCVBUF, &buff_size, (int) sizeof(buff_size));
	return r;
}

void UDPClient::CloseConnection() {
	EndService();
	startedUp = false;
}

bool UDPClient::Send(char *data, int size) {
	return (startedUp && send(mainSocket, data, size, MSG_WAITALL) == size);
}

bool UDPClient::Receive(char *data, int size) {
	return (startedUp && recv(mainSocket, data, size, MSG_WAITALL) == size);
}
