#include "THEMIS_SERVER.h"

HTTP_Protobuf_Callback_Begin(THEMIS)

HTTP_Protobuf_Callback_Endpoint(THEMIS, Update, kible::themis::UpdateRequest, \
	kible::themis::UpdateResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Launch, kible::themis::LaunchRequest, \
	kible::themis::LaunchResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Check, kible::themis::CheckRequest, \
	kible::themis::CheckResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Dimensions, \
	kible::themis::DimensionsRequest, kible::themis::DimensionsResponse)

HTTP_Protobuf_Callback_End()

HTTP_Protobuf_Init_Delete(THEMIS, /api, kible.themis)

bool pb::Update_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::UpdateRequest *request,
	kible::themis::UpdateResponse *response) {
	
	return true;
}

bool pb::Launch_THEMIS_SERVER(pb::THEMIS_SERVER *server,
	kible::themis::LaunchRequest *request,
	kible::themis::LaunchResponse *response) {
	
	return false; //unimplemented
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
	
	return false; //unimplemented
}