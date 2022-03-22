#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <json-c/json.h>
#include "ELASTIC_SEARCH_CLIENT.h"
#include "UTILS.h"
#include "SCREEN_DIM.h"
#include "ASSERT.h"

//oh god... check here for explanation: https://stackoverflow.com/questions/57008541/how-to-avoid-a-globally-defined-c-macro-of-status-from-xlib-h

#if defined (Status)
# undef Status
typedef int Status;
#endif

//beging grpc
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "../idl/cpp/gen/gaia.grpc.pb.h"
//end grpc

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
		ADD_STR_LOG("system", (ctx)->core_system),\
		ADD_STR_LOG("trace_uuid", (ctx)->trace_uuid),\
                ADD_STR_LOG("uuid", (ctx)->uuid.c_str()),\
		ADD_STR_LOG("type", level))
#define FINISH_LOG(ctx) (strcpy(loop.str,\
		json_object_to_json_string_ext(loop.json_obj,\
		JSON_C_TO_STRING_PLAIN)),\
		json_object_put(loop.json_obj),\
		Log_CONTEXT(ctx, loop.str))
#define LOG_CTX(ctx, level)\
	for (struct {char str[MAX_LOG_LEN]; int _break; json_object *json_obj;}\
	loop = {"", 1, NULL}, dummy = {"", (SET_UP_LOG(ctx, level), 1), NULL}; \
        loop._break; loop._break = 0, FINISH_LOG(ctx))

#define LOG_INFO_CTX(ctx) LOG_CTX(ctx, "INFO")
#define LOG_WARN_CTX(ctx) LOG_CTX(ctx, "WARN")
#define LOG_ERROR_CTX(ctx) LOG_CTX(ctx, "ERROR")

#define RESOURCE_DIR_MAX_LEN 2048

#define INFO_FILE_NAME "info"
#define CONTAINER_ID_LOC "/etc/hostname"
//in seconds
#define DEFAULT_GRPC_TIMEOUT 2

struct CONTEXT {
	char trace_uuid[UUID_STR_SIZE];
        string uuid;
	char core_system[CORE_SYSTEM_STR_SIZE];
        char system_resource_dir[RESOURCE_DIR_MAX_LEN];
	ELASTIC_SEARCH_CLIENT client;
	SCREEN_DIM screen_dim;
        bool connection_initialized;
        project::Connection connection;
};

bool Initialize_CONTEXT(CONTEXT*, char*);
SCREEN_DIM Get_Screen_Dim_CONTEXT(CONTEXT*);
void Set_Screen_Dim_CONTEXT(CONTEXT*, SCREEN_DIM);
void Set_System_Resource_Dir_CONTEXT(CONTEXT*, char*);
bool Initialize_Connection_CONTEXT(CONTEXT*, string);
void Log_CONTEXT(CONTEXT*, char*);
void Delete_CONTEXT(CONTEXT*);

#endif
