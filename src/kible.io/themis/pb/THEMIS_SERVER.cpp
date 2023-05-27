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
HTTP_Protobuf_Callback_Endpoint(THEMIS, Density, \
	kible::themis::DensityRequest, kible::themis::DensityResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, Ping, \
	kible::themis::PingRequest, kible::themis::PingResponse)
HTTP_Protobuf_Callback_Endpoint(THEMIS, FPS, \
	kible::themis::FPSRequest, kible::themis::FPSResponse)

HTTP_Protobuf_Callback_End()

HTTP_Protobuf_Init_Delete(THEMIS, /api, kible.themis)