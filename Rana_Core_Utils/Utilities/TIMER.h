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
#ifdef __APPLE__
#include <time.h>

#include <net/if.h>
#include <mach/clock.h>
#include <mach/mach.h>
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
#ifdef __APPLE__
	struct timespec start, end;
	long seconds, useconds;
#endif
};

bool Initialize_TIMER(TIMER*);
void Start_TIMER(TIMER*);
long Stop_TIMER(TIMER*);
void Delete_TIMER(TIMER*);

#endif /* TIMER_H_ */
