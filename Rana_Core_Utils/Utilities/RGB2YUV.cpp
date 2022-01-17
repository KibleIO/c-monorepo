#include "RGB2YUV.h"

bool Initialize_RGB2YUV(
RGB2YUV* trans, uint32_t width, uint32_t height, uint32_t _threads,
uint8_t padding) {
	//if (trans->threads || trans->sws || trans->slice_height || trans->ysize ||
	//trans->uvsize) {
	//	log_err("RGB2YUV struct not properly nullified");
	//	return false;
	//}

	if (_threads < 1) {
		log_err(((const JSON_TYPE){
			{"message", "0 threads requested"},
			JSON_TYPE_END}));
		return false;
	}

	trans->width = width;
	trans->height = height;
	trans->threads = _threads;
	trans->ysize = width * height;
	trans->uvsize = trans->ysize / 4;

	/*
	if (width % 128 == 0 || !padding) {
		trans->yuv_strides[0] = width;
	} else {
		trans->yuv_strides[0] = width + (128 - width % 128);
	}
	*/
	trans->yuv_strides[0] = width;
	trans->yuv_strides[1] = trans->yuv_strides[0] / 2;
	trans->yuv_strides[2] = trans->yuv_strides[1];

	trans->rgb_strides[0] = width * 4;

	AVPixelFormat in = AV_PIX_FMT_BGRA;
	AVPixelFormat out = AV_PIX_FMT_YUV420P;

	int ywidth = trans->yuv_strides[0];

	/*
	uint32_t yslice = 0;
	uint32_t yslice_size = ywidth * height / trans->threads;
	uint32_t uslice = 0;
	uint32_t uslice_size = ywidth * height / 4 / trans->threads;
	uint32_t slice_h = 0;
	*/

	//lets check to make sure the resolution works with the number of
	//threads
	trans->slice_height = height / trans->threads;

	while (height % trans->threads != 0 || trans->slice_height % 2 != 0) {
		trans->threads--;
		trans->slice_height = height / trans->threads;
	}

	//check if  height % trans->threads = 0 && trans->slice_height % 2 = 0
	for (uint32_t i = 0; i < trans->threads; i++) {
		trans->sws[i] = sws_getContext(width, trans->slice_height, in,
			width, trans->slice_height, out, SWS_POINT, 0, 0, 0);

		if (!trans->sws[i]) {
			log_err(((const JSON_TYPE){
				{"message", "failed to open sws context"},
				JSON_TYPE_END}));
			for (uint32_t j = 0; j < i; j++) {
				sws_freeContext(trans->sws[j]);
			}
			return false;
		}

		/*
		trans->yuv_slice_i[i][0] = yslice;
		yslice += yslice_size;
		trans->yuv_slice_i[i][1] = uslice;
		trans->yuv_slice_i[i][2] = uslice;
		uslice += uslice_size;

		trans->slice_y[i] = slice_h;
		slice_h += trans->slice_height;
		*/
	}

	return true;
}

void Convert_RGB2YUV(RGB2YUV* trans, uint8_t* in, uint8_t* out) {
	thread* slice_threads[RGB2YUV_MAX_THREADS];
	uint8_t* rgb_planes[RGB2YUV_MAX_THREADS][1];
	uint8_t* yuv_planes[RGB2YUV_MAX_THREADS][3];

	for (uint32_t i = 0; i < trans->threads; i++) {
		rgb_planes[i][0] = in + (trans->slice_height * trans->rgb_strides[0] * i);

		yuv_planes[i][0] = out + (trans->slice_height * trans->yuv_strides[0] * i);
		yuv_planes[i][1] = out + (trans->ysize + ((trans->slice_height / 2) * trans->yuv_strides[1] * i));
		yuv_planes[i][2] = out + (trans->ysize + trans->uvsize + ((trans->slice_height / 2) * trans->yuv_strides[2] * i));

		/*
		rgb_planes[i][0] = in;

		yuv_planes[i][0] = out;
		yuv_planes[i][1] = out + (trans->ysize);
		yuv_planes[i][2] = out + (trans->ysize + trans->uvsize);
		*/

		slice_threads[i] = new thread(sws_scale,
		//sws_scale(
		trans->sws[i],
		rgb_planes[i],
		trans->rgb_strides,
		0,
		trans->slice_height,
		yuv_planes[i],
		trans->yuv_strides);

		//cout << "2:" << trans->rgb_strides[0] << " " << trans->yuv_strides[0] << " " << trans->yuv_strides[1] << " " << trans->yuv_strides[2] << endl;
		//cout << i << " " << (void*)yuv_planes[i][0] << " " << (void*)yuv_planes[i][1] << " " << (void*)yuv_planes[i][2] << endl;

	}

	for (uint32_t i = 0; i < trans->threads; i++) {
		slice_threads[i]->join();
		delete slice_threads[i];
	}

}

void Delete_RGB2YUV(RGB2YUV* trans) {
	for (uint32_t i = 0; i < trans->threads; i++) {
		sws_freeContext(trans->sws[i]);
	}
}
