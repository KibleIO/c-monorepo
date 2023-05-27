#include "EDGE_CLIENT.h"

HTTP_Protobuf_Client_Init_Delete(kible.edge, EDGE, /api)

HTTP_Protobuf_Endpoint_Definition(EDGE, Themis, kible::edge::ThemisRequest,
	kible::edge::ThemisResponse)
HTTP_Protobuf_Endpoint_Definition(EDGE, Kasm, kible::edge::KasmRequest,
	kible::edge::KasmResponse)
HTTP_Protobuf_Endpoint_Definition(EDGE, Login, kible::edge::LoginRequest,
	kible::edge::LoginResponse)