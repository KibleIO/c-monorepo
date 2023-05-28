#ifndef _HTTP_CLIENT_HELPER_H_
#define _HTTP_CLIENT_HELPER_H_

#include <curl/curl.h>
#include <string>
#include <google/protobuf/util/json_util.h>

#define HTTP_Protobuf_Client_Init_Delete(package_actual, service_actual_name,\
	path_actual)\
	bool pb::Initialize_##service_actual_name##_CLIENT(\
		pb::service_actual_name##_CLIENT *client,\
		std::string service_address) {\
  	client->service_address = service_address;\
	client->service_package = #package_actual;\
	client->service_name = #service_actual_name;\
	client->service_path = #path_actual;\
	curl_global_init(CURL_GLOBAL_ALL);\
	return true;\
}\
void pb::Delete_##service_actual_name##_CLIENT(\
	pb::service_actual_name##_CLIENT *client) {curl_global_cleanup();}\
size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *str) {\
	*str += std::string((char*) ptr);\
	return size*nmemb;\
}\

#define HTTP_Protobuf_Client_Begin(service_actual_name)\
	namespace pb {\
	struct service_actual_name##_CLIENT {\
		std::string service_address;\
		std::string service_package;\
		std::string service_name;\
		std::string service_path;\
	};\
	bool Initialize_##service_actual_name##_CLIENT(\
		service_actual_name##_CLIENT*, std::string);\
	void Delete_##service_actual_name##_CLIENT(\
		service_actual_name##_CLIENT*);\

#define HTTP_Protobuf_Client_End()\
	};\

#define HTTP_Protobuf_Client_Endpoint(service_actual_name, endpoint_name,\
	request, response)\
	bool endpoint_name##_##service_actual_name##_CLIENT(\
		service_actual_name##_CLIENT*, request*, response*);\

#define HTTP_Protobuf_Endpoint_Definition(service_actual_name, endpoint_name,\
	request_obj, response_obj)\
	bool pb::endpoint_name##_##service_actual_name##_CLIENT(\
		pb::service_actual_name##_CLIENT *client,\
		request_obj *request, response_obj *response) {\
		CURL *curl;\
		CURLcode res;\
		curl_slist *hs = NULL;\
		std::string output_str;\
		std::string json_string;\
		google::protobuf::util::JsonPrintOptions options;\
		options.add_whitespace = false;\
		options.always_print_primitive_fields = true;\
		MessageToJsonString(*request, &json_string, options);\
		std::string request_address = client->service_address + \
			client->service_path + "/" + \
			client->service_package + "." + client->service_name + \
			"/" + #endpoint_name;\
		curl = curl_easy_init();\
  		if (curl) {\
			curl_easy_setopt(curl, CURLOPT_URL, \
				request_address.c_str());\
			hs = curl_slist_append(hs, \
				"Content-Type: application/json");\
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);\
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, \
				json_string.c_str());\
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, \
				writefunc);\
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_str);\
			res = curl_easy_perform(curl);\
			if(res != CURLE_OK) {\
				return false;\
			}\
			google::protobuf::util::JsonParseOptions options2;\
			JsonStringToMessage(output_str, response, options2);\
			curl_easy_cleanup(curl);\
			return true;\
		}\
		return false;\
	}\

#endif