#ifndef SERVER_H_
#define SERVER_H_

#include <string.h>
#include <netinet/tcp.h>
#include "Networking.h"
#include "ENCRYPTION_ENGINE.h"
#include <Utilities/LOGGING.h>

class Server {
private:
	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;
	bool        startedUp;
	int         mainSocket;
	int			clientSocket;
	sockaddr_in destination;
public:
	Server();
	~Server();
	bool Init();
	bool Bind(int port);
	bool ListenBound();
	bool Listen(int);
	void CloseConnection();
	bool Send(char *, int);
	bool Receive(char *,int);
	//bool ReceiveNonBlocking(char *data, int size);
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
};

#endif /* SERVER_H_ */
