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