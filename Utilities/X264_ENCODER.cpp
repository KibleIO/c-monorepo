#include "X264_ENCODER.h"

//FILE* fp1;

void Sws_setup(X264_Encoder* x264) {
	x264->sws = new Sws;
	x264->sws->context = NULL;
	x264->sws->rgb[0] = NULL;
	log_dbg("allocated swsrgb");
	x264->sws->context = sws_getCachedContext(x264->sws->context,
		x264->width, x264->height, AV_PIX_FMT_BGRA,
		x264->width, x264->height, AV_PIX_FMT_YUV420P,
		SWS_FAST_BILINEAR, 0, 0, 0);
	//log_dbg("sws width: " + to_string(x264->sws->srcW));
	log_dbg("got sws context");
	x264->sws->rgbstride[0] = 4 * x264->width;

	//int width = RNDTO2(x264->width);
	//int height = RNDTO2(x264->height);
	//int ystride = RNDTO32(width);
	//int uvstride = RNDTO32(width/2);
	int width = x264->width;
	int height = x264->height;
	int ystride = width;
	int uvstride = width/2;
	int ysize = ystride * height;
	int vusize = uvstride * (height/2);
	int size = ysize + (2*vusize);

	x264->sws->yuvbasef = new char[size];
	x264->sws->yuvbaseb = new char[size];
	x264->sws->yuv[0] = x264->sws->yuvbasef;
	x264->pic.img.plane[0] = (uint8_t*)x264->sws->yuvbaseb;
	x264->sws->yuv[1] = x264->sws->yuvbasef+ysize;
	x264->pic.img.plane[1] = (uint8_t*)x264->sws->yuvbaseb+ysize;
	x264->sws->yuv[2] = x264->sws->yuvbasef+ysize+vusize;
	x264->pic.img.plane[2] = (uint8_t*)x264->sws->yuvbaseb+ysize+vusize;
	x264->sws->yuv[3] = 0;
	x264->sws->yuvstride[0] = ystride;
	x264->sws->yuvstride[1] = uvstride;
	x264->sws->yuvstride[2] = uvstride;
	x264->sws->yuvstride[3] = 0;
	log_dbg("SWScale setup complete");
}


void X264_Encoder_Initialize(X264_Encoder* x264, int w, int h, int bitrate) {
	x264->width  = w;
	x264->height = h;
	x264->i_frame = 0;
    x264->bitrate_kbps = bitrate;

    if (x264_param_default_preset(&x264->param, "ultrafast", "zerolatency+fastdecode") < 0) {
        cout << "we failed!!" << endl;
        return;
    }
	/*
    x264->param.i_csp = X264_CSP_BGRA;
    x264->param.i_width  = x264->width;
    x264->param.i_height = x264->height;
	x264->param.i_threads = 1;
    x264->param.rc.i_rc_method = X264_RC_CRF;
	x264->param.i_fps_num = 30;
    x264->param.i_fps_den = 1;
    x264->param.i_keyint_max = 30;
    x264->param.b_intra_refresh = 1;
	x264->param.rc.i_vbv_buffer_size = 100;
	x264->param.rc.i_vbv_max_bitrate = 3000;
    x264->param.rc.f_rf_constant = 30;
    x264->param.rc.f_rf_constant_max = 40;
    x264->param.i_sps_id = 7;
	x264->param.i_slice_max_size = 1500;
    // the following two value you should keep 1
    x264->param.b_repeat_headers = 1;    // to get header before every I-Frame
    x264->param.b_annexb = 1;
	*/
	//x264->param.i_csp = X264_CSP_BGRA;
	x264->param.i_csp = X264_CSP_I420;
	x264->param.i_log_level = X264_LOG_INFO;
    x264->param.i_threads = 16;
    x264->param.i_width = x264->width;
    x264->param.i_height = x264->height;
    x264->param.i_fps_num = 60;
    x264->param.i_fps_den = 1;
    x264->param.i_keyint_max = 60;
    x264->param.b_intra_refresh = 1;
    x264->param.rc.i_rc_method = X264_RC_CRF;
    x264->param.rc.i_vbv_buffer_size = 2000;
    x264->param.rc.i_vbv_max_bitrate = 4000;
    x264->param.rc.f_rf_constant = 20;
    x264->param.rc.f_rf_constant_max = 30;
    x264->param.i_sps_id = 7;
		x264->param.i_slice_max_size = 1500;
    // the following two value you should keep 1
    x264->param.b_repeat_headers = 1;    // to get header before every I-Frame
    x264->param.b_annexb = 1;
	x264->param.b_opencl = 1;
	x264->param.i_nal_hrd = 0;

	//x264->param.i_bframe = 16;
	//x264->param.i_bframe_adaptive = X264_B_ADAPT_NONE;
	//x264->param.analyse.i_me_method = X264_ME_DIA;
	//x264->param.analyse.i_subpel_refine = 0;
	//x264->param.analyse.b_chroma_me = 0;
	x264->param.rc.b_stat_read = 0;
	x264->param.rc.b_stat_write = 1;

	x264_param_apply_fastfirstpass(&x264->param);

    if (x264_param_apply_profile(&x264->param, "baseline") < 0) {
        cout << "we failed to apply profile" << endl;
        return;
    }
    x264_picture_init(&x264->pic);
    //x264->pic.img.i_csp = X264_CSP_BGRA;
    //x264->pic.img.i_plane = 1;
    x264->pic.img.i_csp = X264_CSP_I420;
    x264->pic.img.i_plane = 3;
    x264->h = x264_encoder_open(&x264->param);
    if (!x264->h) {
        cout << "failed to allocate encoder" << endl;
        return;
    }
    //x264->pic.img.i_stride[0] = x264->width * 4;
    //x264->pic.img.i_stride[0] = RNDTO32(RNDTO2(x264->width));
    //x264->pic.img.i_stride[1] = RNDTO32(RNDTO2(x264->width/2));
    //x264->pic.img.i_stride[2] = RNDTO32(RNDTO2(x264->width/2));
    x264->pic.img.i_stride[0] = x264->width;
    x264->pic.img.i_stride[1] = x264->width/2;
    x264->pic.img.i_stride[2] = x264->width/2;
	Sws_setup(x264);
	log_dbg("Encoder set up: " + to_string(w) + "x" + to_string(h));

	//fp1 = fopen("video.x264", "w+b");
}

int X264_Encoder_Get_Header(X264_Encoder* x264, char** out) {
	x264->i_frame_size = x264_encoder_headers(x264->h, &x264->nal, &x264->i_nal);

    if (x264->i_frame_size < 0) {
        cout << "failed encode" << endl;
        return 0;
    }

    *out = (char*) x264->nal->p_payload;

    return x264->i_frame_size;
}

uint8_t averageChroma(uint8_t* rgb, int i, int c, int w) {
	return (rgb[4 * i + c] + rgb[4 * (i+1) + c] + rgb[4 * (i+w) + c] + rgb[4 * (i+w+1) + c]) / 4;
}

void rgb_to_y420p_no_x264(int width, int height, uint8_t* destination, uint8_t* rgb) {
	//Timer tm1;
	//long tv1;
	//tm1.Start();
	size_t image_size = width * height;
	size_t upos = image_size;
	size_t vpos = upos + upos / 4;
	size_t i = 0;

	for (size_t line = 0; line < height; line++) {
		if (!(line & 1)) {
			for (size_t x = 0; x < width; x += 2) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);

				//U and V are switched, deal with it
				destination[vpos++] = (RU*r + GU*g + BU*b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[vpos++] = ((-43*r + -84*g + 127*b) >> 8) + 128;
				//destination[vpos++] = ((-38*r + -74*g + 112*b) >> 8) + 128;
				//destination[vpos++] = CLAMP(-0.148*r + -0.291*g + 0.439*b + 128);
				destination[upos++] = (RV*r + GV*g + BV*b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[upos++] = ((127*r + -106*g + -21*b) >> 8) + 128;
				//destination[upos++] = ((112*r + -94*g + -18*b) >> 8) + 128;
				//destination[upos++] = CLAMP(0.439*r + -0.368*g + -0.071*b + 128);

				r = rgb[4 * i];
				g = rgb[4 * i + 1];
				b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);
			}
		} else {
			for (size_t x = 0; x < width; x++) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);
			}
		}
	}
//	tv1 = tm1.Stop();
//	log_dbg("convert took " + to_string(tv1));
}
void rgb_to_y420p(X264_Encoder* x264, uint8_t* destination, uint8_t* rgb) {
	//Timer tm1;
	//long tv1;
	//tm1.Start();
	size_t width = x264->width;
	size_t height = x264->height;
	size_t image_size = width * height;
	size_t upos = image_size;
	size_t vpos = upos + upos / 4;
	size_t i = 0;

	for (size_t line = 0; line < height; line++) {
		if (!(line & 1)) {
			for (size_t x = 0; x < width; x += 2) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);

				//U and V are switched, deal with it
				destination[vpos++] = (RU*r + GU*g + BU*b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[vpos++] = ((-43*r + -84*g + 127*b) >> 8) + 128;
				//destination[vpos++] = ((-38*r + -74*g + 112*b) >> 8) + 128;
				//destination[vpos++] = CLAMP(-0.148*r + -0.291*g + 0.439*b + 128);
				destination[upos++] = (RV*r + GV*g + BV*b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[upos++] = ((127*r + -106*g + -21*b) >> 8) + 128;
				//destination[upos++] = ((112*r + -94*g + -18*b) >> 8) + 128;
				//destination[upos++] = CLAMP(0.439*r + -0.368*g + -0.071*b + 128);

				r = rgb[4 * i];
				g = rgb[4 * i + 1];
				b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);
			}
		} else {
			for (size_t x = 0; x < width; x++) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				destination[i++] = (RY*r + GY*g + BY*b + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT;
				//destination[i++] = ((77*r + 150*g + 29*b) >> 8) + 16;
				//destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				//destination[i++] = CLAMP(0.257*r + 0.504*g + 0.098*b + 16);
			}
		}
	}
}

int counter = 0;

void swapBuffers(X264_Encoder* x264) {
	void* swapper = x264->sws->yuvbasef;
	x264->sws->yuvbasef = x264->sws->yuvbaseb;
	x264->sws->yuvbaseb = swapper;
	x264->pic.img.plane[0] = (uint8_t*)x264->sws->yuvbaseb;
	x264->pic.img.plane[1] = (uint8_t*)x264->sws->yuvbaseb + x264->width * x264->height;
	x264->pic.img.plane[2] = (uint8_t*)x264->sws->yuvbaseb + x264->width * x264->height + x264->width * x264->height / 4;
}

int X264_Encoder_Encode_Frame_Buffer_No_Convert(X264_Encoder* x264, char* fbp, char** out) {
	x264->pic.img.plane[0] = (uint8_t*)fbp;
	x264->pic.img.plane[1] = (uint8_t*)fbp + x264->width * x264->height;
	x264->pic.img.plane[2] = (uint8_t*)fbp + x264->width * x264->height + x264->width * x264->height / 4;

	x264->i_frame_size = x264_encoder_encode(x264->h, &x264->nal, &x264->i_nal, &x264->pic, &x264->pic_out);

    if (x264->i_frame_size < 0) {
            cout << "failed encode" << endl;
            return 0;
    }

	swapBuffers(x264);

    *out = (char*) x264->nal->p_payload;

    return x264->i_frame_size;
}

int X264_Encoder_Encode_Frame_Buffer(X264_Encoder* x264, char* fbp, char** out) {
	int rv;
	//x264->pic.img.plane[0] = (uint8_t*)fbp;
	//x264->sws->rgb[0] = (uint8_t*)fbp;

	//Timer tm1,tm2;
	//long tv1,tv2;
	//tm1.Start();

	thread convert(rgb_to_y420p, x264, (uint8_t*)x264->sws->yuvbasef, (uint8_t*)fbp);
	//convert.join();
	//thread compress(compress_wrapper, x264);
	//compress.join();
	//swapBuffers(x264);

	//rgb_to_y420p(x264, (uint8_t*)x264->sws->yuvbaseb, (uint8_t*)fbp);

	//sws_scale(x264->sws->context, x264->sws->rgb,
	//	x264->sws->rgbstride, 0, x264->height,
	//	(uint8_t* const*)x264->sws->yuv, x264->sws->yuvstride);


	//x264->pic.img.plane[0] = (uint8_t*)x264->sws->rgb[0];

	//tv1 = tm1.Stop();
	//log_dbg("conversion took " + to_string(tv1) + " ms");
	//x264->pic.i_pts = x264->i_frame++;
	////Timer ttt;
	////ttt.Start();
	//tm1.Start();

	x264->i_frame_size = x264_encoder_encode(x264->h, &x264->nal, &x264->i_nal, &x264->pic, &x264->pic_out);

	//tm2.Start();
	convert.join();
	//tv2 = tm2.Stop();
	//log_dbg("joining took " + to_string(tv2));

	swapBuffers(x264);

	//tv1 = tm1.Stop();
	//log_dbg("x264_encode took " + to_string(tv1) + " ms for " + to_string(x264->i_frame_size) + " bytes");
	//cout << "full encode time: " << ttt.Stop() << " " << x264->i_frame_size << " " << x264->i_nal << endl;

    if (x264->i_frame_size < 0) {
            cout << "failed encode" << endl;
            return 0;
    }


    *out = (char*) x264->nal->p_payload;

    return x264->i_frame_size;
}

void X264_Encoder_Delete(X264_Encoder* x264) {
	x264_encoder_close(x264->h);
}
