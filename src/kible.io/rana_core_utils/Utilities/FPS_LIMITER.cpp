//PLATFORMS: Linux, Windows, OSX

#include "FPS_LIMITER.h"

bool Initialize_FPS_LIMITER(
FPS_LIMITER* fps_limiter, uint32_t fps, uint8_t verbose) {
	fps_limiter->frame_count = 0;
	fps_limiter->frame_rate = 0;
	fps_limiter->fps = fps;
	fps_limiter->verbose = verbose;

	return true;
}

void Start_FPS_LIMITER(FPS_LIMITER* fps_limiter) {
	Start_TIMER(&fps_limiter->frame_timer);
}

void Stop_FPS_LIMITER(FPS_LIMITER* fps_limiter) {
	fps_limiter->frame_time = Stop_TIMER(&fps_limiter->frame_timer);
	if (fps_limiter->verbose) {
		cout << fps_limiter->frame_time << endl;
	}
	if (MILLISECONDS_IN_A_SECOND / fps_limiter->fps > fps_limiter->frame_time) {
		Sleep_Milli(
		MILLISECONDS_IN_A_SECOND / fps_limiter->fps - fps_limiter->frame_time);
	}
	if (fps_limiter->frame_count < 30) {
		fps_limiter->frame_count++;
		fps_limiter->frame_rate += fps_limiter->frame_time;
	} else {
		fps_limiter->frame_rate /= 30;
		if (fps_limiter->verbose) {
			/*
			log_dbg(((const JSON_TYPE){
				{"message", "average frame"},
				{"rate", "to_string(fps_limiter->frame_rate).c_str()"},
				JSON_TYPE_END}));
			*/
		}
		fps_limiter->frame_count = 0;
		fps_limiter->frame_rate = 0;
	}
}

void Delete_FPS_LIMITER(FPS_LIMITER* fps_limiter) {
	(void)fps_limiter;
}
