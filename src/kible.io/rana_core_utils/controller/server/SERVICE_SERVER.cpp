#include "SERVICE_SERVER.h"

bool Initialize_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY *registry,
	KCONTEXT *ctx, int count, ...) {

	registry->service_count = 0;

	for (int i = 0; i < MAX_SERVICES; i++) {
		registry->service_server[i] = NULL;
	}

	if (!Initialize_SOCKET_SERVER_REGISTRY(
		&registry->socket_server_registry)) {
		
		return false;
	}

	if (!Initialize_ROOT_SOCKET_SERVER(
		&registry->socket_server_registry.root_sockets
		[ROOT_SOCKET_TYPE_WS], ctx, ctx->core_services_backbone_port)) {
		
		return false;
	}

	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++) {
		registry->service_server[i] = va_arg(args, SERVICE_SERVER*);
		if (!registry->service_server[i]->Initialize(ctx, registry)) {
			return false;
		}
	}
	va_end(args);

	registry->service_count = count;

	return true;
}

void Delete_SERVICE_SERVER_REGISTRY(SERVICE_SERVER_REGISTRY *registry) {
	for (int i = 0; i < MAX_SERVICES; i++) {
		if (registry->service_server[i] != NULL) {
			registry->service_server[i]->Delete();
			delete registry->service_server[i];
			registry->service_server[i] = NULL;
		}
	}

	Delete_SOCKET_SERVER_REGISTRY(&registry->socket_server_registry);
}