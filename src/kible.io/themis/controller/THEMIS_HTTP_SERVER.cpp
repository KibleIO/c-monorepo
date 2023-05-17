#include "THEMIS_HTTP_SERVER.h"

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server,
	THEMIS_EXT *themis, int port) {

	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->themis_ext = themis;
	return pb::Initialize_THEMIS_SERVER(&server->server,
		(char*) listen_address.c_str(), (void*) server);
}

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server,
	THEMIS_EXT *themis, int port, std::string path) {

	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->themis_ext = themis;
	return pb::Initialize_THEMIS_SERVER(&server->server,
		(char*) listen_address.c_str(), (void*) server, path);
}

void Run_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server) {
	pb::Run_THEMIS_SERVER(&server->server);
}

void Delete_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server) {
	pb::Delete_THEMIS_SERVER(&server->server);
}

bool pb::Update_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::UpdateRequest *request,
	kible::themis::UpdateResponse *response) {
	
	return true;
}

void Launch_Loop2(THEMIS_EXT *themis_ext) {
	if (Connect_THEMIS_EXT(themis_ext)) {
		TIMER t;
		Start_TIMER(&t);
		LOG_INFO_CTX((themis_ext->ctx)) {
			ADD_STR_LOG("message", "Started Themis Session.");
		}
		while (Running_THEMIS_EXT(themis_ext)) {
			Sleep_Milli(1000); //busy wait
		}
		LOG_INFO_CTX((themis_ext->ctx)) {
			ADD_STR_LOG("message", "Ended Themis Session.");
			ADD_INT_LOG("time", Stop_TIMER(&t));
		}
	}
	Disconnect_THEMIS_EXT(themis_ext);
}

bool pb::Launch_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::LaunchRequest *request,
	kible::themis::LaunchResponse *response) {

	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

        if (!*http_server->themis_ext->connected) {
                if (http_server->launch_thread != NULL) {
                        http_server->launch_thread->join();
		        delete http_server->launch_thread;
                }

                http_server->launch_thread = new thread(Launch_Loop2,
			http_server->themis_ext);
		
		//for good measure... or for fun, who knows
		Sleep_Milli(500);

                return true;
        } else {
        	return false;
        }
}

bool pb::Check_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::CheckRequest *request,
	kible::themis::CheckResponse *response) {
	
	response->set_value(*http_server->themis_ext->connected);
	return true;
}

bool pb::Dimensions_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::DimensionsRequest *request,
	kible::themis::DimensionsResponse *response) {
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

	Set_Screen_Dim_KCONTEXT(http_server->themis_ext->ctx, (SCREEN_DIM) {
		request->width(),
		request->width(),
		request->height()});

	if (Resize_VIDEO_SERVICE(&http_server->themis_ext->video,
		request->width(), request->height(),
		http_server->themis_ext->video.encode_level)) {

		return true;
	} else {
		return false;
	}
}

bool pb::Density_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::DensityRequest *request,
	kible::themis::DensityResponse *response) {
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

	switch (request->density()) {
		case kible::themis::PixelDensity::PIXELDENSITY_HIGH:
			if (*http_server->themis_ext->connected) {
				Resize_VIDEO_SERVICE(
					&http_server->themis_ext->video,
					http_server->themis_ext->video.width,
					http_server->themis_ext->video.height,
					ENCODE_LEVEL_HIGH);
			}
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_MEDIUM:
			if (*http_server->themis_ext->connected) {
				Resize_VIDEO_SERVICE(
					&http_server->themis_ext->video,
					http_server->themis_ext->video.width,
					http_server->themis_ext->video.height,
					ENCODE_LEVEL_MEDIUM);
			}
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_LOW:
			if (*http_server->themis_ext->connected) {
				Resize_VIDEO_SERVICE(
					&http_server->themis_ext->video,
					http_server->themis_ext->video.width,
					http_server->themis_ext->video.height,
					ENCODE_LEVEL_LOW);
			}
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_PLACEBO:
			if (*http_server->themis_ext->connected) {
				Resize_VIDEO_SERVICE(
					&http_server->themis_ext->video,
					http_server->themis_ext->video.width,
					http_server->themis_ext->video.height,
					ENCODE_LEVEL_PLACEBO);
			}
			break;
	}

	return true;
}