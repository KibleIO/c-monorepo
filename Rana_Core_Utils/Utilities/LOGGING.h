//PLATFORMS: Linux

#ifndef  LOGGING_H_
#define  LOGGING_H_

#include "ELASTIC_SEARCH_CLIENT.h"

#define log_dbg(payload)

#define log_err(payload) Write_Message(payload,  __FILE__, __LINE__, __func__, \
	"error")

#define log_close(payload) Write_Message(payload, __FILE__, __LINE__, \
	__func__, "close")

#define log_info(...) static JSON_TYPE logging_temp_var = __VA_ARGS__;\
	Write_Message(logging_temp_var, __FILE__, __LINE__, __func__, \
	"info")

#define TO_STRING(integer) (char*) itoa((const char[ITOA_STR_SIZE]){}, integer)

using namespace std;

extern uint8_t INITIALIZED_ELASTIC_SEARCH_CLIENT;
extern ELASTIC_SEARCH_CLIENT ELASTIC_CLIENT;

void initialize_elastic_search_client();
void Write_Message(const JSON_TYPE, string, uint32_t, string, string);

#endif
