#ifndef CLIENT_H_
#define CLIENT_H_

#include "Networking.h"
#include <Utilities/LOGGING.h>

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
	bool Receive_b(char *,int);
};


#endif /* CLIENT_H_ */
