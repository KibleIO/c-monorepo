#ifndef  X264_DECODE_H_
#define  X264_DECODE_H_

#include <iostream>
#include <Utilities/Timer.h>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavutil/frame.h>
	#include <libavutil/opt.h>
}

using namespace std;

struct X264_Decode {
	AVPacket *pkt;
    AVCodecParserContext *parser;
    const AVCodec *codec;
    AVCodecContext *c;
    AVFrame *frame;
    int width, height;
};

void X264_Decode_Initialize(X264_Decode*, int, int);
void X264_Decode_Decode_To_Frame_Buffer(X264_Decode*, char*, int, char*);
void X264_Decode_Decode_To_Frame_Buffer(X264_Decode*, char*, int, char*, char*, int);
void X264_Decode_Convert_gbr24p_to_packed32(const uint8_t**, uint32_t*, int, int);
void X264_Decode_Convert_gbr24p_to_packed32(uint8_t**, int*, uint8_t*, int, int);
void X264_Decode_Convert_gbr24p_to_packed32(uint8_t**, int*, uint8_t*, int, int, char*, int);
void X264_Decode_Core_Decode(X264_Decode*, AVCodecContext*, AVFrame*, AVPacket*, char*);
void X264_Decode_Core_Decode(X264_Decode*, AVCodecContext*, AVFrame*, AVPacket*, char*, char*, int);
void X264_Decode_Delete(X264_Decode*);

#endif
