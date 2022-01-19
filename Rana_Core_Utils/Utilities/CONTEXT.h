#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <json-c/json.h>
#include "ELASTIC_SEARCH_CLIENT.h"
#include "UTILS.h"
#include "SCREEN_DIM.h"

//WARNING: This will eventually cause an issue
#define MAX_LOG_LEN 8192
#define ADD_STR_LOG(key, obj)  json_object_object_add(loop.json_obj, key, json_object_new_string(obj))
#define ADD_INT_LOG(key, obj)  json_object_object_add(loop.json_obj, key, json_object_new_int(obj))
#define SET_UP_LOG(ctx, level) (loop.json_obj = json_object_new_object(),\
		get_current_time(loop.str),\
		ADD_STR_LOG("@_timestamp", loop.str),\
		ADD_STR_LOG("file", __FILE__),\
		ADD_INT_LOG("line", __LINE__),\
		ADD_STR_LOG("function", __func__),\
		ADD_STR_LOG("system", ctx->core_system),\
		ADD_STR_LOG("uuid", ctx->uuid),\
		ADD_STR_LOG("mac_address", ctx->mac_address),\
		ADD_STR_LOG("type", level))
#define FINISH_LOG(ctx) (strcpy(loop.str,\
		json_object_to_json_string_ext(loop.json_obj,\
		JSON_C_TO_STRING_PLAIN)),\
		json_object_put(loop.json_obj),\
		Log_CONTEXT(ctx, loop.str))
#define LOG_CTX(ctx, level)\
	for (struct {char str[MAX_LOG_LEN]; int _break; json_object *json_obj;}\
	loop = {"", (SET_UP_LOG(ctx, level), 1), NULL}; loop._break; \
	loop._break = 0, FINISH_LOG(ctx))

#define LOG_INFO_CTX(ctx) LOG_CTX(ctx, "INFO")
#define LOG_WARN_CTX(ctx) LOG_CTX(ctx, "WARN")
#define LOG_ERROR_CTX(ctx) LOG_CTX(ctx, "ERROR")

struct CONTEXT {
	char mac_address[MAC_ADDRESS_STR_LEN];
	char uuid[UUID_STR_SIZE];
	char core_system[CORE_SYSTEM_STR_SIZE];
	ELASTIC_SEARCH_CLIENT client;
	SCREEN_DIM screen_dim;
};

bool Initialize_CONTEXT(CONTEXT*, char*);
SCREEN_DIM Get_Screen_Dim_CONTEXT(CONTEXT*);
void Set_Screen_Dim_CONTEXT(CONTEXT*, SCREEN_DIM);
void Log_CONTEXT(CONTEXT*, char*);
void Delete_CONTEXT(CONTEXT*);

#endif
