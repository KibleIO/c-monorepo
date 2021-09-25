#ifndef _WIN64

#include "DNS_WRAPPER.h"

uint32_t ip_ret_DNS_WRAPPER;
std::mutex mutex_DNS_WRAPPER;

void state_cb(void* data, int32_t s, int32_t read, int32_t write) {
	(void) data;
	(void) s;
	(void) read;
	(void) write;
}

void callback(void* arg, int32_t status, int32_t timeouts, hostent* host) {
	(void) arg;
	(void) status;
	(void) timeouts;
	if(!host || status != ARES_SUCCESS) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to lookup"},
			{"error", "ares_strerror(status)"},
			JSON_TYPE_END}));
		ip_ret_DNS_WRAPPER = IP_NOT_FOUND;
	} else {
		ip_ret_DNS_WRAPPER = *((uint32_t*)*host->h_addr_list);
	}
}

uint8_t getaddrinfo_k(uint32_t* ip_addr, const char* dns_address,
	uint32_t timeout_val) {

	ares_channel	channel;
	ares_options	options;
	int32_t			status;
	int32_t			optmask;
	int32_t			nfds;
	timeval			tv;
	fd_set			read_fds;
	fd_set			write_fds;
	timeval*		timeout;
	timeval*		tvp;

	optmask				= 0;
	timeout				= new timeval;
	timeout->tv_sec		= timeout_val;
	timeout->tv_usec	= 0;
	ip_ret_DNS_WRAPPER	= IP_NOT_FOUND;

	status = ares_library_init(ARES_LIB_INIT_ALL);
	if (status != ARES_SUCCESS) {
		log_err(((const JSON_TYPE){
			{"message", "ares_library_init"},
			{"error", "ares_strerror(status)"},
			JSON_TYPE_END}));
		return false;
	}

	options.sock_state_cb = state_cb;
	optmask |= ARES_OPT_SOCK_STATE_CB;
	status = ares_init_options(&channel, &options, optmask);
	if (status != ARES_SUCCESS) {
		log_err(((const JSON_TYPE){
			{"message", "ares_init_options"},
			{"error", "ares_strerror(status)"},
			JSON_TYPE_END}));
		return false;
	}

	mutex_DNS_WRAPPER.lock();

	ares_gethostbyname(channel, dns_address, AF_INET, callback, NULL);
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	nfds = ares_fds(channel, &read_fds, &write_fds);
	if (nfds == 0) {
		log_err(((const JSON_TYPE){
			{"message", "ares_fds"},
			{"error", "ares_strerror(status)"},
			JSON_TYPE_END}));
		goto cleanup;
	}
	tvp = ares_timeout(channel, timeout, &tv);

	if (select(nfds, &read_fds, &write_fds, NULL, tvp) <= 0) {
		log_err(((const JSON_TYPE){
			{"message", "Timed out"},
			JSON_TYPE_END}));
		goto cleanup;
	} else {
		ares_process(channel, &read_fds, &write_fds);
	}
	cleanup:

	delete timeout;

	mutex_DNS_WRAPPER.unlock();

	ares_destroy(channel);
	ares_library_cleanup();

	if (ip_ret_DNS_WRAPPER == IP_NOT_FOUND) {
		return false;
	}

	*ip_addr = ip_ret_DNS_WRAPPER;

	return true;
}

#endif
