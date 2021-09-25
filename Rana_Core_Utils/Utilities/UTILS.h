//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  UTILS_H_
#define  UTILS_H_

#include <iostream>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OCTETS_IN_MAC_ADDRESS 6
#define DIGITS_IN_OCTET_IN_MAC_ADDRESS 2
#define MAC_ADDRESS_STR_LEN 18
#define UUID_STR_SIZE 37
#define CORE_SYSTEM_STR_SIZE 37
#define SIOCGIFCONF_BUFFER_SIZE 1024
#define MAC_ADDRESS_RETRY 5
#define MAC_ADDRESS_RETRY_SLEEP 1000

using namespace std;

#define NULLIFY {}

#ifdef __CORE_SYSTEM__
#define _DO_THIS_IN_MAIN_ void get_core_system(char *str) {\
				strcpy(str, __CORE_SYSTEM__);\
			}
#else
#define _DO_THIS_IN_MAIN_ void get_core_system(char *str) {\
				strcpy(str, "PLS DEFINE CORE SYSTEM");\
			}
#endif

string system_output(string);

uint8_t file_exists(string);

void generate_uuid(char*);

void get_mac_address(char*);

void get_core_system(char*);

void Sleep_Milli(unsigned int);

#endif
