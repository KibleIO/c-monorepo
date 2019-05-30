//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef TIMER_H_
#define TIMER_H_

#include "LOGGING.h"

#ifdef __linux__
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <cstdlib>
#endif
#ifdef _WIN64
#include <windows.h>
#endif

struct TIMER {
#ifdef __linux__
	timeval start, end;
	long seconds, useconds;
#endif
#ifdef _WIN64
	double PCFreq;
	__int64 CounterStart;
#endif
};

bool Initialize_TIMER(TIMER*);
void Start_TIMER(TIMER*);
long Stop_TIMER(TIMER*);
void Sleep_Milli(unsigned int);
void Delete_TIMER(TIMER*);

#endif /* TIMER_H_ */

