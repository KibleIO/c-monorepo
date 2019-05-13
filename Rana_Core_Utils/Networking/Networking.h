//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include "ENCRYPTION_ENGINE.h"
#include "../Utilities/LOGGING.h"

// Linux specific includes {{{
#ifdef __linux__
#define socklen_t int
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
// }}} Windows specific includes {{{
#ifdef _WIN64
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
// }}} OSX specific includes {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}

#define NETWORKING_BUFFER_SIZE 3000
#define NETWORKING_NO_TIMEOUT 0

using namespace std;

#endif /* NETWORKING_H_ */
