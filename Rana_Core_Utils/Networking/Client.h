#ifndef CLIENT_H_
#define CLIENT_H_

#ifdef __linux__
#include <netdb.h>
#endif

#include <string>
#include <string.h>
#include "Networking.h"

class Client {
private:
	string name;

	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;

	bool				connected;

	timeval timeout;

#ifdef __linux__
	int cSocket;
#endif
#ifdef _WIN64
	SOCKET cSocket;
#endif

public:
	Client();
	void Init();
	void Set_Name(string _name);
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
	void Set_Recv_Timeout(int seconds, int useconds = 0);
	void Set_High_Priority();
	bool OpenConnection(int, string);
	void CloseConnection();
	bool Send(char *, int);
	bool Receive(char *,int);
	~Client();
};

#endif /* CLIENT_H_ */
