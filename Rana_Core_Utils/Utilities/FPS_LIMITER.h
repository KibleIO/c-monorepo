//PLATFORMS: Linux, Windows, OSX

#ifndef FPS_LIMITER_H_
#define FPS_LIMITER_H_

#include <iostream>
#include "TIMER.h"
#include "LOGGING.h"

#define MILLISECONDS_IN_A_SECOND 1000

using namespace std;

struct FPS_LIMITER {
	uint32_t	fps;
	uint8_t		verbose;
	uint64_t	frame_time;
	TIMER		frame_timer;
	double		frame_rate;
	uint32_t	frame_count;
};

void Initialize_FPS_LIMITER(FPS_LIMITER*, uint32_t, uint8_t = false);
void Start_FPS_LIMITER(FPS_LIMITER*);
void Stop_FPS_LIMITER(FPS_LIMITER*);
void Delete_FPS_LIMITER(FPS_LIMITER*);

#endif
