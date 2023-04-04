//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef NETWORKING_H_
#define NETWORKING_H_

#ifdef _WIN64
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "../Utilities/LOGGING.h"

#define NETWORKING_BUFFER_SIZE 3000
#define NETWORKING_NO_TIMEOUT 0

#define NETWORK_TYPE_TCP 1
#define NETWORK_TYPE_UDP 2
#define NETWORK_TYPE_WS 3

#define ARBITRARILY_LARGE_PACKET 100000
#define MAX_NAME_SIZE 100

#define TEST_BUFF_SIZE 4

#define DEFAULT_RECV_TIMEOUT 1
#define DEFAULT_CONNECT_TIMEOUT 500000
#define DEFAULT_ACCEPT_TIMEOUT 5
#define RECV_ATTEMPTS 100
#define MAX_UDP_PACKET_SIZE 1454
#define MAX_UDP_CONNECTIONS 20
#define POOL_QUEUE_SIZE 50

#define IP_ADDR_STR_LEN 15

struct UDP_PACKET {
	char buffer[MAX_UDP_PACKET_SIZE + 1]; //plus 1 for the type
	int size;
};

#endif /* NETWORKING_H_ */
