// PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef UTILS_H_
#define UTILS_H_

#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <uuid/uuid.h>

#include <algorithm>
#include <iostream>
#include <vector>

#define OCTETS_IN_MAC_ADDRESS 6
#define DIGITS_IN_OCTET_IN_MAC_ADDRESS 2
#define MAC_ADDRESS_STR_LEN 18
#define UUID_STR_SIZE 37
#define CORE_SYSTEM_STR_SIZE 37
#define SIOCGIFCONF_BUFFER_SIZE 1024
#define MAC_ADDRESS_RETRY 5
#define MAC_ADDRESS_RETRY_SLEEP 1000

using namespace std;

#define NULLIFY \
    {}

#ifdef __CORE_SYSTEM__
#define _DO_THIS_IN_MAIN_ \
    void get_core_system(char *str) { strcpy(str, __CORE_SYSTEM__); }
#else
#define _DO_THIS_IN_MAIN_ \
    void get_core_system(char *str) { strcpy(str, "PLS DEFINE CORE SYSTEM"); }
#endif

#define ITOA_STR_SIZE 100

#define abs(x) ((x)<0 ? -(x) : (x))

string system_output(string);

string trim(const string &);

vector<string> split(const string &, const string &);

uint8_t file_exists(string);

void generate_uuid(char *);

void get_mac_address(char *);

void get_core_system(char *);

void Sleep_Milli(unsigned int);

const char *itoa(const char *, int);

#endif
