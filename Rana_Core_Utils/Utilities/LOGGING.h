//PLATFORMS: Linux

#ifndef  LOGGING_H_
#define  LOGGING_H_

#include "ELASTIC_SEARCH_CLIENT.h"

#define log_dbg(payload)

#define log_err(payload) Write_Message(payload,  __FILE__, __LINE__, __func__, \
	"error")

#define log_close(payload) Write_Message(payload, __FILE__, __LINE__, \
	__func__, "close")

//so so so hacky
#define MERGE_(a,b)  a##b
#define LABEL_(a) MERGE_(unique_name_, a)
#define UNIQUE_NAME LABEL_(__LINE__)

#define log_info(...) static JSON_TYPE UNIQUE_NAME = __VA_ARGS__;\
	Write_Message(UNIQUE_NAME, __FILE__, __LINE__, __func__, \
	"info")

#define log_error(...) static JSON_TYPE UNIQUE_NAME = __VA_ARGS__;\
	Write_Message(UNIQUE_NAME, __FILE__, __LINE__, __func__, \
	"error")

#define TO_STRING(str, integer) (char*) itoa(str, integer)
#define STRING_TO(str) (char*) str.c_str()

using namespace std;

extern uint8_t INITIALIZED_ELASTIC_SEARCH_CLIENT;
extern ELASTIC_SEARCH_CLIENT ELASTIC_CLIENT;

void initialize_elastic_search_client();
void Write_Message(const JSON_TYPE, string, uint32_t, string, string);

#endif
