//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef TIMER_H_
#define TIMER_H_

// Linux specific includes {{{
#ifdef __linux__
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <cstdlib>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#include <windows.h>
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif
// }}}

struct TIMER {
// Linux specific includes {{{
#ifdef __linux__
	timeval start, end;
	long seconds, useconds;
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
	double PCFreq;
	__int64 CounterStart;
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif
// }}}
};

bool Initialize_TIMER(TIMER*);
void Start_TIMER(TIMER*);
long Stop_TIMER(TIMER*);
void Sleep_Milli_TIMER(TIMER*, unsigned int);
void Delete_TIMER(TIMER*);

#endif /* TIMER_H_ */

