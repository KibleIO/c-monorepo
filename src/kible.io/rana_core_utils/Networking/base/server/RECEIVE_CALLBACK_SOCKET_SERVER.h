#ifndef _RECEIVE_CALLBACK_SOCKET_SERVER_H_
#define _RECEIVE_CALLBACK_SOCKET_SERVER_H_

#define ROOT_SOCKET_TYPE_TCP 0
#define ROOT_SOCKET_TYPE_UDP 1
#define ROOT_SOCKET_TYPE_WS 2

typedef void (*Receive_Callback_SOCKET_SERVER)(void*, char*, int);

#endif