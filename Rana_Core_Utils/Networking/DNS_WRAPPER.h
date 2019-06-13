#ifndef DNS_WRAPPER_H_
#define DNS_WRAPPER_H_

#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

uint8_t getaddrinfo_k(uint32_t*, const char*, uint32_t); //k for kevin

#endif
