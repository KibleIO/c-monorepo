//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  LOGGING_H_
#define  LOGGING_H_

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Linux specific includes {{{
#ifdef __linux__
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#include "../Networking/direntwin.h"
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple includes
#endif
// }}}

#define log_dbg(_MSG_) Write_Message("DEBUG:   ", _MSG_, __FILE__ , __LINE__, __func__)

#define log_err(_MSG_) Write_Message("ERROR:   ", _MSG_, __FILE__ , __LINE__, __func__)

#define log_tmp(_MSG_) Write_Message("DELETE:  ", _MSG_, __FILE__ , __LINE__, __func__)

#define LOG_DIR "./logs"

#define MAX_LOGS 20

using namespace std;

extern uint8_t GOT_LOCAL_TIME;
extern string LOG_FILE;

void set_local_start_time();
void Write_Message(string, string, string, uint32_t, string);

#endif
