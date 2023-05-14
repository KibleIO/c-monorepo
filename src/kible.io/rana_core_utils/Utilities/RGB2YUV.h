#ifndef _RGB2YUV_H
#define _RGB2YUV_H

#include <thread>
#include <Utilities/LOGGING.h>

#define RGB2YUV_ALGO SWS_BILINEAR

#define RGB2YUV_MAX_THREADS 16

using namespace std;

extern "C" {
	#include <libswscale/swscale.h>
}

struct RGB2YUV {
	uint32_t threads;
	SwsContext* sws[RGB2YUV_MAX_THREADS];
	uint32_t yuv_slice_i[RGB2YUV_MAX_THREADS][3];
	uint32_t slice_y[RGB2YUV_MAX_THREADS];
	uint32_t slice_height;
	uint32_t width;
	uint32_t height;
	int32_t yuv_strides[3];
	int32_t rgb_strides[1];
	uint32_t ysize;
	uint32_t uvsize;
};

bool Initialize_RGB2YUV(
RGB2YUV* trans, uint32_t width, uint32_t height, uint32_t _threads = 1,
uint8_t padding = true);

void Convert_RGB2YUV(RGB2YUV* trans, uint8_t* in, uint8_t* out);

void Delete_RGB2YUV(RGB2YUV* trans);

#endif
