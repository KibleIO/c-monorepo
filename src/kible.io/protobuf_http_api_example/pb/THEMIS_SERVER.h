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
HTTP_Protobuf_Endpoint(THEMIS, Resize, kible::themis::ResizeRequest,
	kible::themis::ResizeResponse)

HTTP_Protobuf_End()

#endif