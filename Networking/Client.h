#ifndef CLIENT_H_
#define CLIENT_H_



#include "Networking.h"
#include "ENCRYPTION_ENGINE.h"
#include <Utilities/LOGGING.h>

class Client {
private:
	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;
	bool        startedUp; // Remove this, should be managed externally
	int         mainSocket;
	sockaddr_in destination;
public:
	Client();
	bool Init();
	bool OpenConnection(int, string);
	void CloseConnection();
	bool Send(char *, int);
	bool Receive(char *,int);
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
	//bool Receive_b(char *,int);
};


#endif /* CLIENT_H_ */
