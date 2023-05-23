#include "SOCKET_SERVER.h"

bool Initialize_SOCKET_SERVER(SOCKET_SERVER *server,
	Receive_Callback_SOCKET_SERVER callback,
	SOCKET_SERVER_REGISTRY *registry, KCONTEXT *ctx, void *user_ptr) {

	server->type = ctx->core_services_backbone;

	switch (server->type) {
		case ROOT_SOCKET_TYPE_WS:
			return Initialize_WS_SERVER(
				&server->ws_server, callback, registry, ctx,
				user_ptr);
		default:
			return false;
	}

}

bool Send_SOCKET_SERVER(SOCKET_SERVER *server, char *buffer, int size) {
	switch (server->type) {
		case ROOT_SOCKET_TYPE_WS:
			return Send_WS_SERVER(&server->ws_server, buffer, size);
		default:
			return false;
	}
}

void Delete_SOCKET_SERVER(SOCKET_SERVER *server) {
	switch (server->type) {
		case ROOT_SOCKET_TYPE_WS:
			Delete_WS_SERVER(&server->ws_server);
	}
}