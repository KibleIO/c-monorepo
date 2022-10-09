#ifndef CLIENT_H_
#define CLIENT_H_

#include "../Networking/NETWORK.h"
#include "../Utilities/KCONTEXT.h"
#include "../Networking/TCP_CLIENT.h"
#include "../Networking/UDP_CLIENT.h"
#include "CLIENT_MASTER.h"

struct CLIENT {
	KCONTEXT *ctx;
	int type;
	CLIENT_MASTER *master;
	union {
		TCP_CLIENT tcp_client;
		UDP_CLIENT udp_client;
	};
};

/*
Yea this is pretty dirty, basically this is an abstract class implemented in
ANSI C. Truthfully we should just design Hermes in such a way that it didn't
rely on an abstract implementation of a CLIENT and SERVER. But it would make the
whole rest of the code base look kinda ugly, and it would hurt maintainability.
Also the impact on performance should be pretty minimal (hopefully) at the
benefit of massive bump in maintainability and stability.

Kord I know what you're thinking, if we are going to take the hit of performance
why didn't we go with the abstract class that c++ already has? Vtable look ups
arent that much slower than switch trees right? Probably... idk maybe we
redesign this sometime down the line.
*/

bool Initialize_CLIENT(CLIENT*, KCONTEXT*, CLIENT_MASTER*, int);
void Set_Name_CLIENT(CLIENT*, char*);
bool Set_Recv_Timeout_CLIENT(CLIENT*, int, int);
bool Set_High_Priority_CLIENT(CLIENT*);
bool Connect_CLIENT(CLIENT*);
bool Send_CLIENT(CLIENT*, char*, int);
bool Receive_CLIENT(CLIENT*, char*, int);
int Receive_Unsafe_CLIENT(CLIENT*, char*);
void Delete_CLIENT(CLIENT*);

#endif
