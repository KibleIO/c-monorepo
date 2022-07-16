// PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef UTILS_H_
#define UTILS_H_

#include <fstream>
#include <time.h>

#ifdef __linux__

#include <linux/if.h>
#include <sys/sysinfo.h>

#elif _WIN64

#include <Winsock2.h>
#include <chrono>

#elif __APPLE__

#include <net/if.h>
#include <mach/clock.h>
#include <mach/mach.h>

#endif

#ifndef _WIN64

#include <uuid/uuid.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#endif

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <vector>

#include "clip/clip.h"
#include "Update_Utility.h"

#define OCTETS_IN_MAC_ADDRESS 6
#define DIGITS_IN_OCTET_IN_MAC_ADDRESS 2
#define MAC_ADDRESS_STR_LEN 18
#define UUID_STR_SIZE 37
#define CORE_SYSTEM_STR_SIZE 37
#define MAC_ADDRESS_RETRY 5
#define MAC_ADDRESS_RETRY_SLEEP 1000
#define VERSION_STRING_LENGTH 30

using namespace std;

#define NULLIFY {}

#define ITOA_STR_SIZE 100

#define k_abs(x) ((x)<0 ? -(x) : (x))

#define CEILING(x, y) (((x) + (y)-1) / (y))

string system_output(string);

string trim(const string &);

vector<string> split(const string &, const string &);

uint8_t file_exists(string);

void generate_uuid(char *);

void get_mac_address(char *);

void get_current_time(char *);

void Sleep_Milli(unsigned int);

bool Write_Bin_To_File(char*, char*, int);

bool Read_Bin_From_File(char*, char*, int);

bool Check_If_White_Space(char *);

const char *itoa(const char *, int);

bool Is_Connected_To_Internet();

//These are here because of a hack... ignore :(
void Get_Clipboard();
void Set_Clipboard();

#endif
