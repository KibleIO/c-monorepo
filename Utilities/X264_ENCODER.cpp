#include "X264_ENCODER.h"

FILE* fp1;

void X264_Encoder_Initialize(X264_Encoder* x264, int w, int h, int bitrate) {
	x264->width  = w;
	x264->height = h;
	x264->i_frame = 0;
    x264->bitrate_kbps = bitrate;

    if (x264_param_default_preset(&x264->param, "ultrafast", "zerolatency") < 0) {
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
	x264->param.i_csp = X264_CSP_BGRA;
	x264->param.i_log_level = X264_LOG_INFO;
    x264->param.i_threads = 8;
    x264->param.i_width = x264->width;
    x264->param.i_height = x264->height;
    x264->param.i_fps_num = 60;
    x264->param.i_fps_den = 1;
    x264->param.i_keyint_max = 60;
    x264->param.b_intra_refresh = 1;
    x264->param.rc.i_rc_method = X264_RC_CRF;
    x264->param.rc.i_vbv_buffer_size = 400;
    x264->param.rc.i_vbv_max_bitrate = 10000;
    x264->param.rc.f_rf_constant = 25;
    x264->param.rc.f_rf_constant_max = 35;
    x264->param.i_sps_id = 7;
    // the following two value you should keep 1
    x264->param.b_repeat_headers = 1;    // to get header before every I-Frame
    x264->param.b_annexb = 1;

    if (x264_param_apply_profile(&x264->param, "high444") < 0) {
        cout << "we failed to apply profile" << endl;
        return;
    }
    x264_picture_init(&x264->pic);
    x264->pic.img.i_csp = X264_CSP_BGRA;
    x264->pic.img.i_plane = 1;
    x264->h = x264_encoder_open(&x264->param);
    if (!x264->h) {
        cout << "failed to allocate encoder" << endl;
        return;
    }
    x264->pic.img.i_stride[0] = x264->width * 4;

	fp1 = fopen("video.x264", "w+b");
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

int counter = 0;

int X264_Encoder_Encode_Frame_Buffer(X264_Encoder* x264, char* fbp, char** out) {
	x264->pic.img.plane[0] = (uint8_t*)fbp;
	x264->pic.i_pts = x264->i_frame++;
	Timer ttt;
	ttt.Start();
	x264->i_frame_size = x264_encoder_encode(x264->h, &x264->nal, &x264->i_nal, &x264->pic, &x264->pic_out);
	//cout << "full encode time: " << ttt.Stop() << " " << x264->i_frame_size << " " << x264->i_nal << endl;

    if (x264->i_frame_size < 0) {
            cout << "failed encode" << endl;
            return 0;
    }

	if (counter < 10000) {
		fwrite(x264->nal->p_payload, x264->i_frame_size, 1, fp1);
		counter++;
	} else {
		fclose(fp1);
		exit(1);
	}

    *out = (char*) x264->nal->p_payload;

    return x264->i_frame_size;
}

void X264_Encoder_Delete(X264_Encoder* x264) {
	x264_encoder_close(x264->h);
}
