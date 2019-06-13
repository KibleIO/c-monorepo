#include "DNS_WRAPPER.h"
#include "dns.h"

//time out is in 100th's of a second

uint8_t getaddrinfo_k(
uint32_t* ip_addr, const char* dns_address, uint32_t timeout) {
	int32_t 				rc;
	struct addrinfo			hints;
	struct dns_resolv_conf*	dns_conf;
    struct dns_hosts*		dns_hosts;
    struct dns_hints*		dns_hints;
    struct dns_resolver*	resolver;
	struct dns_addrinfo*	ai;
	struct addrinfo*		it;

	dns_conf	= dns_resconf_local(&rc);
    assert(dns_conf);
    dns_hosts	= dns_hosts_local(&rc);
    assert(dns_hosts);
    dns_hints	= dns_hints_local(dns_conf, &rc);
    assert(dns_hints);
    resolver	= dns_res_open(
	dns_conf, dns_hosts, dns_hints, NULL, dns_opts(), &rc);
    assert(resolver);
	it			= NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;

    ai = dns_ai_open(dns_address, "80", DNS_T_A, &hints, resolver, &rc);
    assert(ai);

	while (timeout-- > 0) {
        rc = dns_ai_nextent(&it, ai);
        switch(rc) {
                case 0:
						*ip_addr = (
						(struct sockaddr_in *) it->ai_addr)->sin_addr.s_addr;
                        free(it);
                        goto exit_loop;
                case EAGAIN:
                        rc = dns_ai_poll(ai, 1);
                        assert(rc == 0);
                        break;
                default:
                        goto exit_loop;
        }
        usleep(100 * 1000);
	}

	exit_loop:

	dns_ai_close		(ai);
	dns_res_close		(resolver);
	dns_hints_close		(dns_hints);
	dns_hosts_close		(dns_hosts);
	dns_resconf_close	(dns_conf);

	switch(rc) {
		case 0:
			return 1;
		case EAGAIN:
			printf("DNS_WRAPPER: timed out\n");
			break;
		case ENOENT:
			printf("DNS_WRAPPER: file doesn't exist\n");
			break;
		default:
			printf("DNS_WRAPPER: unknown error\n");
			break;
	}

	return 0;
}
