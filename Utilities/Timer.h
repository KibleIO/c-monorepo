#ifndef TIMER_H_
#define TIMER_H_

#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <cstdlib>

using namespace std;

class Timer {
	private:
		timeval start, end;
		long seconds, useconds;
	public:
		Timer() {}
		void Start() { gettimeofday(&start, NULL); }
		long Stop() { gettimeofday(&end, NULL);
			seconds = end.tv_sec - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;
			return ((seconds) * 1000 + useconds / 1000.0) + 0.5; }
		void sleepMilli(unsigned int milli) { usleep(milli * 1000); }
};

#endif /* TIMER_H_ */