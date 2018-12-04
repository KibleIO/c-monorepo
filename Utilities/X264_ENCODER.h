#ifndef  X264_ENCODER_H_
#define  X264_ENCODER_H_

#include <cstring>
#include <iostream>
#include <thread>
#include <Utilities/Timer.h>
#include <Utilities/LOGGING.h>

#define RNDTO2(X) ( (X) & 0xFFFFFFFE )
#define RNDTO32(X) ( ( (X) % 32 ) ? ( ( (X) + 32 ) &  0xFFFFFFE0 ) : (X) )

#define CLAMP(X) ( (X) < 16 ? 16 : ( (X) > 235 ? 235 : (X) ) )

#define RGB2YUV_SHIFT 15
#define	BY ((int)( 0.098 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	BV ((int)(-0.071 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	BU ((int)( 0.439 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	GY ((int)( 0.504 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	GV ((int)(-0.368 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	GU ((int)(-0.291 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	RY ((int)( 0.257 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	RV ((int)( 0.439 * (1 << RGB2YUV_SHIFT) + 0.5))
#define	RU ((int)(-0.148 * (1 << RGB2YUV_SHIFT) + 0.5))

extern "C" {
	#include <x264.h>
}

extern "C" {
	#include <libswscale/swscale.h>
}

using namespace std;

struct Sws {
	SwsContext* context;
	int rgbstride[1];
	int yuvstride[4];
	void* yuv[4];
	void* yuvbasef;
	void* yuvbaseb;
	uint8_t* rgb[1];
};

struct X264_Encoder {
	x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t* h;
    x264_nal_t *nal;
    int i_nal;
    int i_frame = 0;
    int i_frame_size;
    int bitrate_kbps = 100;
    int width, height;
	Sws* sws;
};

void X264_Encoder_Initialize(X264_Encoder*, int, int, int);
int X264_Encoder_Get_Header(X264_Encoder*, char**);
int X264_Encoder_Encode_Frame_Buffer(X264_Encoder*, char*, char**);
void X264_Encoder_Delete(X264_Encoder*);

#endif
