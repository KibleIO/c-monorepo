#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#ifdef __linux__

#include <Utilities/LOGGING.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

#define SOCKET_ERROR -1

#else

#include <Networking/Networking.h>
#include <Utilities/LOGGING.h>
#include <stdint.h>
#include <string.h>

#include <string>

#endif

#include <Utilities/TIMER.h>
#include <iostream>

//// Linux specific includes {{{
//#ifdef __linux__
//#define socklen_t int
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
//
//#endif
//// }}} Windows specific includes {{{
//#ifdef _WIN64
//#include <WinSock2.h>
//#include <Windows.h>
//#include <WS2tcpip.h>
//#endif

#define TEST_BUFF_SIZE 4
#define CONNECT_ATTEMPTS 50

//#define BUFLEN 1500

using namespace std;

struct UDP_CLIENT {
	int32_t sockfd;
	sockaddr_in server_address;
	uint32_t server_address_size;

// struct sockaddr_in si_other;
// int s, slen = sizeof(si_other);
// char buf[BUFLEN];
// char message[BUFLEN];
#ifdef _W64
	WSADATA wsa;
#endif
};

uint8_t Initialize_UDP_CLIENT(UDP_CLIENT*);
uint8_t Connect_UDP_CLIENT(UDP_CLIENT*, string, uint32_t);
void Set_Timeout_UDP_CLIENT(UDP_CLIENT*, int, int);
uint8_t Send_UDP_CLIENT(UDP_CLIENT*, uint8_t*, int32_t);
int32_t Receive_UDP_CLIENT(UDP_CLIENT*, uint8_t*, int32_t);
uint8_t Receive_Peek_UDP_CLIENT(UDP_CLIENT*, int32_t&);
void Delete_UDP_CLIENT(UDP_CLIENT*);

#endif
