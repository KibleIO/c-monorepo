#ifndef _EDGE_CLIENT_H_
#define _EDGE_CLIENT_H_

#include "../../gen/edge/edge.pb.h"
#include "HTTP_CLIENT_HELPER.h"

HTTP_Protobuf_Client_Begin(EDGE)

HTTP_Protobuf_Client_Endpoint(EDGE, Themis, kible::edge::ThemisRequest,
	kible::edge::ThemisResponse)
HTTP_Protobuf_Client_Endpoint(EDGE, Kasm, kible::edge::KasmRequest,
	kible::edge::KasmResponse)
HTTP_Protobuf_Client_Endpoint(EDGE, Login, kible::edge::LoginRequest,
	kible::edge::LoginResponse)

HTTP_Protobuf_Client_End()

#endif