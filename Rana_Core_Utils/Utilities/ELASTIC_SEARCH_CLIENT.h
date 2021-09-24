#ifndef _ELASTIC_SEARCH_CLIENT_H_
#define _ELASTIC_SEARCH_CLIENT_H_

#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/syscall.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include "UTILS.h"

using namespace std;

#define TEMP_BUFFER_SIZE 100
#define ELASTIC_SEARCH_URL "http://elk.kible.io:9200/kible/_doc/?pretty"
#define MAX_KEY_VALUE_LEN 128
#define ELEMENTS_IN_JSON_PAIR 2
#define MAX_PAYLOAD_KEYS 50
#define MAX_PAYLOAD_SIZE MAX_KEY_VALUE_LEN * MAX_PAYLOAD_KEYS * \
	ELEMENTS_IN_JSON_PAIR
//quick and dirty JSON implementation... probably should be removed
#define JSON_TYPE_KEY 0
#define JSON_TYPE_VALUE 1
#define JSON_TYPE_END {"\0", "\0"}
typedef char* JSON_TYPE[][2];

struct ELASTIC_SEARCH_CLIENT {
	CURL *curl;
	curl_slist *hs;
	char payload[MAX_PAYLOAD_SIZE];
	char *payload_ptr;
	uint32_t payload_size;
	char mac_address[MAC_ADDRESS_STR_LEN];
	char uuid[UUID_STR_SIZE];
	char core_system[CORE_SYSTEM_STR_SIZE];
};

bool Initialize_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);
bool Convert_JSON_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*,
	const JSON_TYPE, const char*, const char*, const char*, const char*);

bool Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*, const JSON_TYPE,
	const char*, const char*, const char*, const char*);

void Delete_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);

#endif
