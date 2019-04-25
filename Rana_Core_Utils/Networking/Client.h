//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef CLIENT_H_
#define CLIENT_H_

#ifdef __linux__
#include <netdb.h>
#endif

#include <string.h>
#include "Networking.h"

class Client {
private:
	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;

	bool				connected;

	// Linux specific members {{{
	#ifdef __linux__
	int cSocket;
	#endif
	// }}} Windows specific members {{{
	#ifdef _WIN64
	SOCKET cSocket;
	#endif
	// }}} OSX specific members {{{
	#ifdef __APPLE__
	//TODO apple sockets
	#endif
	// }}}

public:
	Client();
	void Init();
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
	void Set_Recv_Timeout(int seconds, int useconds = 0);
	bool OpenConnection(int, string);
	void CloseConnection();
	bool Send(char *, int);
	bool Receive(char *,int);
	~Client();
};

#endif /* CLIENT_H_ */
