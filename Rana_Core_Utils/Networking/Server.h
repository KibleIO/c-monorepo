//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <thread>
#include "Networking.h"
#include "../Utilities/TIMER.h"

class Server {
private:
	string name;

	ENCRYPTION_PROFILE* enc;
	char*				enc_buf_auth;
	char*				enc_buf_data;

	bool				connected;
	int					c_port;

	timeval timeout;
	int32_t high_priority;

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
	void Set_Name(string _name);
	void Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc);
	void Set_Recv_Timeout(int seconds, int useconds = 0);
	void Set_High_Priority();
	void Set_Opts();
	bool Bind(int port);
	bool ListenBound();
	bool Listen(int);
	void CloseConnection();
	bool Send(char*, int);
	bool Receive(char*, int);
	~Server();
};

#endif /* SERVER_H_ */
