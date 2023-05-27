#include "SOCKET_SERVER_REGISTRY.h"

bool Initialize_SOCKET_SERVER_REGISTRY(SOCKET_SERVER_REGISTRY *registry,
	KCONTEXT *ctx) {
	
	for (int i = 0; i < MAX_ROOT_SOCKETS; i++) {
		registry->root_sockets[i] = NULL;
	}

	registry->root_sockets[ctx->core_services_backbone] =
				new ROOT_SOCKET_SERVER;
	return Initialize_ROOT_SOCKET_SERVER(
		registry->root_sockets[ctx->core_services_backbone], ctx,
		ctx->core_services_backbone_port);
}

void Delete_SOCKET_SERVER_REGISTRY(SOCKET_SERVER_REGISTRY *registry) {
	for (int i = 0; i < MAX_ROOT_SOCKETS; i++) {
		if (registry->root_sockets[i] != NULL) {
			Delete_ROOT_SOCKET_SERVER(registry->root_sockets[i]);
			delete registry->root_sockets[i];
			registry->root_sockets[i] = NULL;
		}
	}
}