#include "TIMER.h"

bool Initialize_TIMER(TIMER* timer) {
	(void)timer;
	return true;
}

void Start_TIMER(TIMER* timer) {
#ifdef __linux__
	gettimeofday(&timer->start, NULL);
#endif
#ifdef _WIN64
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);

	timer->PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	timer->CounterStart = li.QuadPart;
#endif
}

long Stop_TIMER(TIMER* timer) {
#ifdef __linux__
	gettimeofday(&timer->end, NULL);
	timer->seconds = timer->end.tv_sec - timer->start.tv_sec;
	timer->useconds = timer->end.tv_usec - timer->start.tv_usec;
	return ((timer->seconds) * 1000 + timer->useconds / 1000.0) + 0.5;
#endif
#ifdef _WIN64
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return long(double(li.QuadPart - timer->CounterStart) / timer->PCFreq);
#endif
}

void Delete_TIMER(TIMER* timer) {
	(void)timer;
}
