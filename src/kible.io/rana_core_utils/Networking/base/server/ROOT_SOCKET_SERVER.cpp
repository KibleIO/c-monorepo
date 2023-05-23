#include "ROOT_SOCKET_SERVER.h"

bool Initialize_ROOT_SOCKET_SERVER(ROOT_SOCKET_SERVER *server, KCONTEXT *ctx,
	int port) {

	server->type = ctx->core_services_backbone;

	switch (server->type) {
		case ROOT_SOCKET_TYPE_WS:
			if (!Initialize_WS_SERVER_MASTER(
				&server->ws_server_master, ctx, port)) {
				
				return false;
			}
			server->initialized = true;
			return true;
		default:
			return false;
	}
}

void Delete_ROOT_SOCKET_SERVER(ROOT_SOCKET_SERVER *server) {
	switch (server->type) {
		case ROOT_SOCKET_TYPE_WS:
			Delete_WS_SERVER_MASTER(&server->ws_server_master);
			break;
	}
}