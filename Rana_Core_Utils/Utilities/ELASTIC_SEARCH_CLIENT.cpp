#include "ELASTIC_SEARCH_CLIENT.h"

bool Initialize_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client) {
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	client->hs = NULL;
	client->hs = curl_slist_append(client->hs,
		"Content-Type: application/json");

	generate_uuid(client->uuid);
	get_mac_address(client->mac_address);
	get_core_system(client->core_system);

	client->curl = curl_easy_init();

	return client->curl;
}

size_t Read_Callback_ELASTIC_SEARCH_CLIENT(char *ptr, size_t size, size_t nmemb,
	void *stream) {

	ELASTIC_SEARCH_CLIENT *client = (ELASTIC_SEARCH_CLIENT*) stream;
	int bytes_to_read;

	bytes_to_read = (size * nmemb > client->payload_size) ?
		client->payload_size : size * nmemb;

	memcpy(ptr, client->payload_ptr, bytes_to_read);

	client->payload_size -= bytes_to_read;
	client->payload_ptr += bytes_to_read;

	return (curl_off_t) bytes_to_read / size;
}

bool Convert_Hacky_JSON_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client,
	const JSON_TYPE json, const char *file, const char *line,
	const char *function, const char *type) {

	char buffer [TEMP_BUFFER_SIZE];
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	std::stringstream ssTp;
	ssTp << put_time(gmtime(&now_t), "%Y-%m-%dT%H:%M:%S");
	int thread = syscall(__NR_gettid);
	int ret;
	int i = 0;


	client->payload[0] = '\0'; //lets reset that string

	strcat(client->payload, "{");

	//ewwww....
	strcat(client->payload, "\"");
	strcat(client->payload, "@_timestamp");
	strcat(client->payload, "\": \"");
	strcat(client->payload, ssTp.str().c_str());
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "file");
	strcat(client->payload, "\": \"");
	strcat(client->payload, file);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "line");
	strcat(client->payload, "\": \"");
	strcat(client->payload, line);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "function");
	strcat(client->payload, "\": \"");
	strcat(client->payload, function);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "type");
	strcat(client->payload, "\": \"");
	strcat(client->payload, type);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "system");
	strcat(client->payload, "\": \"");
	strcat(client->payload, client->core_system);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "uuid");
	strcat(client->payload, "\": \"");
	strcat(client->payload, client->uuid);
	strcat(client->payload, "\",");

	strcat(client->payload, "\"");
	strcat(client->payload, "mac_address");
	strcat(client->payload, "\": \"");
	strcat(client->payload, client->mac_address);
	strcat(client->payload, "\",");

	ret = snprintf(buffer, TEMP_BUFFER_SIZE, "%d", thread);
	if (ret > 0) {
		strcat(client->payload, "\"");
		strcat(client->payload, "thread");
		strcat(client->payload, "\": \"");
		strcat(client->payload, buffer);
		strcat(client->payload, "\"");
	}

	while (json[i][JSON_TYPE_KEY][0] != '\0') {
		strcat(client->payload, ",");
		strcat(client->payload, "\"");
		strcat(client->payload, json[i][JSON_TYPE_KEY]);
		strcat(client->payload, "\": \"");
		strcat(client->payload, json[i][JSON_TYPE_VALUE]);
		strcat(client->payload, "\"");

		i++;
	}

	strcat(client->payload, "}\n");

	return true;

}

//throw away function
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	(void) buffer;
	(void) userp;
	return size * nmemb;
}

//ugh.. const.. you win this one Kord
bool Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client,
	const JSON_TYPE json, const char *file, const char *line,
	const char *function, const char* type) {

	CURLcode res;

	client->mutex_.lock();
	if (!Convert_Hacky_JSON_ELASTIC_SEARCH_CLIENT(client, json, file, line,
		function, type)) {

		return false;
	}

	client->payload_ptr = client->payload;
	client->payload_size = strlen(client->payload);

	/* specify target URL, and note that this URL should include a
	file name, not only a directory */
	curl_easy_setopt(client->curl, CURLOPT_URL, ELASTIC_SEARCH_URL);

	/* enable uploading (implies PUT over HTTP) */
	curl_easy_setopt(client->curl, CURLOPT_POST, 1L);

	curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, client->hs);

	/* we want to use our own read function */
	curl_easy_setopt(client->curl, CURLOPT_READFUNCTION,
		Read_Callback_ELASTIC_SEARCH_CLIENT);

	/* now specify which file to upload */
	curl_easy_setopt(client->curl, CURLOPT_READDATA, client);

	/* provide the size of the upload, we specicially typecast the
	value to curl_off_t since we must be sure to use the correct
	data size */
	curl_easy_setopt(client->curl, CURLOPT_POSTFIELDSIZE,
	     (long)client->payload_size);

	curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_data);

	/* Now run off and do what you've been told! */
	res = curl_easy_perform(client->curl);

	client->mutex_.unlock();

	/* Check for errors */
	return (res != CURLE_OK);
}

void Delete_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client) {
	(void) client;
	/* always cleanup */
	curl_easy_cleanup(client->curl);
	curl_global_cleanup();
}
