#ifndef _PB_THEMIS_SERVER_H_
#define _PB_THEMIS_SERVER_H_

#include <iostream>
#include "../../gen/themis/themis.pb.h"
#include "HTTP_HELPER.h"

HTTP_Protobuf_Begin(THEMIS)

HTTP_Protobuf_Endpoint(THEMIS, Update, kible::themis::UpdateRequest,
	kible::themis::UpdateResponse)
HTTP_Protobuf_Endpoint(THEMIS, Launch, kible::themis::LaunchRequest,
	kible::themis::LaunchResponse)
HTTP_Protobuf_Endpoint(THEMIS, Check, kible::themis::CheckRequest,
	kible::themis::CheckResponse)
HTTP_Protobuf_Endpoint(THEMIS, Dimensions, kible::themis::DimensionsRequest,
	kible::themis::DimensionsResponse)
HTTP_Protobuf_Endpoint(THEMIS, Density, kible::themis::DensityRequest,
	kible::themis::DensityResponse)

HTTP_Protobuf_End()

#endif