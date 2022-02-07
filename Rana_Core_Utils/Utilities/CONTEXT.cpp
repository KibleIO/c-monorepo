#include "CONTEXT.h"

bool Initialize_CONTEXT(CONTEXT *ctx, char *core_system) {
	generate_uuid(ctx->uuid);
	get_mac_address(ctx->mac_address);
	strcpy(ctx->core_system, core_system);

	if (!Initialize_ELASTIC_SEARCH_CLIENT(&ctx->client)) {
		cout << "failed to initialize elastic search client" << endl;
		return false;
	}

	std::unique_ptr<project::GAIA::Stub> stub_;

	stub_ = project::GAIA::NewStub(grpc::CreateChannel("api.kible.io:50052",
		grpc::InsecureChannelCredentials()));

	grpc::ClientContext context;
	grpc::ClientContext context2;
	project::UUID uuid;
	project::Empty empty;
	project::Rana rana;
	grpc::Status status;

	status = stub_->CreateEmptyRana(&context, empty, &uuid);
	if (!status.ok()) {
		std::cout << "CreateItem rpc failed." << std::endl;
	}

	cout << "hello world " << uuid.value() << endl;

	status = stub_->GetRana(&context2, uuid, &rana);
	if (!status.ok()) {
		std::cout << "CreateItem rpc failed." << std::endl;
	}

	cout << "hello world " << rana.uuid().value() << endl;

	return true;
}

SCREEN_DIM Get_Screen_Dim_CONTEXT(CONTEXT *ctx) {
	return ctx->screen_dim;
}

void Set_Screen_Dim_CONTEXT(CONTEXT *ctx, SCREEN_DIM screen_dim) {
	ctx->screen_dim = screen_dim;
}

void Log_CONTEXT(CONTEXT *ctx, char *str) {
	Post_ELASTIC_SEARCH_CLIENT(&ctx->client, str);
}

void Delete_CONTEXT(CONTEXT *ctx) {
	Delete_ELASTIC_SEARCH_CLIENT(&ctx->client);
}
