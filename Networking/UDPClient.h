#ifndef UDPCLIENT_H_
#define UDPCLIENT_H_

#include "Networking.h"

class UDPClient {
private:
	bool        startedUp; // Remove this, should be managed externally
	int         mainSocket;
	sockaddr_in destination;
public:
	UDPClient();
	bool  Init();
	bool  OpenConnection(int, string);
	void  CloseConnection();
	bool  Send(char *, int);
	bool Receive(char *,int);
};


#endif /* UDPCLIENT_H_ */
