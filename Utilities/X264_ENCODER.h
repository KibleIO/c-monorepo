#ifndef  X264_ENCODER_H_
#define  X264_ENCODER_H_

#include <iostream>
#include <Utilities/Timer.h>

extern "C" {
	#include <x264.h>
}

using namespace std;

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
};

void X264_Encoder_Initialize(X264_Encoder*, int, int, int);
int X264_Encoder_Get_Header(X264_Encoder*, char**);
int X264_Encoder_Encode_Frame_Buffer(X264_Encoder*, char*, char**);
void X264_Encoder_Delete(X264_Encoder*);

#endif
