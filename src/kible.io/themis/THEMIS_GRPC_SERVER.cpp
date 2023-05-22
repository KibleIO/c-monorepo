#include "THEMIS_GRPC_SERVER.h"

THEMIS_GRPC_SERVER::THEMIS_GRPC_SERVER(THEMIS_EXT *t) {
	update_thread = NULL;
	launch_thread = NULL;
	themis_ext = t;

	Initialize_UPDATE_UTILITY(&updateInfo, PATH_TO_VERSION,
		REMOTE_FILE_ARCHIVE_NAME, PATH_TO_FILE_ARCHIVE,
		ROOT_DIR);
}

void Update_Loop(Update_UTILITY *updateInfo) {
	Update_System(updateInfo);
}

grpc::Status THEMIS_GRPC_SERVER::Dimensions(grpc::ServerContext* context,
	const kible::themis::DimensionsRequest* request, kible::themis::DimensionsResponse* response) {

	Set_Screen_Dim_KCONTEXT(themis_ext->ctx, (SCREEN_DIM) {
		request->width(),
		request->width(),
		request->height()});

	if (Resize_VIDEO_SERVICE(&themis_ext->video, request->width(),
		request->height(), themis_ext->video.encode_level)) {

		return grpc::Status::OK;
	} else {
		return grpc::Status(grpc::StatusCode::ABORTED,
			"Couldn't change screen size");
	}
}

grpc::Status THEMIS_GRPC_SERVER::Update(grpc::ServerContext* context,
	const kible::themis::UpdateRequest* request, kible::themis::UpdateResponse* response) {

	/*
	if (update_thread != NULL) {
		update_thread->join();
		delete update_thread;
	}

	update_thread = new thread(Update_Loop, &updateInfo);
	*/

	return grpc::Status::OK;
}

void Launch_Loop(THEMIS_EXT *themis_ext) {
	if (Connect_THEMIS_EXT(themis_ext)) {
		while (Running_THEMIS_EXT(themis_ext)) {
			Sleep_Milli(1000); //busy wait
		}
	}
	Disconnect_THEMIS_EXT(themis_ext);
}

grpc::Status THEMIS_GRPC_SERVER::Launch(grpc::ServerContext* context,
	const kible::themis::LaunchRequest* request, kible::themis::LaunchResponse* response) {
	
	char version[VERSION_STRING_LENGTH];

	if (update_running) {
		return grpc::Status(grpc::StatusCode::ABORTED, "An update is in progress");
	}

	if (strcmp(version, SOFTWARE_VERSION) != 0) {
		update_running = true;
		Update_System(&updateInfo);
		return grpc::Status(grpc::StatusCode::ABORTED, "Updated.");
	}

        if (!*themis_ext->connected) {
                if (launch_thread != NULL) {
                        launch_thread->join();
		        delete launch_thread;
                }

                launch_thread = new thread(Launch_Loop, themis_ext);

                return grpc::Status::OK;
        } else {
        	return grpc::Status(grpc::StatusCode::ABORTED, "Instance still running");
        }
}

grpc::Status THEMIS_GRPC_SERVER::Check(grpc::ServerContext* context, const kible::themis::CheckRequest* request,
	kible::themis::CheckResponse* response) {

	//false -> rana is not connected
	//true -> rana is connected

	response->set_value(*themis_ext->connected);

	return grpc::Status::OK;
}

void THEMIS_GRPC_SERVER::Start(int port) {
        launch_thread = NULL;
	update_running = false;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(string("0.0.0.0:") + to_string(port), grpc::InsecureServerCredentials());
	builder.RegisterService(this);
	server = builder.BuildAndStart();

	server->Wait();
}

void THEMIS_GRPC_SERVER::Stop() {
	if (*themis_ext->connected) {
		*themis_ext->connected = false;
	}
	if (launch_thread != NULL) {
		launch_thread->join();
		delete launch_thread;
	}

	server->Shutdown();
}
