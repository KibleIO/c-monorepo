#include "VIDEO_SERVER.h"

void Recv_Callback_VIDEO_SERVER(void *user_ptr, char *buffer, int buffer_size) {
	//nop... video server doesn't receive data
}

bool VIDEO_SERVER::Initialize(KCONTEXT *ctx_in,
	SERVICE_SERVER_REGISTRY *registry) {

	ctx = ctx_in;
	main_loop = NULL;
	main_loop_running = false;
	encode_level = ENCODE_LEVEL_MEDIUM;
	fps = VIDEO_FPS;

	if (!Initialize_SOCKET_SERVER(&socket_server,
		Recv_Callback_VIDEO_SERVER, &registry->socket_server_registry,
		ctx, this)) {
		
		return false;
	}

	SCREEN_DIM screen_dim = Get_Screen_Dim_KCONTEXT(ctx);
	width = screen_dim.sw;
	height = screen_dim.h;

	ASSERT_E_R((Initialize_FPS_LIMITER(&fps_limiter,
		fps, false)), "Failed to intialize fps limiter",
		ctx);

	ASSERT_E_R((Initialize_XVFB_Handler(&xvfb, screen_dim.sw,
		screen_dim.h)), "Couldn't initialize xvfb", ctx);

	ASSERT_E_R((Initialize_X264_ENCODER(&encoder, screen_dim,
		encode_level, fps)),
		"Couldn't initialize x264 encoder", ctx);

	main_loop_running = true;
        main_loop = new thread(Main_VIDEO_SERVER, this);

	return true;
}

void VIDEO_SERVER::Delete() {
	main_loop_running = false;
	if (main_loop != NULL) {
		main_loop->join();
		delete main_loop;
		main_loop = NULL;
	}

        Delete_XVFB_Handler(&xvfb);
        Delete_X264_ENCODER(&encoder);

	Delete_FPS_LIMITER(&fps_limiter);

	Delete_SOCKET_SERVER(&socket_server);
}

void Main_VIDEO_SERVER(VIDEO_SERVER *video) {
        int size;

	while (video->main_loop_running) {
		if (video->resize) {
			Delete_XVFB_Handler(&video->xvfb);
        		Delete_X264_ENCODER(&video->encoder);
			Delete_FPS_LIMITER(&video->fps_limiter);

			ASSERT_E_B((Initialize_FPS_LIMITER(&video->fps_limiter,
				video->fps, false)), "Failed to intialize fps limiter",
				video->ctx);
			ASSERT_E_B((Initialize_XVFB_Handler(&video->xvfb,
				video->width, video->height)),
				"Couldn't initialize xvfb", video->ctx);
			ASSERT_E_B((Initialize_X264_ENCODER(&video->encoder,
			(SCREEN_DIM) {
				video->width,
				video->width,
				video->height}, video->encode_level,
			video->fps)), "Couldn't initialize x264 encoder",
			video->ctx);

			video->resize = false;
		} else {
			Start_FPS_LIMITER(&video->fps_limiter);

			size = Encode_Frame_X264_ENCODER(&video->encoder,
			(uint8_t*) Render_XVFB_Handler(&video->xvfb));

			Send_SOCKET_SERVER(&video->socket_server,
				(char*) video->encoder.nals[0].p_payload,
				size);
			
			Stop_FPS_LIMITER(&video->fps_limiter);
		}
	}
}

bool Resize_VIDEO_SERVER(VIDEO_SERVER *video, int width, int height,
	ENCODE_LEVEL encode_level, int fps) {
	
	int counter = 0;

	video->width = width;
	video->height = height;
	video->resize = true;
	video->fps = fps;
	video->encode_level = encode_level;

	while (counter < MAX_LOOPS_RESIZE) {
		if (!video->resize) {
			return true;
		}
		Sleep_Milli(100);
	}

	return false;
}