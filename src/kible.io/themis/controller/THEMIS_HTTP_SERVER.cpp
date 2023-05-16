#include "THEMIS_HTTP_SERVER.h"

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server,
	THEMIS_EXT *themis, int port) {

	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->themis_ext = themis;
	return pb::Initialize_THEMIS_SERVER(&server->server,
		(char*) listen_address.c_str(), (void*) server->themis_ext);
}

bool Initialize_THEMIS_HTTP_SERVER(THEMIS_HTTP_SERVER *server,
	THEMIS_EXT *themis, int port, std::string path) {

	std::string listen_address;
	listen_address += "http://0.0.0.0:";
	listen_address += std::to_string(port);

	server->themis_ext = themis;
	return pb::Initialize_THEMIS_SERVER(&server->server,
		(char*) listen_address.c_str(), (void*) server->themis_ext,
		path);
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

                return true;
        } else {
        	return false;
        }
}

bool pb::Check_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::CheckRequest *request,
	kible::themis::CheckResponse *response) {
	
	std::cout << "called check " << request->text() << std::endl;
	response->set_value(true);
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
		request->width(), request->height())) {

		return true;
	} else {
		return false;
	}
}