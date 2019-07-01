#ifndef _YUV2RGB_H
#define _YUV2RGB_H

#include <thread>
#include <Utilities/LOGGING.h>

#define YUV2RGB_ALGO SWS_BILINEAR

#define YUV2RGB_MAX_THREADS 16

using namespace std;

extern "C" {
	#include <libswscale/swscale.h>
}

struct YUV2RGB {
	uint32_t threads;
	SwsContext* sws[YUV2RGB_MAX_THREADS];
	uint32_t yuv_slice_i[YUV2RGB_MAX_THREADS][3];
	uint32_t slice_y[YUV2RGB_MAX_THREADS];
	uint32_t slice_height;
	int32_t yuv_strides[3];
	int32_t rgb_strides[1];
	uint32_t ysize;
	uint32_t uvsize;
};

bool Initialize_YUV2RGB(
YUV2RGB* trans, uint32_t width, uint32_t height, uint32_t _threads = 1,
uint8_t padding = true);

void Convert_YUV2RGB(YUV2RGB* trans, uint8_t* in, uint8_t* out);

void Convert_YUV2RGB(YUV2RGB* trans, uint8_t** in, uint8_t* out);

void Delete_YUV2RGB(YUV2RGB* trans);

#endif
