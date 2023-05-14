#ifndef WS_SERVER_MASTER_H_
#define WS_SERVER_MASTER_H_

#include <string.h>
#include <thread>
#include <libwebsockets.h>
#include "../../Utilities/KCONTEXT.h"
#include "../../Utilities/CONCURRENT_QUEUE.h"
#include "../../Utilities/UTILS.h"

#define WEB_SOCKET_POOL_SIZE 80
#define MAX_WEBSOCKET_PACKET_SIZE 100000
#define WEB_SOCKET_SLEEP_TIME 1
#define LWS_SLEEP_TIME 0
#define MAX_HOSTS 20
#define NUM_PROTOCOLS 2
#define MAX_ACCUMULATED_FRAMES 4
#define WEB_SOCKET_TIME_OUT 10
#define WS_SLEEP_TIME 1
#define WS_RECV_TIMEOUT 5

#define DEFAULT_PROTOCOLS(PROTO) PROTO[0] = {\
	"dumb-increment-protocol",\
	callback_dumb_increment,\
	65536,\
	65536};\
	PROTO[1] = {\
	NULL,\
	NULL,\
	0};

struct WEBSOCKET_ELEMENT {
	int32_t		size;
	uint8_t*	bytes;
};

struct WS_SERVER_MASTER {
	char name[100];
	lws_context *context;
	lws_vhost *vhost;
	thread *main_thread;
	int port;
	uint8_t	running;
	uint8_t	accept;
	uint8_t host_count;
	KCONTEXT *ctx;

	Queue<WEBSOCKET_ELEMENT*>	*pool;
	Queue<WEBSOCKET_ELEMENT*>	*active_write;
	Queue<WEBSOCKET_ELEMENT*>	*active_read[MAX_HOSTS];
};

bool Initialize_WS_SERVER_MASTER(WS_SERVER_MASTER*, KCONTEXT*, int);
void Set_Name_WS_SERVER_MASTER(WS_SERVER_MASTER*, char*);
bool Set_Recv_Timeout_WS_SERVER_MASTER(WS_SERVER_MASTER*, int, int);
bool Set_High_Priority_WS_SERVER_MASTER(WS_SERVER_MASTER*);
void Delete_WS_SERVER_MASTER(WS_SERVER_MASTER*);

void Service_Thread_WS_SERVER_MASTER(WS_SERVER_MASTER*);
bool Send_WS_SERVER_MASTER(WS_SERVER_MASTER*, uint8_t*, uint32_t, uint8_t);
bool Receive_WS_SERVER_MASTER(WS_SERVER_MASTER*, uint8_t*, int32_t, int32_t,
	uint8_t);
int Receive_Unsafe_WS_SERVER_MASTER(WS_SERVER_MASTER*, uint8_t*, int32_t,
	uint8_t);
uint8_t Register_Vhost_WS_SERVER_MASTER(WS_SERVER_MASTER*);

#endif