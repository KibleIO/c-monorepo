#include "YUV2RGB.h"

bool Initialize_YUV2RGB(
YUV2RGB* trans, uint32_t width, uint32_t height, uint32_t _threads,
uint8_t padding) {
	//if (trans->threads || trans->sws || trans->slice_height || trans->ysize ||
	//trans->uvsize) {
	//	log_err("yuv2rgb struct not properly nullified");
	//	return false;
	//}

	if (_threads < 1) {
		/*
		log_err(((const JSON_TYPE){
			{"message", "0 threads requested"},
			JSON_TYPE_END}));
		*/
		return false;
	}

	trans->threads = _threads;
	trans->ysize = width * height;
	trans->uvsize = trans->ysize / 4;

	if (width % 128 == 0 || !padding) {
		trans->yuv_strides[0] = width;
	} else {
		trans->yuv_strides[0] = width + (128 - width % 128);
	}
	trans->yuv_strides[1] = trans->yuv_strides[0] / 2;
	trans->yuv_strides[2] = trans->yuv_strides[1];

	trans->rgb_strides[0] = width * 4;

	AVPixelFormat in = AV_PIX_FMT_YUV420P;
	AVPixelFormat out = AV_PIX_FMT_RGB32;

	int ywidth = trans->yuv_strides[0];

	uint32_t yslice = 0;
	uint32_t yslice_size = ywidth * height / trans->threads;
	uint32_t uslice = 0;
	uint32_t uslice_size = ywidth * height / 4 / trans->threads;
	uint32_t slice_h = 0;
	trans->slice_height = height / trans->threads;
	for (uint32_t i = 0; i < trans->threads; i++) {
		trans->sws[i] = sws_getContext(
		width, height, in, width, height, out,
		SWS_POINT, 0, 0, 0);

		if (!trans->sws[i]) {
			/*
			log_err(((const JSON_TYPE){
				{"message", "failed to open sws context"},
				JSON_TYPE_END}));
			*/
			for (uint32_t j = 0; j < i; j++) {
				sws_freeContext(trans->sws[j]);
			}
			return false;
		}

		trans->yuv_slice_i[i][0] = yslice;
		yslice += yslice_size;
		trans->yuv_slice_i[i][1] = uslice;
		trans->yuv_slice_i[i][2] = uslice;
		uslice += uslice_size;

		trans->slice_y[i] = slice_h;
		slice_h += trans->slice_height;
	}

	return true;
}

void Convert_YUV2RGB(YUV2RGB* trans, uint8_t* in, uint8_t* out) {
	thread* slice_threads[YUV2RGB_MAX_THREADS];
	uint8_t* rgb_planes[1];
	uint8_t* yuv_planes[YUV2RGB_MAX_THREADS][3];

	rgb_planes[0] = out;

	for (uint32_t i = 0; i < trans->threads; i++) {
		yuv_planes[i][0] = in + trans->yuv_slice_i[i][0];
		//swapped for some reason
		yuv_planes[i][1] = in + trans->ysize + trans->yuv_slice_i[i][1];
		yuv_planes[i][2] = in + trans->ysize + trans->uvsize +
		trans->yuv_slice_i[i][2];
		slice_threads[i] = new thread(sws_scale,
		trans->sws[i],
		yuv_planes[i],
		trans->yuv_strides,
		trans->slice_y[i],
		trans->slice_height,
		rgb_planes,
		trans->rgb_strides);
	}

	for (uint32_t i = 0; i < trans->threads; i++) {
		slice_threads[i]->join();
		delete slice_threads[i];
	}
}

void Convert_YUV2RGB(YUV2RGB* trans, uint8_t** in, uint8_t* out) {
	thread* slice_threads[YUV2RGB_MAX_THREADS];
	uint8_t* rgb_planes[YUV2RGB_MAX_THREADS][1];
	uint8_t* yuv_planes[YUV2RGB_MAX_THREADS][3];

	for (uint32_t i = 0; i < trans->threads; i++) {
		rgb_planes[i][0] = out + (trans->slice_height * trans->rgb_strides[0] * i);
		yuv_planes[i][0] = in[0] + trans->yuv_slice_i[i][0];
		//swapped for some reason
		yuv_planes[i][1] = in[1] + trans->yuv_slice_i[i][1];
		yuv_planes[i][2] = in[2] + trans->yuv_slice_i[i][2];
		slice_threads[i] = new thread(sws_scale,
		trans->sws[i],
		yuv_planes[i],
		trans->yuv_strides,
		0,
		trans->slice_height,
		rgb_planes[i],
		trans->rgb_strides);
	}

	for (uint32_t i = 0; i < trans->threads; i++) {
		slice_threads[i]->join();
		delete slice_threads[i];
	}
}

void Delete_YUV2RGB(YUV2RGB* trans) {
	for (uint32_t i = 0; i < trans->threads; i++) {
		sws_freeContext(trans->sws[i]);
	}
}
