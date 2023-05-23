#ifndef VIDEO_SERVER_H_
#define VIDEO_SERVER_H_

#include <thread>
#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/FPS_LIMITER.h>
#include "XVFB_HANDLER.h"
#include "X264_ENCODER.h"
#include <controller/server/SERVICE_SERVER.h>

#define VIDEO_FPS 60
#define NAL_MAX_SIZE 100000
#define MAX_LOOPS_RESIZE 100

struct VIDEO_SERVER : public SERVICE_SERVER {
	KCONTEXT *ctx;
	XVFB_HANDLER xvfb;
	X264_ENCODER encoder;
	FPS_LIMITER fps_limiter;
	ENCODE_LEVEL encode_level;
	thread *main_loop;
	volatile bool main_loop_running;
	volatile bool resize;
	int width;
	int height;
	char nal_unit[NAL_MAX_SIZE];

	SOCKET_SERVER socket_server;

	bool Initialize(KCONTEXT*, SERVICE_SERVER_REGISTRY*) override;
	void Delete() override;
};

void Main_VIDEO_SERVER(VIDEO_SERVER*);
bool Resize_VIDEO_SERVER(VIDEO_SERVER*, int, int, ENCODE_LEVEL);

#endif
