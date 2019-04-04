//PLATFORMS: Windows
#ifdef _WIN64

#include "UDP_Client.h"

void Initialize_UDP_CLIENT(UDP_CLIENT* client) {

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &client->wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((client->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	int opt = 1;

	if (setsockopt(client->s, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) {
		printf("setsockopt() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	u_long nMode = 0; // 0: BLOCKING
	if (ioctlsocket(client->s, FIONBIO, &nMode) == SOCKET_ERROR) {
		printf("ioctlsocket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&client->si_other, 0, sizeof(client->si_other));
	client->si_other.sin_family = AF_INET;
	client->si_other.sin_port = htons(PORT);
	client->si_other.sin_addr.s_addr = inet_addr(SERVER);

	int bind_status = bind(client->s, (SOCKADDR *)&client->si_other, sizeof(client->si_other));

	cout << bind_status << " " << WSAGetLastError() << endl;
}


int Receive_UDP_CLIENT(UDP_CLIENT* client, char* buffer, int size) {

	return recvfrom(client->s, buffer, size, 0, (struct sockaddr *) &client->si_other, &client->slen);

	/*
	if (received_size == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	*/
	//return received_size;
}

void Close_UDP_CLIENT(UDP_CLIENT* client) {
	closesocket(client->s);
	WSACleanup();
}

#endif