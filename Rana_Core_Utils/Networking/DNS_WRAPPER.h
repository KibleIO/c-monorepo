//#ifndef _WIN64

#ifndef DNS_WRAPPER_H_
#define DNS_WRAPPER_H_

//#include <sys/select.h>
#include <ares.h>
//#include <netdb.h>
#include <mutex>
#include "../Utilities/LOGGING.h"

extern uint32_t ip_ret_DNS_WRAPPER;
extern std::mutex mutex_DNS_WRAPPER;

#define IP_NOT_FOUND 0

uint8_t getaddrinfo_k(long unsigned int*, const char*, uint32_t); //k for kevin

#endif

//#endif
