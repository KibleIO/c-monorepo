#ifndef _THEMIS_HTTP_SERVER_H_
#define _THEMIS_HTTP_SERVER_H_

#include <thread>
#include <Utilities/KCONTEXT.h>
#include <Utilities/TIMER.h>
#include "../THEMIS_EXT.h"
#include "video/VIDEO_SERVER.h"

//oh god... check here for explanation: https://stackoverflow.com/questions/57008541/how-to-avoid-a-globally-defined-c-macro-of-status-from-xlib-h
#if defined (Status)
# undef Status
typedef int Status;
#endif

#include <pb/THEMIS_SERVER.h>

struct THEMIS_HTTP_SERVER {
	pb::THEMIS_SERVER server;
	thread *launch_thread;
	KCONTEXT *ctx;
	THEMIS_EXT themis_ext;
};

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*, KCONTEXT*, int);
bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*, KCONTEXT*, int,
	std::string);
void Run_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*);
void Delete_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*);

#endif