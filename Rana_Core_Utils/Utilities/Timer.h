//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef TIMER_H_
#define TIMER_H_

#include <stdio.h>
#include <stdlib.h>

// Linux specific includes {{{
#ifdef __linux__
#include <sys/time.h>
#include <unistd.h>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#include <stdint.h>
#include <Windows.h>
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif
// }}}

// Linux specific includes {{{
#ifdef __linux__
typedef timeval time_type;
typedef int64_t _int64;
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
struct time_type {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif

using namespace std;

class Timer {
	private:
		time_type start, end;
		long seconds, useconds;

		int get_time(time_type* tp, struct timezone* tzp) {
			// Linux specific code {{{
			#ifdef __linux__
			return gettimeofday(tp, tzp);
			#endif
			// }}} Windows specific code {{{
			#ifdef _WIN64
			const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

			SYSTEMTIME  system_time;
			FILETIME    file_time;
			uint64_t    time;

			GetSystemTime(&system_time);
			SystemTimeToFileTime(&system_time, &file_time);
			time = ((uint64_t)file_time.dwLowDateTime);
			time += ((uint64_t)file_time.dwHighDateTime) << 32;

			tp->tv_sec = (long)((time - EPOCH) / 10000000L);
			tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
			return 0;
			#endif
			// }}} OSX specific code {{{
			#ifdef __APPLE__
			//TODO apple code
			#endif
			// }}}
		}

		void sleep_u(_int64 usec) {
			// Linux specific code {{{
			#ifdef __linux__
			usleep(usec);
			#endif
			// }}} Windows specific code {{{
			#ifdef _WIN64
			HANDLE timer;
			LARGE_INTEGER ft;

			ft.QuadPart = -(10 * usec);

			timer = CreateWaitableTimer(NULL, TRUE, NULL);
			SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
			WaitForSingleObject(timer, INFINITE);
			CloseHandle(timer);
			#endif
			// }}} OSX specific code {{{
			#ifdef __APPLE__
			//TODO apple code
			#endif
			// }}}
		}

	public:
		Timer() {
		}

		void Start() {
			get_time(&start, NULL);
		}

		long Stop() {
			get_time(&end, NULL);
			seconds = end.tv_sec - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;
			return ((seconds) * 1000 + useconds / 1000.0) + 0.5; 
		}

		void sleepMilli(unsigned int milli) {
			sleep_u(milli * 1000);
		}
};

#endif /* TIMER_H_ */

