#ifndef _KCONTEXT_H_
#define _KCONTEXT_H_

#include "UTILS.h"
#include "SCREEN_DIM.h"
#include "ASSERT.h"
#include "LOGGING.h"

//oh god... check here for explanation: https://stackoverflow.com/questions/57008541/how-to-avoid-a-globally-defined-c-macro-of-status-from-xlib-h
#if defined (Status)
# undef Status
typedef int Status;
#endif

#define RESOURCE_DIR_MAX_LEN 2048

#define INFO_FILE_NAME "info"
#define CONTAINER_ID_LOC "/etc/hostname"

#define LOGGER_INFO(ctx, ...) Log_LOGGING(&ctx->logging, __VA_ARGS__, "INFO", __FILE__, __LINE__, __func__)
#define LOGGER_WARN(ctx, ...) Log_LOGGING(&ctx->logging, __VA_ARGS__, "WARN", __FILE__, __LINE__, __func__)
#define LOGGER_ERROR(ctx, ...) Log_LOGGING(&ctx->logging, __VA_ARGS__, "ERROR", __FILE__, __LINE__, __func__)

#define INIT_CONN_KCONTEXT_SUCCESS 0
#define INIT_CONN_KCONTEXT_ABORT 1
#define INIT_CONN_KCONTEXT_KEY 2
#define INIT_CONN_KCONTEXT_EMAIL 3
#define INIT_CONN_KCONTEXT_LOCATION 4
#define INIT_CONN_KCONTEXT_WELCOME 5 //this is only used in RANA
#define INIT_CONN_KCONTEXT_REGISTER 6 //this is only used in RANA

struct KCONTEXT {
	char trace_uuid[UUID_STR_SIZE];
        string uuid;
	char core_system[CORE_SYSTEM_STR_SIZE];
        char system_resource_dir[RESOURCE_DIR_MAX_LEN];
	LOGGING logging;
	SCREEN_DIM screen_dim;
        volatile bool connection_initialized;
	volatile bool rana_initialized;
	string recent_error;
	bool insecure_mode;
	int core_services_backbone;
	int core_services_backbone_port;
};

bool Initialize_KCONTEXT(KCONTEXT*, char*, bool);
SCREEN_DIM Get_Screen_Dim_KCONTEXT(KCONTEXT*);
void Set_Screen_Dim_KCONTEXT(KCONTEXT*, SCREEN_DIM);
void Set_System_Resource_Dir_KCONTEXT(KCONTEXT*, char*);
int Initialize_Connection_KCONTEXT(KCONTEXT*, string);
void Delete_KCONTEXT(KCONTEXT*);

#endif
