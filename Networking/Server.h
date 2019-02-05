#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/tcp.h>
#include "Networking.h"
#include <Utilities/LOGGING.h>

class Server {
private:
	bool        startedUp;
	int         mainSocket;
	sockaddr_in destination;
public:
	Server();
	~Server();
	bool Init();
	bool Bind(int port);
	bool ListenBound();
	bool Listen(int);
	void  CloseConnection();
	bool  Send(char *, int);
	bool Receive(char *,int);
	bool ReceiveNonBlocking(char *data, int size);
};

#endif /* SERVER_H_ */
