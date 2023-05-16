#ifndef _THEMIS_HTTP_SERVER_H_
#define _THEMIS_HTTP_SERVER_H_

#include <thread>
#include <Utilities/TIMER.h>
#include "../THEMIS_EXT.h"
#include "../pb/THEMIS_SERVER.h"

struct THEMIS_HTTP_SERVER {
	pb::THEMIS_SERVER server;
	THEMIS_EXT *themis_ext;
	thread *launch_thread;
};

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*, THEMIS_EXT*, int);
bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*, THEMIS_EXT*, int,
	std::string);
void Run_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*);
void Delete_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER*);

#endif