#ifndef MULTISERVER_H_
#define MULTISERVER_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string.h>
#include <errno.h>
#include <mutex>

using namespace std;

struct Sockets {
	int socket;
	Sockets* next;
};

class MultiServer {
private:
	bool startedUp;
	int mainSocket;
	sockaddr_in destination;
	Sockets* head;
	mutex mtx;

public:
	MultiServer();
	bool Init();
	bool Listen(int port);
	int Accept();
	void Remove(int pid);
	void CloseConnection();
	bool Send(char* data, int size, int sock);
	bool Receive(char* data, int size, int sock);
	~MultiServer();
};

#endif
