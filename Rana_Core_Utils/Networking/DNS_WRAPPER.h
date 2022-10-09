#ifndef DNS_WRAPPER_H_
#define DNS_WRAPPER_H_

#ifndef _WIN64
#include <sys/select.h>
#include <netdb.h>
#endif

#include <ares.h>
#include <mutex>
#include "../Utilities/LOGGING.h"

extern uint32_t ip_ret_DNS_WRAPPER;
extern std::mutex mutex_DNS_WRAPPER;

#define IP_NOT_FOUND 0

#ifdef _WIN64
uint8_t getaddrinfo_k(long unsigned int*, const char*, uint32_t); //k for kevin
#else
uint8_t getaddrinfo_k(uint32_t*, const char*, uint32_t); //k for kevin
#endif

#endif
