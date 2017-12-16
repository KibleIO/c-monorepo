#ifndef CLIENT_H_
#define CLIENT_H_

#include "Networking.h"

class Client {
private:
	bool        startedUp; // Remove this, should be managed externally
	int         mainSocket;
	sockaddr_in destination;
public:
	Client();
	bool  Init();
	bool  OpenConnection(int, string);
	void  CloseConnection();
	bool  Send(char *, int);
	bool Receive(char *,int);
};


#endif /* CLIENT_H_ */
