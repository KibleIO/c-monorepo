#include "EVENT.h"

void   Initialize_Event(EVENT* event) { event->Changed = false; event->count_down = 0; }

bool   Check_Event(EVENT* event) {
	bool ean; //lol ;)
	event->Mutex.lock();
	ean = event->Changed;
	if (event->Changed && --event->count_down <= 0) {
		event->Changed = false;
	}
	event->Mutex.unlock();
	return ean;
}

void   Set_Event(EVENT* event) {
	event->Mutex.lock();
	event->Changed = true;
	event->count_down = 3;
	event->Mutex.unlock();
}
