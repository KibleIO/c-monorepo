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

#define LOG_DIR "./logs"

#define MAX_LOGS 20

using namespace std;

extern uint8_t INITIALIZED_ELASTIC_SEARCH_CLIENT;
extern ELASTIC_SEARCH_CLIENT ELASTIC_CLIENT;

void initialize_elastic_search_client();
void Write_Message(const JSON_TYPE, string, uint32_t, string, string);

#endif
