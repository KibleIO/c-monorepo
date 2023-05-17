#include "VIDEO_SERVICE.h"

bool Initialize_VIDEO_SERVICE(VIDEO_SERVICE *video, KCONTEXT *ctx) {
	video->ctx = ctx;
	video->main_loop = NULL;
	video->main_loop_running = false;
	video->encode_level = ENCODE_LEVEL_MEDIUM;

	ASSERT_E_R((Initialize_FPS_LIMITER(&video->fps_limiter,
		VIDEO_FPS, false)), "Failed to intialize fps limiter",
		video->ctx);

	return true;
}

void Main_TCP_Loop_VIDEO_SERVICE(VIDEO_SERVICE *video) {
        int size;

	while (video->main_loop_running) {
		Start_FPS_LIMITER(&video->fps_limiter);

		if (video->resize) {
			Delete_XVFB_Handler(&video->xvfb);
        		Delete_X264_ENCODER(&video->encoder);
			ASSERT_E_B((Initialize_XVFB_Handler(&video->xvfb,
				video->width, video->height)),
				"Couldn't initialize xvfb", video->ctx);
			ASSERT_E_B((Initialize_X264_ENCODER(&video->encoder,
			(SCREEN_DIM) {
				video->width,
				video->width,
				video->height}, video->encode_level,
			VIDEO_FPS)), "Couldn't initialize x264 encoder",
			video->ctx);

			size = Encode_Headers_X264_ENCODER(&video->encoder);
			ASSERT_E_B(size > 0, "Couldn't encode headers", 
				video->ctx);
			ASSERT_E_B((Send_SERVER(video->s, (char*) &size,
				sizeof(int))), "Couldn't send headers",
				video->ctx);

			ASSERT_E_B((Send_SERVER(video->s,
				(char*)video->encoder.nals[0].p_payload, size)),
				"Couldn't send headers", video->ctx);

			video->resize = false;
		}

		size = Encode_Frame_X264_ENCODER(&video->encoder,
			(uint8_t*) Render_XVFB_Handler(&video->xvfb));

                Send_SERVER(video->s, (char*) &size, sizeof(int));
                Send_SERVER(video->s, (char*) video->encoder.nals[0].
				p_payload,
				size);

		Stop_FPS_LIMITER(&video->fps_limiter);
	}
}

bool Resize_VIDEO_SERVICE(VIDEO_SERVICE *video, int width, int height,
	ENCODE_LEVEL encode_level) {
	
	int counter = 0;

	video->width = width;
	video->height = height;
	video->resize = true;
	video->encode_level = encode_level;

	while (counter < MAX_LOOPS_RESIZE) {
		if (!video->resize) {
			return true;
		}
		Sleep_Milli(100);
	}

	return false;
}

void Main_UDP_Loop_VIDEO_SERVICE(VIDEO_SERVICE *video) {
	while (video->main_loop_running) {
		Start_FPS_LIMITER(&video->fps_limiter);

		Encode_Frame_X264_ENCODER(&video->encoder,
			(uint8_t*) Render_XVFB_Handler(&video->xvfb));

		while (video->encoder.index < video->encoder.i_nals) {
			((int*) video->nal_unit)[0] = video->encoder.pts;
			memcpy(video->nal_unit + 4,
				video->encoder.nals[video->encoder.index].
				p_payload,
				video->encoder.nals[video->encoder.index].
				i_payload);
			Send_SERVER(video->s, video->nal_unit,
				video->encoder.nals[video->encoder.index].
				i_payload + 4);
			video->encoder.index++; 
		}

		Stop_FPS_LIMITER(&video->fps_limiter);
	}
}

bool Connect_VIDEO_SERVICE(VIDEO_SERVICE *video, SERVER *video_init,
	SERVER *s) {

	int size;
	SCREEN_DIM screen_dim = Get_Screen_Dim_KCONTEXT(video->ctx);

	video->s = s;

	ASSERT_E_R(video_init != NULL, "video_init is NULL", video->ctx);
	ASSERT_E_R(video->s != NULL, "server is NULL", video->ctx);

	ASSERT_E_R((Initialize_XVFB_Handler(&video->xvfb, screen_dim.sw,
		screen_dim.h)), "Couldn't initialize xvfb", video->ctx);
	ASSERT_E_R((Initialize_X264_ENCODER(&video->encoder, screen_dim,
		video->encode_level, VIDEO_FPS)),
		"Couldn't initialize x264 encoder", video->ctx);

	size = Encode_Headers_X264_ENCODER(&video->encoder);
	ASSERT_E_R(size > 0, "Couldn't encode headers", video->ctx);
        ASSERT_E_R((Send_SERVER(video_init,
		(char*) &size, sizeof(int))),
		"Couldn't send headers", video->ctx);
	ASSERT_E_R((Send_SERVER(video_init,
		(char*)video->encoder.nals[0].p_payload, size)),
		"Couldn't send headers", video->ctx);

	video->main_loop_running = true;

        video->main_loop = new thread(Main_TCP_Loop_VIDEO_SERVICE, video);

	return true;
}

void Disconnect_VIDEO_SERVICE(VIDEO_SERVICE *video) {
	video->main_loop_running = false;
	if (video->main_loop != NULL) {
		video->main_loop->join();
		delete video->main_loop;
		video->main_loop = NULL;
	}

        Delete_XVFB_Handler(&video->xvfb);
        Delete_X264_ENCODER(&video->encoder);
}

void Delete_VIDEO_SERVICE(VIDEO_SERVICE *video) {
	Delete_FPS_LIMITER(&video->fps_limiter);
}
