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
#include <Utilities/SODIUM_WRAPPER.h>
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

bool Initialize_X264_ENCODER(X264_ENCODER*, SCREEN_DIM, int);
int32_t Encode_Headers_X264_ENCODER(X264_ENCODER*);
int32_t Encode_Frame2_X264_ENCODER(X264_ENCODER*);
int32_t Encode_Frame_X264_ENCODER(X264_ENCODER*, uint8_t*);
int32_t Encode_Frame_No_Convert_X264_ENCODER(X264_ENCODER*, uint8_t*);
void Delete_X264_ENCODER(X264_ENCODER*);

#endif
