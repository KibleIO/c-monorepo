#ifndef _ELASTIC_SEARCH_CLIENT_H_
#define _ELASTIC_SEARCH_CLIENT_H_

#ifndef _WIN64
#include <sys/syscall.h>
#endif

#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include "UTILS.h"

#define ELASTIC_SEARCH_URL "http://elk.kible.io:9200/kible/_doc/?pretty"

struct ELASTIC_SEARCH_CLIENT {
	CURL *curl;
	curl_slist *hs;
	char *payload_ptr;
	uint32_t payload_size;
	mutex mutex_;
};

bool Initialize_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);
bool Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*, char*);
void Delete_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);

#endif
