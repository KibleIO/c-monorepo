#include "TIMER.h"

bool Initialize_TIMER(TIMER* timer) {
	return true;
}

void Start_TIMER(TIMER* timer) {
	// Linux specific includes {{{
	#ifdef __linux__
	gettimeofday(&timer->start, NULL);
	#endif
	// }}} Windows specific includes {{{
	#ifdef _WIN64
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);

	timer->PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	timer->CounterStart = li.QuadPart;
	#endif
	// }}} OSX specific includes {{{
	#ifdef __APPLE__
	//TODO apple includes
	#endif
	// }}}
}

long Stop_TIMER(TIMER* timer) {
	// Linux specific includes {{{
	#ifdef __linux__
	gettimeofday(&timer->end, NULL);
	timer->seconds = timer->end.tv_sec - timer->start.tv_sec;
	timer->useconds = timer->end.tv_usec - timer->start.tv_usec;
	return ((timer->seconds) * 1000 + timer->useconds / 1000.0) + 0.5;
	#endif
	// }}} Windows specific includes {{{
	#ifdef _WIN64
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return long(double(li.QuadPart - timer->CounterStart) / timer->PCFreq);
	#endif
	// }}} OSX specific includes {{{
	#ifdef __APPLE__
	//TODO apple includes
	#endif
	// }}}
}

void Sleep_Milli_TIMER(TIMER* timer, unsigned int milli) {
	// Linux specific includes {{{
	#ifdef __linux__
	usleep(milli * 1000);
	#endif
	// }}} Windows specific includes {{{
	#ifdef _WIN64
	Sleep(milli);
	#endif
	// }}} OSX specific includes {{{
	#ifdef __APPLE__
	//TODO apple includes
	#endif
	// }}}
}

void Delete_TIMER(TIMER* timer) {}