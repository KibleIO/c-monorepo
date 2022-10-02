#ifndef _KCONTEXT_H_
#define _KCONTEXT_H_

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
#include "../idl/cpp/gen/gateway.grpc.pb.h"
//end grpc

#ifdef _WIN64

#include <wincrypt.h>
#include <Windows.h>

#endif

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
		Log_KCONTEXT(ctx, loop.str))
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
#define DEFAULT_GRPC_TIMEOUT 60

#define INIT_CONN_KCONTEXT_SUCCESS 0
#define INIT_CONN_KCONTEXT_ABORT 1
#define INIT_CONN_KCONTEXT_KEY 2
#define INIT_CONN_KCONTEXT_EMAIL 3
#define INIT_CONN_KCONTEXT_LOCATION 4
#define INIT_CONN_KCONTEXT_WELCOME 5 //this is only used in RANA
#define INIT_CONN_KCONTEXT_REGISTER 6 //this is only used in RANA

#define GRPC_ADDRESS "api.kible.com:51942"
#define INSECURE_GRPC_ADDRESS "45.57.227.210:52942"

#define KIBLE_PREMIUM_UUID "313319a8-7045-479e-90f1-1f31ea099eaa"
#define KIBLE_TRIAL_ENDED_UUID "1b3300ed-140b-4fd8-9cef-4420b9b2a289"
#define KIBLE_FULL_DESKTOP "fb6a8625-7c95-4803-adde-51709cc21032"

#define CHECKOUT_URL_SIZE 512

#define INIT_GRPC_STUB \
std::unique_ptr<gaia::GATEWAY::Stub> stub;\
	if (ctx->insecure_mode) {\
		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(\
			INSECURE_GRPC_ADDRESS,\
			grpc::InsecureChannelCredentials()));\
	} else {\
		char cacert_dir[MAX_DIRECTORY_LEN];\
		Get_CACERT_Dir(cacert_dir);\
		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);\
		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,\
			grpc::SslCredentials(grpc::SslCredentialsOptions())));\
	}\

#define INIT_GRPC_STUB_LINUX \
std::unique_ptr<gaia::GATEWAY::Stub> stub;\
	if (ctx->insecure_mode) {\
		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(\
			INSECURE_GRPC_ADDRESS,\
			grpc::InsecureChannelCredentials()));\
	} else {\
		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,\
			grpc::SslCredentials(grpc::SslCredentialsOptions())));\
	}\

struct KCONTEXT {
	char trace_uuid[UUID_STR_SIZE];
        string uuid;
	char core_system[CORE_SYSTEM_STR_SIZE];
        char system_resource_dir[RESOURCE_DIR_MAX_LEN];
	ELASTIC_SEARCH_CLIENT client;
	SCREEN_DIM screen_dim;
        volatile bool connection_initialized;
	volatile bool rana_initialized;
        gaia::Connection connection;
	gaia::LocationUUID locationID;
	//LOL this is so bad, please fix these
	gaia::GetLocationsResponse locations;
	gaia::GetProductsResponse products;
	gaia::ListAdsResponse ads;
	string recent_error;
	bool insecure_mode;
};

bool Initialize_KCONTEXT(KCONTEXT*, char*, bool);
SCREEN_DIM Get_Screen_Dim_KCONTEXT(KCONTEXT*);
void Set_Screen_Dim_KCONTEXT(KCONTEXT*, SCREEN_DIM);
void Set_System_Resource_Dir_KCONTEXT(KCONTEXT*, char*);
int Initialize_Connection_KCONTEXT(KCONTEXT*, string);
int Create_Rana_KCONTEXT(KCONTEXT*, string, string);
int Check_Existing_Token_KCONTEXT(KCONTEXT*);
bool Check_Password_Strength_KCONTEXT(KCONTEXT*, string);
bool Reset_Password_KCONTEXT(KCONTEXT*, string);
bool Check_For_Update_KCONTEXT(KCONTEXT*, char*);
bool Login_Rana_KCONTEXT(KCONTEXT*, string, string);
bool Get_Location_KCONTEXT(KCONTEXT*);
bool Get_Products_KCONTEXT(KCONTEXT*);
bool Get_Ads_KCONTEXT(KCONTEXT*);
void Sign_Out_Of_Session_KCONTEXT(KCONTEXT*);
bool GetCheckoutUrl(KCONTEXT*, char*);
bool GetCheckPayment(KCONTEXT*);
void Log_KCONTEXT(KCONTEXT*, char*);
void Delete_KCONTEXT(KCONTEXT*);

#endif
