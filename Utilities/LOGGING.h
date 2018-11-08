#ifndef  LOGGING_H_
#define  LOGGING_H_

#include <stdio.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#define log_dbg(_MSG_) Write_Message("DEBUG:   ", _MSG_, __FILE__ , __LINE__, __func__, false)
#define if_log_dbg(_CON_, _MSG_) if (_CON_) Write_Message("DEBUG:   ", _MSG_, __FILE__ , __LINE__, __func__, false)


#define log_err(_MSG_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__, true)
#define if_log_err(_CON_, _MSG_) if (_CON_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__, true)
#define log_err_no_kill(_MSG_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__, false)
#define if_log_err_no_kill(_CON_, _MSG_) if (_CON_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__, false)


#define log_warn(_MSG_) Write_Message("WARNING: ", _MSG_, __FILE__ , __LINE__, __func__, false)
#define if_log_warn(_CON_, _MSG_) if (_CON_) Write_Message("WARNING: ", _MSG_, __FILE__ , __LINE__, __func__, false)


#define log_note(_MSG_) Write_Message("NOTE:    ", _MSG_, __FILE__ , __LINE__, __func__, false)
#define if_log_note(_CON_, _MSG_) if (_CON_) Write_Message("NOTE:    ", _MSG_, __FILE__ , __LINE__, __func__, false)

//#define log_dbg(_MSG_)
//#define if_log_dbg(_CON_, _MSG_)
//
//#define log_err(_MSG_)
//#define if_log_err(_CON_, _MSG_)
//#define log_err_no_kill(_MSG_)
//#define if_log_err_no_kill(_CON_, _MSG_)
//
//#define log_warn(_MSG_)
//#define if_log_warn(_CON_, _MSG_)
//
//#define log_note(_MSG_)
//#define if_log_note(_CON_, _MSG_)

using namespace std;

extern bool GOT_LOCAL_TIME; 
extern string LOG_FILE;

int get_local_start_time();
void Write_Message(string type, string message, const char* file, int line, const char* func, bool kill);

#endif
