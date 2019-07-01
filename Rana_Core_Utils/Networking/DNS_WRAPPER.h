#ifndef DNS_WRAPPER_H_
#define DNS_WRAPPER_H_

#include <ares.h>
#include <netdb.h>
#include <mutex>
#include "../Utilities/LOGGING.h"

extern uint32_t ip_ret_DNS_WRAPPER;
extern std::mutex mutex_DNS_WRAPPER;

#define IP_NOT_FOUND 0

uint8_t getaddrinfo_k(uint32_t*, const char*, uint32_t); //k for kevin

#endif
