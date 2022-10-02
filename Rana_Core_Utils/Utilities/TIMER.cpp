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
#ifdef __APPLE__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	timer->start.tv_sec = mts.tv_sec;
	timer->start.tv_nsec = mts.tv_nsec;
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
#ifdef __APPLE__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	timer->end.tv_sec = mts.tv_sec;
	timer->end.tv_nsec = mts.tv_nsec;

	timer->seconds = timer->end.tv_sec - timer->start.tv_sec;
	timer->useconds = (timer->end.tv_nsec - timer->start.tv_nsec) / 1000;

	return ((timer->seconds) * 1000 + timer->useconds / 1000.0) + 0.5;
#endif
}

void Delete_TIMER(TIMER* timer) { (void)timer; }

void set_interval_helper(void (*func)(void*), unsigned int interval,
						 void *data) {
	while (true) {
		func(data);
		Sleep_Milli(interval);
	}
}

void set_delay_helper(void (*func)(void*), unsigned int interval, void *data) {
	Sleep_Milli(interval);
	func(data);
}

void Set_Interval(void (*func)(void*), unsigned int interval, void *data) {
	std::thread(set_interval_helper, func, interval, data).detach();
}

void Set_Delay(void (*func)(void*), unsigned int interval, void *data) {
	std::thread(set_delay_helper, func, interval, data).detach();
}
