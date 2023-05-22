#ifndef _X264_ENCODER_H
#define _X264_ENCODER_H

#include <iostream>
#include <queue>
#include <stdint.h>
#include <thread>
#include <cstring>
#include <mutex>
#include <Utilities/LOGGING.h>
#include <Utilities/RGB2YUV.h>
#include <Utilities/SCREEN_DIM.h>
#include <Utilities/TIMER.h>
#include <Networking/NETWORK.h>

extern "C" {
	#include <x264.h>
	#include <libswscale/swscale.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/frame.h>
	#include <libavutil/opt.h>
	#include <libavcodec/version.h>
	#include <libavformat/avformat.h>
	#include <libavutil/hwcontext.h>
	#include <libavutil/imgutils.h>
}

using namespace std;

/*

Firstly, let me be clear. I have almost no idea what any of these params mean.
But based on my limited understanding of what these params mean, I will try to
explain these fields in ENCODE_LEVEL.

max_kbs -> ok, so this is the target max kilobits (yes, kilobits, not kilobytes)
that the stream will attempt to use. Generally, your average usage when watching
a youtube video will be around half this. I have observed it exceeding this
number by a bit when playing staggering beauty.

rf_constant -> this refers to the quality of the stream. So the lower this
number is the higher quality the stream will be. This will be a secondary
consideration behind max_kbs. This value ranges from 0 to 51. 51 being just a 
blob and 0 being completely lossless. The sane values for this number are 15 to
30.

vbv_buffer_size -> this one is a bit of a mystery to me. But generally what it
means is the variability in the bit rate. The larger this number is the more
I notice the average bit rate will vary, and the lower this number is the lower
the variability is. I generally keep this number low.

I guess this article is decent abou h264
https://slhck.info/video/2017/03/01/rate-control.html

*/
struct ENCODE_LEVEL {
	int max_kbs;
	int rf_constant;
	int vbv_buffer_size;
};

#define ENCODE_LEVEL_HIGH (ENCODE_LEVEL){10000, 15, 1000}
#define ENCODE_LEVEL_MEDIUM (ENCODE_LEVEL){7500, 20, 500}
#define ENCODE_LEVEL_LOW (ENCODE_LEVEL){2500, 25, 250}
#define ENCODE_LEVEL_PLACEBO (ENCODE_LEVEL){100, 45, 1}

struct X264_ENCODER {
	mutex 			mtx;
	SCREEN_DIM screen_dim;
	int32_t index;
	int32_t i_nals;

	x264_t*			encoder;
	x264_param_t	parameters;
	x264_picture_t	picture_in;
	x264_picture_t	picture_out;
	x264_nal_t*	nals;

	uint8_t*	yuvbasef;
	uint8_t*	yuvbaseb;
	uint8_t*	nalu_process_buff;
	uint32_t ysize;
	uint32_t uvsize;

	SwsContext *resize;

	AVFrame* frame1;
	AVFrame* frame2;

	RGB2YUV converter;
	int pts;
};

bool Initialize_X264_ENCODER(X264_ENCODER*, SCREEN_DIM, ENCODE_LEVEL, int);
int32_t Encode_Headers_X264_ENCODER(X264_ENCODER*);
int32_t Encode_Frame2_X264_ENCODER(X264_ENCODER*);
int32_t Encode_Frame_X264_ENCODER(X264_ENCODER*, uint8_t*);
int32_t Encode_Frame_No_Convert_X264_ENCODER(X264_ENCODER*, uint8_t*);
void Delete_X264_ENCODER(X264_ENCODER*);

#endif
