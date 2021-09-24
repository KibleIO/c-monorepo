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
#define NETWORK_CARD_NAME "eth0"
#define UUID_STR_SIZE 37

using namespace std;

#define NULLIFY {}

string system_output(string);

uint8_t file_exists(string);

void generate_uuid(char*);

void get_mac_address(char*);

#endif
