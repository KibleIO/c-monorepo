#include "CONTEXT.h"

bool Initialize_CONTEXT(CONTEXT *ctx, char *core_system) {
	generate_uuid(ctx->uuid);
	get_mac_address(ctx->mac_address);
	strcpy(ctx->core_system, core_system);

	if (!Initialize_ELASTIC_SEARCH_CLIENT(&ctx->client)) {
		cout << "failed to initialize elastic search client" << endl;
		return false;
	}

	return true;
}

void Log_CONTEXT(CONTEXT *ctx, char *str) {
	Post_ELASTIC_SEARCH_CLIENT(&ctx->client, str);
}

void Delete_CONTEXT(CONTEXT *ctx) {
	Delete_ELASTIC_SEARCH_CLIENT(&ctx->client);
}
