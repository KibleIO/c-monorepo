//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef SERVER_H_
#define SERVER_H_

#include "Networking.h"

class Server {
private:
	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;

	bool				connected;
	int					c_port;

	// Linux specific members {{{
	#ifdef __linux__
	int lSocket;
	int cSocket;
	#endif
	// }}} Windows specific members {{{
	#ifdef _WIN64
	SOCKET lSocket;
	SOCKET cSocket;
	#endif
	// }}} OSX specific members {{{
	#ifdef __APPLE__
	//TODO apple sockets
	#endif
	// }}}

public:
	Server();
	void Init();
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
	bool Bind(int port);
	bool ListenBound();
	bool Listen(int);
	void CloseConnection();
	bool Send(char*, int);
	bool Receive(char*, int);
	~Server();
};

#endif /* SERVER_H_ */
