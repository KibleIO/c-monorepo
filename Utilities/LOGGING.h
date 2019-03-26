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
#include <dirent.h>

#define log_dbg(_MSG_) Write_Message("DEBUG:   ", _MSG_, __FILE__ , __LINE__, __func__)

#define log_err(_MSG_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__)

#define log_warn(_MSG_) Write_Message("WARNING: ", _MSG_, __FILE__ , __LINE__, __func__)

#define log_note(_MSG_) Write_Message("NOTE:    ", _MSG_, __FILE__ , __LINE__, __func__)

#define LOG_DIR "logs"

#define MAX_LOGS 20

using namespace std;

extern uint8_t GOT_LOCAL_TIME;
extern string LOG_FILE;

void set_local_start_time();
void Write_Message(string, string, string, uint32_t, string);

#endif
