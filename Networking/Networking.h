#ifndef NETWORKING_H_
#define NETWORKING_H_

//#define __WINDOWS__
#define __LINUX__
//#define __MAC__

#ifdef __WINDOWS__
#include <winsock.h>

#define StartService() {WSADATA wsaData; WSAStartup(0x0202, &wsaData);}
#define EndService() {closesocket(mainSocket); WSACleanup();}
#define socklen_t int
#endif

#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define StartService() {}
#define EndService() {shutdown(mainSocket, 2);}
#endif

#include <string>
#include <iostream>
using namespace std;


#endif /* NETWORKING_H_ */
