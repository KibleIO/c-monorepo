#ifndef KEYBOARD_SERVICE_H_
#define KEYBOARD_SERVICE_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/KEYBOARD.h>
#include <Utilities/C_ARRAY.h>
#include <Hermes/SERVER.h>

struct KEYBOARD_SERVICE {
	KCONTEXT *ctx;
	SERVER *s;
	thread *main_loop;
	bool main_loop_running;
};

bool Initialize_KEYBOARD_SERVICE(KEYBOARD_SERVICE*, KCONTEXT*);
void Main_Loop_KEYBOARD_SERVICE(KEYBOARD_SERVICE*);
bool Connect_KEYBOARD_SERVICE(KEYBOARD_SERVICE*, SERVER*);
void Disconnect_KEYBOARD_SERVICE(KEYBOARD_SERVICE*);
void Delete_KEYBOARD_SERVICE(KEYBOARD_SERVICE*);

#endif
