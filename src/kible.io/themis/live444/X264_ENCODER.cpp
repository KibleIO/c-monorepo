#include "X264_ENCODER.h"

void cb(x264_t * h, x264_nal_t * nal, void * opaque) {
	X264_ENCODER* x264 = (X264_ENCODER*)opaque;
	//assert((nal->i_payload*3)/2 + 5 + 64 < WS_SIZE);

	//cout << (nal->i_payload*3)/2 + 5 + 64 << endl;
	// wow bug: https://stackoverflow.com/questions/65665225/proper-use-of-nalu-process-callback-in-x264
	uint8_t temp[(nal->i_payload*3)/2 + 5 + 64];
	x264_nal_encode(x264->encoder, temp, nal);

	cout << (void*) temp << " " << (void*)nal->p_payload << endl;
	//x264->mtx.lock();
	//Send_Single_Frame_LIVE_444_SERVER(x264->server, nal);
	//x264->mtx.unlock();
	// Removed: Process nal.
}

bool Initialize_X264_ENCODER(X264_ENCODER* x264, SCREEN_DIM screen_dim, int fps) {
	log_dbg(((const JSON_TYPE){
		{"message", "initializing x264 encoder"},
		JSON_TYPE_END}));
	x264->screen_dim	= screen_dim;
	x264->encoder		= NULL;
	x264->nals			= NULL;
	x264->yuvbasef		= NULL;
	x264->yuvbaseb		= NULL;

	// get parameters object
	x264->mtx.lock();
	x264_param_default_preset(
	&x264->parameters, "faster", "zerolatency+fastdecode");
	x264->mtx.unlock();

	// parameter setup
	x264->parameters.i_csp					= X264_CSP_I420;
	x264->parameters.i_log_level 			= X264_LOG_INFO;
	x264->parameters.i_threads				= 8;
	//x264->parameters.b_sliced_threads		= 0;
	x264->parameters.b_sliced_threads		= 1;
	x264->parameters.i_width 				= screen_dim.bw;
	x264->parameters.i_height				= screen_dim.h;
	x264->parameters.i_fps_num				= fps;
	x264->parameters.i_fps_den				= 1;
	x264->parameters.i_nal_hrd = X264_NAL_HRD_NONE;

	x264->parameters.i_keyint_max			= X264_KEYINT_MAX_INFINITE;//change this when UDP comes back; X264_KEYINT_MAX_INFINITE
	x264->parameters.b_intra_refresh		= 1;

	//Rate control:
	int max_kbs = 7500;
	//int fps = 60;
	/*
	x264->parameters.rc.i_rc_method = X264_RC_ABR;
	x264->parameters.rc.i_vbv_buffer_size = max_kbs / fps;
	x264->parameters.rc.i_vbv_max_bitrate = max_kbs;
	x264->parameters.rc.i_bitrate = max_kbs;
	x264->parameters.b_vfr_input = 0;
	*/

	/*
	x264->parameters.analyse.i_subpel_refine = 6;
	x264->parameters.analyse.i_me_method = X264_ME_DIA;//X264_ME_HEX?X264_ME_DIA
	x264->parameters.analyse.i_me_range = 16;
	x264->parameters.analyse.i_direct_mv_pred = X264_DIRECT_PRED_AUTO;
	*/


	//x264->parameters.rc.i_rc_method			= X264_RC_ABR;

	x264->parameters.rc.i_rc_method = X264_RC_CRF;
	x264->parameters.rc.f_rf_constant		= 18;
	x264->parameters.rc.f_rf_constant_max	= 18 + 1;

	x264->parameters.rc.i_vbv_buffer_size	= 500; //why 100? honestly no idea
	x264->parameters.rc.i_vbv_max_bitrate 	= max_kbs;
	x264->parameters.rc.i_bitrate		= max_kbs;

	//x264->parameters.rc.i_vbv_buffer_size	= 100;
	//x264->parameters.rc.i_vbv_max_bitrate 	= 2048;
	//x264->parameters.rc.i_bitrate			= 2048;

	//x264->parameters.rc.b_filler = 1; //makes the bit rate more constant..

	//x264->parameters.rc.f_rf_constant		= 40;
	//x264->parameters.nalu_process = cb;
	//x264->parameters.rc.f_rf_constant_max	= 100;

	// x264->parameters.i_sps_id			= 7;
	x264->parameters.i_slice_max_size		= MAX_UDP_PACKET_SIZE - (crypto_secretbox_MACBYTES + crypto_secretbox_NONCEBYTES + 4); //add this back in when UDP is re-enabled
	x264->parameters.b_repeat_headers		= 1;
	//x264->parameters.b_annexb 			= 1;

	x264->mtx.lock();
	x264_param_apply_profile(&x264->parameters, "main");

	// open encoder
	x264->encoder = x264_encoder_open(&x264->parameters);
	if (!x264->encoder) {
		log_err(((const JSON_TYPE){
			{"message", "failed to open x264 encoder"},
			JSON_TYPE_END}));
		return false;
	}
	x264->mtx.unlock();

	x264->ysize = screen_dim.bw * screen_dim.h;
	x264->uvsize = x264->ysize / 4;

	int32_t bufsize	= x264->ysize * 1.5;

	x264->yuvbasef = new uint8_t[bufsize];
	x264->yuvbaseb = new uint8_t[bufsize];
	x264->nalu_process_buff = new uint8_t[bufsize]; //(nal->i_payload*3)/2 + 5 + 64

	// get picture in
	x264_picture_alloc(
	&x264->picture_in, X264_CSP_I420,
	x264->parameters.i_width, x264->parameters.i_height);
	x264_picture_clean(&x264->picture_in); // Using our own buffers

	x264->picture_in.i_type				= X264_TYPE_AUTO;
	x264->picture_in.img.i_csp			= X264_CSP_I420;
	x264->picture_in.img.i_plane		= 3;
	x264->picture_in.img.i_stride[0]	= screen_dim.bw;
	x264->picture_in.img.i_stride[1] 	= screen_dim.bw / 2;
	x264->picture_in.img.i_stride[2] 	= screen_dim.bw / 2;
	x264->picture_in.opaque			= (void*) x264;

	x264->picture_in.img.plane[0] = x264->yuvbasef;
	x264->picture_in.img.plane[1] = x264->yuvbasef + x264->ysize;
	x264->picture_in.img.plane[2] = x264->yuvbasef + x264->ysize +
		x264->uvsize;

	x264->frame1 = av_frame_alloc();
	x264->frame2 = av_frame_alloc();

	x264->resize = sws_getContext(screen_dim.bw, screen_dim.h,
		AV_PIX_FMT_BGRA, screen_dim.bw, screen_dim.h,
		AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	x264->frame1->format = AV_PIX_FMT_BGRA;
	x264->frame1->width = screen_dim.bw;
	x264->frame1->height = screen_dim.h;
	av_frame_get_buffer(x264->frame1, 1);

	x264->frame2->format = AV_PIX_FMT_YUV420P;
	x264->frame2->width = screen_dim.bw;
	x264->frame2->height = screen_dim.h;
	av_frame_get_buffer(x264->frame2, 1);

	x264->frame2->data[0] = x264->picture_in.img.plane[0];
	x264->frame2->data[1] = x264->picture_in.img.plane[1];
	x264->frame2->data[2] = x264->picture_in.img.plane[2];

	Initialize_RGB2YUV(&x264->converter, screen_dim.bw, screen_dim.h, 8);

	x264->pts = 0;

	log_dbg(((const JSON_TYPE){
		{"message", "done initializing x264 encoder"},
		JSON_TYPE_END}));
	return true;
}

int32_t Encode_Headers_X264_ENCODER(X264_ENCODER* x264) {
	int encoded;

	x264->i_nals	= 0;
	x264->index 	= 0;

	encoded = x264_encoder_headers(x264->encoder, &x264->nals, &x264->i_nals);

	return encoded;
}

int32_t Encode_Frame2_X264_ENCODER(X264_ENCODER* x264) {
	int encoded;

	x264->i_nals	= 0;
	x264->index 	= 0;

	x264->picture_in.i_pts = x264->pts;

	encoded = x264_encoder_encode(x264->encoder, &x264->nals, &x264->i_nals,
	&x264->picture_in, &x264->picture_out);

	x264->pts++;

	return encoded;
}

TIMER t;

int32_t Encode_Frame_X264_ENCODER(X264_ENCODER* x264, uint8_t* image) {
	int encoded;
        long time;

	x264->i_nals	= 0;
	x264->index 	= 0;

	x264->picture_in.i_pts = x264->pts;

        //Start_TIMER(&t);

	//Convert_RGB2YUV(&x264->converter, image, x264->yuvbasef);
	x264->frame1->data[0] = image;
	sws_scale(x264->resize, x264->frame1->data, x264->frame1->linesize, 0,
			  x264->screen_dim.h, x264->frame2->data,
			  x264->frame2->linesize);

	encoded = x264_encoder_encode(x264->encoder, &x264->nals, &x264->i_nals,
	&x264->picture_in, &x264->picture_out);      

        //cout << "stop " << Stop_TIMER(&t) << " " << x264->i_nals << " " << encoded << endl;

	/*
        time = Stop_TIMER(&t);
        if (time > (1000 / x264->parameters.i_fps_num)) {
                cout << "lag " << time << endl;
        }

	if (encoded > 100000) {
		cout << "SIZE " << encoded << " " << x264->i_nals << endl;
	}

	if (x264->i_nals > 11) {
		cout << "SIZE " << encoded << " " << x264->i_nals << endl;
	}
	*/

	x264->pts++;

	return encoded;
}

int32_t Encode_Frame_No_Convert_X264_ENCODER(X264_ENCODER* x264,
	uint8_t* image) {

	int encoded;

	x264->i_nals	= 0;
	x264->index 	= 0;

	x264->mtx.lock();
	x264->picture_in.img.plane[0] = image;
	x264->picture_in.img.plane[1] = image + x264->ysize;
	x264->picture_in.img.plane[2] = image + x264->ysize + x264->uvsize;

	encoded = x264_encoder_encode(x264->encoder, &x264->nals, &x264->i_nals,
	&x264->picture_in, &x264->picture_out);

	x264->mtx.unlock();

	return encoded;
}

void Delete_X264_ENCODER(X264_ENCODER* x264) {
	x264->mtx.lock();
	if (x264->yuvbasef) {
		delete [] x264->yuvbasef;
		x264->yuvbasef = NULL;
	}
	if (x264->yuvbaseb) {
		delete [] x264->yuvbaseb;
		x264->yuvbaseb = NULL;
	}
	if (x264->nalu_process_buff) {
		delete[] x264->nalu_process_buff;
		x264->nalu_process_buff = NULL;
	}
	if (x264->frame1) {
		av_frame_free(&x264->frame1);
		x264->frame1 = NULL;
	}
	if (x264->frame2) {
		av_frame_free(&x264->frame2);
		x264->frame2 = NULL;
	}
	if (x264->resize) {
		sws_freeContext(x264->resize);
		x264->resize = NULL;
	}
	log_dbg(((const JSON_TYPE){
		{"message", "deleting x264 encoder"},
		JSON_TYPE_END}));
	if (x264->encoder) {
		// flush
		log_dbg(((const JSON_TYPE){
			{"message", "attempting to flush x264..."},
			JSON_TYPE_END}));

		// memset((char *)&x264->picture_in, 0, sizeof(x264->picture_in));
		// memset((char *)&x264->picture_out, 0, sizeof(x264->picture_out));
		// x264_encoder_encode(
		// x264->encoder, &x264->nals, &x264->i_nals,
		// NULL, &x264->picture_out);
		log_dbg(((const JSON_TYPE){
			{"message", "flushed x264"},
			JSON_TYPE_END}));
		// close
		x264_encoder_close(x264->encoder);
		x264->encoder = NULL;

		log_dbg(((const JSON_TYPE){
			{"message", "successfully closed x264"},
			JSON_TYPE_END}));
		Delete_RGB2YUV(&x264->converter); //ewww
	}

	x264->mtx.unlock();
	log_dbg(((const JSON_TYPE){
		{"message", "done deleting x264 encoder"},
		JSON_TYPE_END}));
}
