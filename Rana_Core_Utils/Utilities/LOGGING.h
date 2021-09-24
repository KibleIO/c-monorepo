//PLATFORMS: Linux

#ifndef  LOGGING_H_
#define  LOGGING_H_

#include "ELASTIC_SEARCH_CLIENT.h"

#define log_dbg(payload) Write_Message(payload, __FILE__, __LINE__, __func__, \
	"debug")

#define log_err(payload) Write_Message(payload,  __FILE__, __LINE__, __func__, \
	"error")

#define log_close(payload) Write_Message(payload, __FILE__, __LINE__, \
	__func__, "close")

#ifdef __SYSTEM_RANA__
#define __CORE_SYSTEM__ "RANA"
#endif

#ifdef __SYSTEM_IRIS__
#define __CORE_SYSTEM__ "IRIS"
#endif

#ifdef __SYSTEM_THEMIS__
#define __CORE_SYSTEM__ "THEMIS"
#endif

#ifdef __SYSTEM_IRIS_HOST__
#define __CORE_SYSTEM__ "IRIS_HOST"
#endif

#ifdef __SYSTEM_THEMIS_HOST__
#define __CORE_SYSTEM__ "THEMIS_HOST"
#endif

#ifndef __CORE_SYSTEM__
#error PLEASE DEFINE __CORE_SYSTEM__
#endif

using namespace std;

extern uint8_t INITIALIZED_ELASTIC_SEARCH_CLIENT;
extern ELASTIC_SEARCH_CLIENT ELASTIC_CLIENT;

void initialize_elastic_search_client();
void Write_Message(const JSON_TYPE, string, uint32_t, string, string);

#endif
