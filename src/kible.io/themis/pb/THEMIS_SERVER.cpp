#include "THEMIS_SERVER.h"

HTTP_Protobuf_Callback_Begin(THEMIS)

HTTP_Protobuf_Callback_Endpoint(THEMIS, Update, /api, kible.themis, \
	kible::themis::UpdateRequest, kible::themis::UpdateResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Launch, /api, kible.themis, \
	kible::themis::LaunchRequest, kible::themis::LaunchResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Check, /api, kible.themis, \
	kible::themis::CheckRequest, kible::themis::CheckResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Dimensions, /api, kible.themis, \
	kible::themis::DimensionsRequest, kible::themis::DimensionsResponse)

HTTP_Protobuf_Callback_End()

HTTP_Protobuf_Init_Delete(THEMIS)