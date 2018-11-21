#ifndef  X264_DECODE_H_
#define  X264_DECODE_H_

#include <iostream>
#include <Utilities/Timer.h>
#include <Utilities/LOGGING.h>

#define RNDTO2(X) ( (X) & 0xFFFFFFFE )
#define RNDTO32(X) ( ( (X) % 32 ) ? ( ( (X) + 32 ) &  0xFFFFFFE0 ) : (X) )

#define CLAMP(X) ( (X) < 0 ? 0 : ( (X) > 255 ? 255 : (X) ) )

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavutil/frame.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}

using namespace std;

struct Sws {
	SwsContext* context;
	int rgbstride[1];
	int yuvstride[4];
	void* yuv[4];
	void* yuvbase;
	uint8_t* rgb[1];
};

struct X264_Decode {
	AVPacket *pkt;
    AVCodecParserContext *parser;
    const AVCodec *codec;
    AVCodecContext *c;
    AVFrame *frame;
    int width, height;
	Sws* sws;
};

void X264_Decode_Initialize(X264_Decode*, int, int);
void X264_Decode_Decode_To_Frame_Buffer(X264_Decode*, char*, int, char*);
void X264_Decode_Decode_To_Frame_Buffer(X264_Decode*, char*, int, char*, char*, int);
void X264_Decode_Convert_yuv_to_packed32(const uint8_t** src, uint8_t* dst, int width, int height);
void X264_Decode_Convert_gbr24p_to_packed32(const uint8_t**, uint32_t*, int, int);
void X264_Decode_Convert_gbr24p_to_packed32(uint8_t**, int*, uint8_t*, int, int);
void X264_Decode_Convert_gbr24p_to_packed32(uint8_t**, int*, uint8_t*, int, int, char*, int);
void X264_Decode_Core_Decode(X264_Decode*, AVCodecContext*, AVFrame*, AVPacket*, char*);
void X264_Decode_Core_Decode(X264_Decode*, AVCodecContext*, AVFrame*, AVPacket*, char*, char*, int);
void X264_Decode_Delete(X264_Decode*);

#endif
