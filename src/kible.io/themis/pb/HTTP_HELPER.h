#ifndef _HTTP_HELPER_H_
#define _HTTP_HELPER_H_

#define POLL_LATENCY 100

#include <string>
#include <google/protobuf/util/json_util.h>
#include "../mongoose/mongoose.h"

#define HTTP_Protobuf_Begin(service_actual_name)\
namespace pb {\
struct service_actual_name##_SERVER {\
	mg_mgr mgr;\
	volatile bool running;\
	void *user_ptr;\
};\
bool Initialize_##service_actual_name##_SERVER(service_actual_name##_SERVER*, \
char*, void*);\
void Run_##service_actual_name##_SERVER(service_actual_name##_SERVER*);\
void Delete_##service_actual_name##_SERVER(service_actual_name##_SERVER*);\

#define HTTP_Protobuf_End()\
	};\

#define HTTP_Protobuf_Endpoint(service_actual_name, endpoint_name,\
	request, response)\
	bool endpoint_name##_##service_actual_name##_SERVER(\
		service_actual_name##_SERVER*, request*, response*);\

#define HTTP_Protobuf_Callback_Begin(service_actual_name)\
void callback_##service_actual_name(mg_connection *c, int ev, void *ev_data, \
	void *fn_data) {\
	if (ev == MG_EV_HTTP_MSG) {\
		pb::service_actual_name##_SERVER *server = \
		(pb::service_actual_name##_SERVER*) fn_data;\
		mg_http_message *hm = (mg_http_message *) ev_data;\
		if (mg_vcmp(&hm->method, "POST") == 0) {\


#define HTTP_Protobuf_Callback_End()\
		}\
		mg_http_reply(c, 403, "", "%s", "Not Authorized\n");\
	}\
}\

#define HTTP_Protobuf_Callback_Endpoint(service_actual_name, endpoint_name, \
	path_actual, package_actual, request_obj, response_obj)\
	std::string endpoint_name##_request_address = \
			std::string(#path_actual) +  "/" + #package_actual + \
			"." + #service_actual_name + "/" + #endpoint_name;\
	if (mg_http_match_uri(hm, endpoint_name##_request_address.c_str())) {\
		request_obj request;\
		response_obj response;\
		google::protobuf::util::JsonParseOptions options;\
		JsonStringToMessage(std::string(hm->body.ptr), &request, \
			options);\
		if (pb::endpoint_name##_##service_actual_name##_SERVER(server, \
			&request, &response)) {\
			std::string json_string;\
			google::protobuf::util::JsonPrintOptions options2;\
			options2.add_whitespace = false;\
			options2.always_print_primitive_fields = true;\
			MessageToJsonString(response, &json_string, options2);\
			mg_http_reply(c, 200, \
				"Content-Type: application/json\r\n", \
				"%s", json_string.c_str());\
			return;\
		}\
	}\

#define HTTP_Protobuf_Init_Delete(service_actual_name)\
bool pb::Initialize_##service_actual_name##_SERVER(\
	pb::service_actual_name##_SERVER *server, char *address, \
	void *user_ptr) {\
	mg_mgr_init(&server->mgr);\
	mg_http_listen(&server->mgr, address, callback_##service_actual_name,\
	server);\
	server->running = false;\
	server->user_ptr = user_ptr;\
	return true;\
}\
void pb::Run_##service_actual_name##_SERVER(\
	pb::service_actual_name##_SERVER *server) {\
	server->running = true;\
	while (server->running) {\
		mg_mgr_poll(&server->mgr, POLL_LATENCY);\
	}\
}\
void pb::Delete_##service_actual_name##_SERVER(\
	pb::service_actual_name##_SERVER *server) {\
	server->running = false;\
	mg_mgr_free(&server->mgr);\ 
}\

#endif