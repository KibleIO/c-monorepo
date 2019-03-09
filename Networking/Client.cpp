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

	startedUp = (!(mainSocket < 0));
	if (startedUp) {
		log_dbg("Client initialized successfully");
	} else {
	 	log_err("Client socket failed to open");
	}

	return (startedUp);
}

bool Client::OpenConnection(int port, string ip) {
	if (!startedUp)
	{
		log_err("Cannot open connection because client is not started up");
		return false;
	}
	destination.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &(destination.sin_addr.s_addr)) < 1) {
		struct addrinfo hints, *res;

		memset (&hints, 0, sizeof (hints));
		  hints.ai_family = PF_UNSPEC;
		  hints.ai_socktype = SOCK_STREAM;
		  hints.ai_flags |= AI_CANONNAME;

		if (getaddrinfo (ip.c_str(), NULL, &hints, &res) != 0) {
		  return false;
		}

		while (res) {
			if (res->ai_family == AF_INET) {
				destination.sin_addr.s_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr.s_addr;
				goto success;
			}
			res = res->ai_next;
	    }

		return false;
	}
	success:
	//destination.sin_addr.s_addr = inet_addr(ip.c_str());
	bool r = connect(mainSocket, (sockaddr *) &destination, sizeof destination) == 0;
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_RCVBUF, &buff_size, (int) sizeof(buff_size));
	if (r) {
		log_dbg("Connection successful " + to_string(port) + " : " + ip);
	}
	return r;
}

void Client::CloseConnection() {
	EndService();
	log_dbg("Client connection closed");
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
