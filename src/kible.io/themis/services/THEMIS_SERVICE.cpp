#include "THEMIS_SERVICE.h"

bool Initialize_THEMIS_SERVICE(THEMIS_SERVICE *themis, KCONTEXT *ctx,
	VIDEO_SERVICE *video_service) {
	
	themis->ctx = ctx;
	themis->video_service = video_service;
	themis->main_loop = NULL;
	themis->main_loop_running = false;
	
	return true;
}

void Main_Loop_THEMIS_SERVICE(THEMIS_SERVICE *themis) {
	int code;
	SCREEN_DIM screen_dim;

	while (themis->main_loop_running) {
		if (Receive_SERVER(themis->s, (char*) &code, sizeof(int))) {
			if (code == THEMIS_SERVICE_CODE_CHANGE_SCREEN_SIZE) {
				if (Receive_SERVER(themis->s,
					(char*)&screen_dim,
					sizeof(SCREEN_DIM))) {
					
					Set_Screen_Dim_KCONTEXT(themis->ctx,
						screen_dim);

					if (!Resize_VIDEO_SERVICE(
						themis->video_service,
						screen_dim.sw,
						screen_dim.h,
						themis->video_service->encode_level)) {
						
						LOG_INFO_CTX((themis->ctx)) {
							ADD_STR_LOG("message",
							"Couldn't resize screen.");
						}
					}
				}
			}
		}
	}
}

bool Connect_THEMIS_SERVICE(THEMIS_SERVICE *themis, SERVER *s) {
	SCREEN_DIM screen_dim;

	themis->s = s;

	ASSERT_E_R(themis->s != NULL, "Server is NULL", themis->ctx);
	ASSERT_E_R(Receive_SERVER(themis->s, (char*)&screen_dim, sizeof(SCREEN_DIM)),
		"Couldn't receive screen_dim", themis->ctx);

	Set_Screen_Dim_KCONTEXT(themis->ctx, screen_dim);

	themis->main_loop_running = true;
	themis->main_loop = new thread(Main_Loop_THEMIS_SERVICE, themis);

	return true;
}

void Disconnect_THEMIS_SERVICE(THEMIS_SERVICE *themis) {
	themis->main_loop_running = false;
	if (themis->main_loop != NULL) {
		themis->main_loop->join();
		delete themis->main_loop;
		themis->main_loop = NULL;
	}
}

void Delete_THEMIS_SERVICE(THEMIS_SERVICE *themis) {

}
