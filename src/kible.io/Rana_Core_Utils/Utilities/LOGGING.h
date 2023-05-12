//PLATFORMS: Linux

#ifndef  LOGGING_H_
#define  LOGGING_H_

//this is here purely for legacy purposes
#define log_dbg(payload)
#define log_err(payload)
#define log_close(payload)
#define log_info(...)
#define log_error(...)

#define JSON_TYPE_END {"\0", "\0"}
typedef char* JSON_TYPE[][2];


#endif
