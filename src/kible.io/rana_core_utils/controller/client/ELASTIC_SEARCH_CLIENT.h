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
#include "../../Utilities/UTILS.h"

#define ELASTIC_SEARCH_URL "https://hub.alienhub.xyz/elk/_doc/?pretty"
#define ELK_USERNAME "admin"
#define ELK_PASSWORD "6ZSAZG5fE7Ip"
#define ELK_TIMEOUT 100L

struct ELASTIC_SEARCH_CLIENT {
	CURL *curl;
	curl_slist *hs;
	char *payload_ptr;
	uint32_t payload_size;
	mutex mutex_;
	string recent_error;
};

bool Initialize_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);
bool Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*, char*);
bool Custom_Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*, char*, char*);
bool Custom2_Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*, char*, char*);
void Delete_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT*);

#endif
