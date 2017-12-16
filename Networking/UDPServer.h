#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include "Networking.h"

class UDPServer {
private:
	bool        startedUp;
	int         mainSocket;
	sockaddr_in destination;
public:
	UDPServer();
	~UDPServer();
	bool Init();
	bool Listen(int);
	void  CloseConnection();
	bool  Send(char *, int);
	bool Receive(char *,int);
};

#endif /* UDPSERVER_H_ */
