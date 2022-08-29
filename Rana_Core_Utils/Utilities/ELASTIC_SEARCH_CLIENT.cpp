#include "ELASTIC_SEARCH_CLIENT.h"

bool Initialize_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client) {
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	client->hs = NULL;
	client->hs = curl_slist_append(client->hs,
		"Content-Type: application/json");

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

//throw away function
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	(void) buffer;
	(void) userp;
	//cout << (char*) buffer << endl;
	return size * nmemb;
}

bool Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client, char *str) {
	CURLcode res;

	client->mutex_.lock();

	client->payload_ptr = str;
	client->payload_size = strlen(client->payload_ptr);

	/* specify target URL, and note that this URL should include a
	file name, not only a directory */
	curl_easy_setopt(client->curl, CURLOPT_URL, ELASTIC_SEARCH_URL);

	/* enable uploading (implies PUT over HTTP) */
	curl_easy_setopt(client->curl, CURLOPT_POST, 1L);

	/* complete within 20 seconds */
  	curl_easy_setopt(client->curl, CURLOPT_TIMEOUT, 20L);

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

	curl_easy_setopt(client->curl, CURLOPT_USERNAME, ELK_USERNAME);

	curl_easy_setopt(client->curl, CURLOPT_PASSWORD, ELK_PASSWORD);

	/* Now run off and do what you've been told! */
	res = curl_easy_perform(client->curl);

	client->mutex_.unlock();

	if (res != CURLE_OK) {
		client->recent_error = string(curl_easy_strerror(res));
	}

	/* Check for errors */
	return (res == CURLE_OK);
}

bool Custom_Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client, char *str, char *url) {
	CURLcode res;

	client->mutex_.lock();

	client->payload_ptr = str;
	client->payload_size = strlen(client->payload_ptr);

	/* specify target URL, and note that this URL should include a
	file name, not only a directory */
	curl_easy_setopt(client->curl, CURLOPT_URL, url);

	/* Set the default value: strict certificate check please */
  	curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYHOST, 0L);

	/* complete within X seconds */
  	curl_easy_setopt(client->curl, CURLOPT_TIMEOUT, ELK_TIMEOUT);

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

	curl_easy_setopt(client->curl, CURLOPT_USERNAME, ELK_USERNAME);

	curl_easy_setopt(client->curl, CURLOPT_PASSWORD, ELK_PASSWORD);

	/* Now run off and do what you've been told! */
	res = curl_easy_perform(client->curl);

	client->mutex_.unlock();

	if(res != CURLE_OK) {
		client->recent_error = string(curl_easy_strerror(res));
	}

	/* Check for errors */
	return (res == CURLE_OK);
}

bool Custom2_Post_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client, char *str, char *url) {
	CURLcode res;

	client->mutex_.lock();

	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "api2.kible.com:443:45.57.227.210");

	client->payload_ptr = str;
	client->payload_size = strlen(client->payload_ptr);

	curl_easy_setopt(client->curl, CURLOPT_RESOLVE, host);

	/* specify target URL, and note that this URL should include a
	file name, not only a directory */
	curl_easy_setopt(client->curl, CURLOPT_URL, url);

	/* Set the default value: strict certificate check please */
  	curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(client->curl, CURLOPT_SSL_VERIFYHOST, 0L);

	/* complete within X seconds */
  	curl_easy_setopt(client->curl, CURLOPT_TIMEOUT, ELK_TIMEOUT);

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

	curl_easy_setopt(client->curl, CURLOPT_USERNAME, ELK_USERNAME);

	curl_easy_setopt(client->curl, CURLOPT_PASSWORD, ELK_PASSWORD);

	/* Now run off and do what you've been told! */
	res = curl_easy_perform(client->curl);

	client->mutex_.unlock();

	if(res != CURLE_OK) {
		client->recent_error = string(curl_easy_strerror(res));
	}

	/* Check for errors */
	return (res == CURLE_OK);
}

void Delete_ELASTIC_SEARCH_CLIENT(ELASTIC_SEARCH_CLIENT *client) {
	curl_easy_cleanup(client->curl);
	curl_global_cleanup();
}
