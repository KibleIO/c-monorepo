#ifndef MOUSE_SERVICE_H_
#define MOUSE_SERVICE_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/MOUSE.h>
#include <Utilities/C_ARRAY.h>
#include <Hermes/SERVER.h>

struct MOUSE_SERVICE {
	KCONTEXT *ctx;
	SERVER *s;
	thread *main_loop;
	bool main_loop_running;
};

bool Initialize_MOUSE_SERVICE(MOUSE_SERVICE*, KCONTEXT*);
void Main_Loop_MOUSE_SERVICE(MOUSE_SERVICE*);
bool Connect_MOUSE_SERVICE(MOUSE_SERVICE*, SERVER*);
void Disconnect_MOUSE_SERVICE(MOUSE_SERVICE*);
void Delete_MOUSE_SERVICE(MOUSE_SERVICE*);

#endif
