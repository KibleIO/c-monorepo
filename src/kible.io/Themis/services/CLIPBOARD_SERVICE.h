#ifndef CLIPBOARD_SERVICE_H_
#define CLIPBOARD_SERVICE_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Hermes/SERVER.h>
#include <clip.h>

#define MAX_CLIPBOARD_SIZE 256

struct CLIPBOARD_SERVICE {
	KCONTEXT *ctx;
	SERVER *s;
	thread *main_loop;
	bool main_loop_running;
};

bool Initialize_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE*, KCONTEXT*);
void Main_Loop_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE*);
bool Connect_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE*, SERVER*);
void Disconnect_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE*);
void Delete_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE*);

#endif
