//PLATFORMS: Windows
#ifdef _WIN64

#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>

#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVER "127.0.0.1"	//ip address of udp server
#define BUFLEN 1500	//Max length of buffer
#define PORT 4011	//The port on which to listen for incoming data

struct UDP_CLIENT {
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
};

void Initialize_UDP_CLIENT(UDP_CLIENT*);
int Receive_UDP_CLIENT(UDP_CLIENT*, char*, int);
void Close_UDP_CLIENT(UDP_CLIENT*);

#endif

#endif