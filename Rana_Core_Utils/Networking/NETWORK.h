//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "../Utilities/LOGGING.h"

#define NETWORKING_BUFFER_SIZE 3000
#define NETWORKING_NO_TIMEOUT 0

#define NETWORK_TYPE_TCP 1
#define NETWORK_TYPE_UDP 2

#define ARBITRARILY_LARGE_PACKET 100000
#define MAX_NAME_SIZE 100

#define TEST_BUFF_SIZE 4

#define DEFAULT_RECV_TIMEOUT 1
#define DEFAULT_CONNECT_TIMEOUT 100000

#endif /* NETWORKING_H_ */
