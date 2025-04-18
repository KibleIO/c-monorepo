#include "THEMIS_HTTP_SERVER.h"

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server, KCONTEXT *ctx,
	int port) {
	
	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->ctx = ctx;
	server->themis_ext.connected = false;
	server->launch_thread = NULL;

	return pb::Initialize_THEMIS_SERVER(&server->server,
		(char*) listen_address.c_str(), (void*) server);
}

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server, KCONTEXT *ctx,
	int port, std::string path) {

	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->ctx = ctx;
	server->themis_ext.connected = false;
	server->launch_thread = NULL;

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

void Launch_Loop(THEMIS_EXT *themis_ext, KCONTEXT *ctx) {
	if (Initialize_THEMIS_EXT(themis_ext, ctx)) {
		while (Running_THEMIS_EXT(themis_ext)) {
			Sleep_Milli(1000); //busy wait
		}
	}
	Delete_THEMIS_EXT(themis_ext);
}

bool pb::Launch_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::LaunchRequest *request,
	kible::themis::LaunchResponse *response) {

	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

        if (!http_server->themis_ext.connected) {
                if (http_server->launch_thread != NULL) {
                        http_server->launch_thread->join();
		        delete http_server->launch_thread;
			http_server->launch_thread = NULL;
                }

		Set_Screen_Dim_KCONTEXT(http_server->ctx, (SCREEN_DIM) {
		request->width(),
		request->width(),
		request->height()});
		http_server->ctx->core_services_backbone_port = THEMIS_PORT;

		switch (request->launchbackend()) {
			case kible::themis::LaunchBackend::LAUNCHBACKEND_WS:
				http_server->ctx->core_services_backbone = 
					ROOT_SOCKET_TYPE_WS;
				break;
			default:
				return false;
		}

                http_server->launch_thread = new thread(Launch_Loop,
			&http_server->themis_ext, http_server->ctx);
		
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
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

	response->set_value(http_server->themis_ext.connected);
	return true;
}

bool pb::Ping_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::PingRequest *request,
	kible::themis::PingResponse *response) {
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;

	Start_TIMER(&http_server->themis_ext.timer);
	return true;
}

bool pb::Dimensions_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::DimensionsRequest *request,
	kible::themis::DimensionsResponse *response) {
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;
	
	VIDEO_SERVER* video_server = 
		Get_Instance_Of_SERVICE_SERVER_REGISTRY<VIDEO_SERVER*>(
		&http_server->themis_ext.registry);
	
	if (video_server == NULL) {
		return true;
	}

	Set_Screen_Dim_KCONTEXT(http_server->ctx, (SCREEN_DIM) {
		request->width(),
		request->width(),
		request->height()});

	return Resize_VIDEO_SERVER(video_server,
		request->width(), request->height(),
		video_server->encode_level, video_server->fps);
}

bool pb::FPS_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::FPSRequest *request,
	kible::themis::FPSResponse *response) {
	
	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;
	
	VIDEO_SERVER* video_server = 
		Get_Instance_Of_SERVICE_SERVER_REGISTRY<VIDEO_SERVER*>(
		&http_server->themis_ext.registry);
	
	if (video_server == NULL) {
		return true;
	}

	return Resize_VIDEO_SERVER(video_server,
		video_server->width,  video_server->height,
		video_server->encode_level, request->fps());
}

bool pb::Density_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::DensityRequest *request,
	kible::themis::DensityResponse *response) {
	
	ENCODE_LEVEL encode_level;

	THEMIS_HTTP_SERVER *http_server =
		(THEMIS_HTTP_SERVER*) server->user_ptr;
	
	VIDEO_SERVER* video_server = 
		Get_Instance_Of_SERVICE_SERVER_REGISTRY<VIDEO_SERVER*>(
		&http_server->themis_ext.registry);
	
	if (video_server == NULL || !http_server->themis_ext.connected) {
		return true;
	}
	
	switch (request->density()) {
		case kible::themis::PixelDensity::PIXELDENSITY_HIGH:
			encode_level = ENCODE_LEVEL_HIGH;
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_MEDIUM:
			encode_level = ENCODE_LEVEL_MEDIUM;
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_LOW:
			encode_level = ENCODE_LEVEL_LOW;
			break;
		case kible::themis::PixelDensity::PIXELDENSITY_PLACEBO:
			encode_level = ENCODE_LEVEL_PLACEBO;
			break;
	}

	Resize_VIDEO_SERVER(video_server, video_server->width,
		video_server->height, encode_level, video_server->fps);

	return true;
}

/*

why does this have to be defined here? https://stackoverflow.com/questions/56392506/compiler-optimization-removes-implicit-template-instantiation-leading-to-linker

*/
template<typename T>
T Get_Instance_Of_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY *registry) {
	T return_type = NULL;
	for (int i = 0; i < registry->service_count; i++) {
		return_type = dynamic_cast<const T>(registry->service_server[i]);
		if (return_type != NULL) {
			return return_type;
		}
	}
	return return_type;
}