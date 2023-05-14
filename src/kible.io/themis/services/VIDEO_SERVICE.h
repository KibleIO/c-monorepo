#ifndef VIDEO_SERVICE_H_
#define VIDEO_SERVICE_H_

#include <thread>
#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/FPS_LIMITER.h>
#include <Hermes/SERVER.h>
#include "../xvfb/XVFB_HANDLER.h"
#include "../live444/X264_ENCODER.h"

#define VIDEO_FPS 60
#define NAL_MAX_SIZE 100000
#define MAX_LOOPS_RESIZE 100

struct VIDEO_SERVICE {
	KCONTEXT *ctx;
	XVFB_HANDLER xvfb;
	X264_ENCODER encoder;
	SERVER *s;
	FPS_LIMITER fps_limiter;
	thread *main_loop;
	volatile bool main_loop_running;
	volatile bool resize;
	int width;
	int height;
	char nal_unit[NAL_MAX_SIZE];
};

bool Initialize_VIDEO_SERVICE(VIDEO_SERVICE*, KCONTEXT*);
bool Connect_VIDEO_SERVICE(VIDEO_SERVICE*, SERVER*, SERVER*);
void Main_TCP_Loop_VIDEO_SERVICE(VIDEO_SERVICE*);
void Main_UDP_Loop_VIDEO_SERVICE(VIDEO_SERVICE*);
bool Resize_VIDEO_SERVICE(VIDEO_SERVICE*, int, int);
void Disconnect_VIDEO_SERVICE(VIDEO_SERVICE*);
void Delete_VIDEO_SERVICE(VIDEO_SERVICE*);

#endif
