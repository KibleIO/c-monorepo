//PLATFORMS: Linux, Windows, OSX

#ifndef  EVENT_H_
#define  EVENT_H_

#include <mutex>

using namespace std;

struct EVENT {
	bool  Changed;
	int   count_down;
	mutex Mutex;
};

void Initialize_Event(EVENT*);
bool Check_Event(EVENT*);
void Set_Event(EVENT*);
void Delete_Event(EVENT*);


#endif
