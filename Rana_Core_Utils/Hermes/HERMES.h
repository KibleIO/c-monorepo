//PLATFORMS: Linux, Windows, OSX

#ifndef HERMES_H_
#define HERMES_H_

#include <string>
#include "../Networking/NETWORK.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/UTILS.h"

#define HERMES_CONNECTIONS_MAX MAX_UDP_CONNECTIONS

#define HERMES_PORT_MIN 2230
#define HERMES_PORT_MAX 3329

#define HERMES_NULL		(HERMES_TYPE){"null", NETWORK_TYPE_TCP, 0}
//NETWORK_TYPE_UDP
#define HERMES_AUDIO		(HERMES_TYPE){"audio", NETWORK_TYPE_TCP, 1}
#define HERMES_DEVICE		(HERMES_TYPE){"device", NETWORK_TYPE_UDP, 2}
//NETWORK_TYPE_UDP
#define HERMES_VIDEO		(HERMES_TYPE){"video", NETWORK_TYPE_TCP, 3}
#define HERMES_PRINT		(HERMES_TYPE){"print", NETWORK_TYPE_TCP, 4}
#define HERMES_DATA		(HERMES_TYPE){"data", NETWORK_TYPE_TCP, 5}
#define HERMES_MIC		(HERMES_TYPE){"mic", NETWORK_TYPE_TCP, 6}
#define HERMES_CAM		(HERMES_TYPE){"cam", NETWORK_TYPE_TCP, 7}
#define HERMES_THEMIS		(HERMES_TYPE){"themis", NETWORK_TYPE_TCP, 8}
#define HERMES_VIDEO_INIT	(HERMES_TYPE){"video_init", NETWORK_TYPE_TCP, 9}
//NETWORK_TYPE_UDP
#define HERMES_KEYBOARD		(HERMES_TYPE){"keyboard", NETWORK_TYPE_TCP, 10}
//NETWORK_TYPE_UDP
#define HERMES_MOUSE		(HERMES_TYPE){"mouse", NETWORK_TYPE_TCP, 11}
#define HERMES_CLIENT_T		(HERMES_TYPE){"hermes client", NETWORK_TYPE_TCP, 12}
#define HERMES_SERVER_T		(HERMES_TYPE){"hermes server", NETWORK_TYPE_TCP, 13}

#define HERMES_GET_CONNECTION	1
#define HERMES_EXIT 		2
#define HERMES_STATUS 		3

#define HERMES_TIMEOUT_TRIES 10

#define HERMES_STATUS_NORMAL 0
#define HERMES_STATUS_UNEXPECTED_DISCONNECT 1
#define HERMES_STATUS_EXPECTED_DISCONNECT 2
#define HERMES_STATUS_DISCONNECTED 3

#define HERMES_CORE_TIMEOUT 3

struct HERMES_TYPE {
	uint8_t name[MAX_NAME_SIZE];
	int32_t type;
	int32_t id;
}  __attribute__((packed));

#endif
