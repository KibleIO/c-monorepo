#include "X264_DECODE.h"

void Sws_setup(X264_Decode* x264) {
	x264->sws = new Sws;
	x264->sws->context = NULL;
	x264->sws->rgb[0] = new uint8_t[x264->width*x264->height*4];
	log_dbg("allocated swsrgb");
	x264->sws->context = sws_getCachedContext(x264->sws->context,
		x264->width, x264->height, AV_PIX_FMT_YUV420P,
		x264->width, x264->height, AV_PIX_FMT_BGRA, 
		0, 0, 0, 0);
	log_dbg("got sws context");
	x264->sws->rgbstride[0] = 4 * x264->width;

	int width = RNDTO2(x264->width);
	int height = RNDTO2(x264->height);
	int ystride = RNDTO32(width);
	int uvstride = RNDTO32(width/2);
	int ysize = ystride * height;
	int vusize = uvstride * (height/2);
	int size = ysize + (2*vusize);

	x264->sws->yuvbase = new char[size];
	x264->sws->yuv[0] = x264->sws->yuvbase;
	x264->sws->yuv[1] = x264->sws->yuvbase+ysize;
	x264->sws->yuv[2] = x264->sws->yuvbase+ysize+vusize;
	x264->sws->yuv[3] = 0;
	x264->sws->yuvstride[0] = ystride;
	x264->sws->yuvstride[1] = uvstride;
	x264->sws->yuvstride[2] = uvstride;
	x264->sws->yuvstride[3] = 0;
	log_dbg("SWScale setup complete");
}
void X264_Decode_Initialize(X264_Decode* x264, int w, int h) {
	x264->width  = w;
	x264->height = h;

	avcodec_register_all();
    x264->pkt = av_packet_alloc();
    if (!x264->pkt) {
        cout << "failed to initialize packet" << endl;
        return;
    }
    x264->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!x264->codec) {
        cout << "failed to initialize codec" << endl;
        return;
    }
    x264->parser = av_parser_init(x264->codec->id);
    if (!x264->parser) {
        cout << "failed to initialize parser" << endl;
        return;
    }
    x264->c = avcodec_alloc_context3(x264->codec);
    if (!x264->c) {
        cout << "failed to initialize context" << endl;
        return;
    }
    /*
    av_opt_set(x264->c->priv_data, "preset", "ultrafast", 0);

	av_opt_set(x264->c->priv_data, "g", "30", 0);
	*/
	av_opt_set(x264->c->priv_data, "tune", "zerolatency+fastdecode", 0);
	x264->c->thread_count = 1;
    if (avcodec_open2(x264->c, x264->codec, NULL) < 0) {
        cout << "failed to open codec" << endl;
        return;
    }
    x264->frame = av_frame_alloc();
    if (!x264->frame) {
        cout << "failed to allocate frame" << endl;
        return;
    }

	Sws_setup(x264);
}

void X264_Decode_Decode_To_Frame_Buffer(X264_Decode* x264, char* x264_buff, int x264_buff_size, char* fbp, char* index_arr, int index_arr_size) {
	int len;
	while(x264_buff_size) {
		len = av_parser_parse2(x264->parser, x264->c, &x264->pkt->data, &x264->pkt->size, (const uint8_t*) x264_buff, x264_buff_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
		
		if (len < 0) {
	        cout << "error parsing!" << endl;
	        return;
	    }
		
		x264_buff += len;
		x264_buff_size  -= len;
		
		if(x264->pkt->size) {
			X264_Decode_Core_Decode(x264, x264->c, x264->frame, x264->pkt, fbp, index_arr, index_arr_size);
		}
	}
	len = av_parser_parse2(x264->parser, x264->c, &x264->pkt->data, &x264->pkt->size, NULL, 0, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
	if (len < 0) {
	    cout << "error parsing!" << endl;
	    return;
	}
	if(x264->pkt->size) {
		X264_Decode_Core_Decode(x264, x264->c, x264->frame, x264->pkt, fbp, index_arr, index_arr_size);
	}
}
void X264_Decode_Decode_To_Frame_Buffer(X264_Decode* x264, char* x264_buff, int x264_buff_size, char* fbp) {
	int len;
	while(x264_buff_size) {
		len = av_parser_parse2(x264->parser, x264->c, &x264->pkt->data, &x264->pkt->size, (const uint8_t*) x264_buff, x264_buff_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (len < 0) {
	        cout << "error parsing!" << endl;
	        return;
	    }

		x264_buff += len;
		x264_buff_size  -= len;

		if(x264->pkt->size) {
			X264_Decode_Core_Decode(x264, x264->c, x264->frame, x264->pkt, fbp);
		}
	}
	len = av_parser_parse2(x264->parser, x264->c, &x264->pkt->data, &x264->pkt->size, NULL, 0, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
	if (len < 0) {
		cout << "error parsing!" << endl;
		return;
	}
	if(x264->pkt->size) {
		X264_Decode_Core_Decode(x264, x264->c, x264->frame, x264->pkt, fbp);
	}
}

void X264_Decode_Core_Decode(X264_Decode* x264, AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, char* fbp, char* index_arr, int index_arr_size) {
    //char buf[1024];
    int ret;
	Timer tm1;
	long tv1;

    ret = avcodec_send_packet(dec_ctx, x264->pkt);
    if (ret < 0) {
        cout << "error sending packet1 " << ret << endl;
        return;
    }
    
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, x264->frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        	//cout << "ERROR!! " << ret << endl; //pretty sure this isn't an error
            break;
        } else if (ret < 0) {
            cout << "error on decode" << endl;
            break;
        }
		tm1.Start();
        X264_Decode_Convert_gbr24p_to_packed32(x264->frame->data, x264->frame->linesize, (uint8_t*) fbp, x264->width, x264->height, index_arr, index_arr_size);
		tv1 = tm1.Stop();
		log_dbg("conversion took " + to_string(tv1) + " ms");
    }
}

void X264_Decode_Core_Decode(X264_Decode* x264, AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, char* fbp) {
    //char buf[1024];
    int ret;
	Timer tm1;
	long tv1;

    ret = avcodec_send_packet(dec_ctx, x264->pkt);
    if (ret < 0) {
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, x264->frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        	//cout << "ERROR!! " << ret << endl; //pretty sure this isn't an error
			//log_dbg("error on decode " + to_string(ret));
            break;
        } else if (ret < 0) {
            log_dbg("error on decode");
            break;
        }
		tm1.Start();
        //X264_Decode_Convert_yuv_to_packed32(const_cast<const uint8_t**>(x264->frame->data), (uint8_t*) fbp, x264->width, x264->height);
		x264->sws->rgb[0] = (uint8_t*)fbp;
		sws_scale(x264->sws->context, const_cast<const uint8_t**>(x264->frame->data),
			x264->sws->yuvstride, 0, x264->height,
			(uint8_t* const*)x264->sws->rgb, x264->sws->rgbstride);
		tv1 = tm1.Stop();
		log_dbg("convert took " + to_string(tv1) + " ms");
    }
}

void X264_Decode_Convert_gbr24p_to_packed32(uint8_t** src, int* srcStride, uint8_t *dst, int width, int height, char* index_arr, int index_arr_size) {
	unsigned int di = 0;
    unsigned int bi;
    unsigned int bl;
    
    int out_size = 0;
    unsigned int index = 0;
    unsigned int length = 0;
	
	
	while (di < index_arr_size) {
		if (*((unsigned char*)&index_arr[di]) % 2 != 0) {
			bi = *((unsigned int*)&index_arr[di]) - 1;
        	di += sizeof(unsigned int);
		} else {
			bi = *((unsigned char*)&index_arr[di]) + index + length;
			di += sizeof(unsigned char);
		}
		index = bi;
		if (*((unsigned char*)&index_arr[di]) % 2 != 0) {
			bl = *((unsigned int*)&index_arr[di]) - 1;
        	di += sizeof(unsigned int);
		} else {
			bl = *((unsigned char*)&index_arr[di]);
			di += sizeof(unsigned char);
		}
		length = bl;
		
		for (int i = bi; i < bi + bl; i++) {
			((int*) dst)[i / 4] = src[1][i / 4] + (src[0][i / 4] << 8) + (src[2][i / 4] << 16) + (0xff << 24);
		}
	}
}

void X264_Decode_Convert_yuv_to_packed32(const uint8_t** src, uint8_t* dst, int width, int height) {
	const uint8_t* y = src[0];
	const uint8_t* u = src[1];
	const uint8_t* v = src[2];
	uint8_t r;
	uint8_t g;
	uint8_t b;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int yy = y[(j*width)+i];
			int uu = u[((j/2) * (width/2)) +(i/2)];
			int vv = v[((j/2) * (width/2)) +(i/2)];

			r = 1.164 * (yy - 16) + 1.596 * (vv - 128);
			g = 1.164 * (yy - 16) - 0.813 * (vv - 128) - 0.391 * (uu - 128);
			b = 1.164 * (yy - 16) + 2.018 * (uu - 128);

			*dst++ = CLAMP(r);
			*dst++ = CLAMP(g);
			*dst++ = CLAMP(b);
			*dst++ = 0xFF;
		}
	}
}

void X264_Decode_Convert_gbr24p_to_packed32(const uint8_t** src, uint32_t* dst, int width, int height) {
	uint32_t *dst_end = ((uint32_t*)dst) + (width * height);
	const uint8_t* __restrict gp = src[0];
    const uint8_t* __restrict bp = src[1];
    const uint8_t* __restrict rp = src[2];

	for (; dst < dst_end; dst++) *dst = *bp++ | (*gp++ << 8) | (*rp++ << 16);
}

void X264_Decode_Convert_gbr24p_to_packed32(uint8_t** src, int* srcStride, uint8_t *dst, int width, int height) {
	for (int i = 0; i < width * height; i++) {
		((int*) dst)[i] = src[1][i] + (src[0][i] << 8) + (src[2][i] << 16) + (0xff << 24);
	}
}

void X264_Decode_Delete(X264_Decode* x264) {
	av_parser_close(x264->parser);
    avcodec_free_context(&x264->c);
    av_frame_free(&x264->frame);
    av_packet_free(&x264->pkt);
}
