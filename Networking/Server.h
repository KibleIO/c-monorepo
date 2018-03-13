#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/tcp.h>
#include "Networking.h"

class Server {
private:
	bool        startedUp;
	int         mainSocket;
	sockaddr_in destination;
public:
	Server();
	~Server();
	bool Init();
	bool Listen(int);
	void  CloseConnection();
	bool  Send(char *, int);
	bool Receive(char *,int);
};

#endif /* SERVER_H_ */
