#ifndef SESSION_H_
#define SESSION_H_

#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <string>

using namespace std;

class Session {
	private:
		bool connected;
		ssh_session ssh;
	public:
		string username;
		Session();
		~Session();
		bool Connect(string, string, string, int*);
		string runCommand(string);
		string runCommand_(string);
};

#endif /* SESSION_H_ */
