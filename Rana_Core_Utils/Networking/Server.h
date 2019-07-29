#ifndef SERVER_H_
#define SERVER_H_

#ifdef __linux__
#include <signal.h>
#endif

#include <string>
#include <thread>
#include <string.h>
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

#ifdef __linux__
	timeval o_to;
	int32_t o_pr;
	int lSocket;
	int cSocket;
#endif
#ifdef _WIN64
	DWORD o_to;
	SOCKET lSocket;
	SOCKET cSocket;
#endif

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
