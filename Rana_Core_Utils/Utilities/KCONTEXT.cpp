#include "KCONTEXT.h"

bool Initialize_KCONTEXT(KCONTEXT *ctx, char *core_system, bool insecure) {
        ctx->uuid = "ERROR";
	ctx->insecure_mode = !insecure;

	generate_uuid(ctx->trace_uuid);
	strcpy(ctx->core_system, core_system);
        strcpy(ctx->system_resource_dir, "ERROR");

        ctx->connection_initialized = false;
	ctx->rana_initialized = false;

	if (!Initialize_ELASTIC_SEARCH_CLIENT(&ctx->client)) {
		cout << "failed to initialize elastic search client" << endl;
		return false;
	}

	return true;
}

int Initialize_Connection_KCONTEXT(KCONTEXT *ctx, string uuid_) {
        std::unique_ptr<gaia::GATEWAY::Stub> stub;
	
	if (ctx->insecure_mode) {

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(
			INSECURE_GRPC_ADDRESS,
			grpc::InsecureChannelCredentials()));

	} else {
		char cacert_dir[MAX_DIRECTORY_LEN];

		Get_CACERT_Dir(cacert_dir);
		
		#ifdef __linux__

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#else

		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#endif
	}

	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "System resource directory has "
				"not been initialized.");
		}
		return INIT_CONN_KCONTEXT_ABORT;
	}
	
	if (ctx->connection_initialized) {
		LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Ports have already been "
				"initialized.");
		}
		return INIT_CONN_KCONTEXT_SUCCESS;
	}

        if (strcmp(ctx->core_system, "RANA") == 0) {
		grpc::Status status;
		grpc::ClientContext context;
		chrono::system_clock::time_point deadline =
		chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
		context.set_deadline(deadline);

		gaia::RegisterRanaRequest registerRequest;
		gaia::RegisterRanaResponse registerResponse;
		gaia::RanaUUID ranaID;
		gaia::LocationUUID locationID;
		string uuid;

		ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, uuid);
                        file.close();

			ranaID.mutable_uuid()->set_value(uuid);
			registerRequest.mutable_ranaid()->CopyFrom(ranaID);
		}

		if (!uuid_.empty()) {
			ranaID.mutable_uuid()->set_value(uuid_);
			registerRequest.mutable_ranaid()->CopyFrom(ranaID);
		}

		status = stub->RegisterRana(&context, registerRequest, &registerResponse);

		if (status.ok()) {
			ctx->connection = registerResponse.connection();

                        ctx->connection_initialized = true;

			LOG_INFO_CTX(ctx) {
				ADD_STR_LOG("message", "Successfully lets go.");
			}

			return INIT_CONN_KCONTEXT_SUCCESS;
		}

		ctx->recent_error = status.error_message();

		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Lets go failed.");
			ADD_STR_LOG("error", ctx->recent_error.c_str());
			ADD_STR_LOG("type", "register");
		}

		switch (status.error_code()) {
			case grpc::StatusCode::PERMISSION_DENIED:
				return INIT_CONN_KCONTEXT_KEY;
			case grpc::StatusCode::ABORTED:
				return INIT_CONN_KCONTEXT_ABORT;
			case grpc::StatusCode::NOT_FOUND:
				return INIT_CONN_KCONTEXT_EMAIL;
			case grpc::StatusCode::INVALID_ARGUMENT:
				return INIT_CONN_KCONTEXT_LOCATION;
			default:
				return INIT_CONN_KCONTEXT_ABORT;
		}
        } else if (strcmp(ctx->core_system, "THEMIS") == 0) {
		grpc::Status status;
		grpc::ClientContext context;
		chrono::system_clock::time_point deadline =
		chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
		context.set_deadline(deadline);

		gaia::RegisterThemisRequest registerRequest;
		gaia::RegisterThemisResponse registerResponse;
		gaia::ContainerID containerID;

		string containerID_;

		ifstream containerid(CONTAINER_ID_LOC);
		ASSERT_E_R(containerid,
			"Could not open container id location.",
			ctx);
		getline(containerid, containerID_);
		containerid.close();

		containerID.mutable_id()->set_value(containerID_);
		registerRequest.mutable_containerid()->CopyFrom(containerID);
		
		status = stub->RegisterThemis(&context, registerRequest, &registerResponse);
		if (!status.ok()) {
			LOG_ERROR_CTX(ctx) {
				ADD_STR_LOG("message", "Failed to register themis.");
				ADD_STR_LOG("error", status.error_message().c_str());
			}
			return INIT_CONN_KCONTEXT_ABORT;
		}

		ctx->uuid = registerResponse.ranaid().uuid().value();
		ctx->connection = registerResponse.connection();
		ctx->connection_initialized = true;

		return INIT_CONN_KCONTEXT_SUCCESS;
        } else {
                LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Unknown core system");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        }
}

//email and uuid are optional for Themis
int Create_Rana_KCONTEXT(KCONTEXT *ctx, string email_, string uuid_) {
        std::unique_ptr<gaia::GATEWAY::Stub> stub;
	if (ctx->insecure_mode) {

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(
			INSECURE_GRPC_ADDRESS,
			grpc::InsecureChannelCredentials()));

	} else {
		char cacert_dir[MAX_DIRECTORY_LEN];

		Get_CACERT_Dir(cacert_dir);
		
		#ifdef __linux__

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#else

		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#endif
	}

	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "System resource directory has "
				"not been initialized.");
		}
		return INIT_CONN_KCONTEXT_ABORT;
	}

	if (ctx->rana_initialized) {
		LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Rana have already been "
				"initialized.");
		}
		return INIT_CONN_KCONTEXT_SUCCESS;
	}

        if (strcmp(ctx->core_system, "RANA") == 0) {
		grpc::Status status;
		grpc::ClientContext context;
		chrono::system_clock::time_point deadline =
		chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
		context.set_deadline(deadline);

		gaia::CreateRanaRequest registerRequest;
		gaia::CreateRanaResponse registerResponse;
		gaia::RanaUUID ranaID;
		gaia::LocationUUID locationID;
		string uuid;

		ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, uuid);
                        file.close();

			ranaID.mutable_uuid()->set_value(uuid);
			registerRequest.mutable_ranaid()->CopyFrom(ranaID);
		}

		if (!uuid_.empty()) {
			ranaID.mutable_uuid()->set_value(uuid_);
			registerRequest.mutable_ranaid()->CopyFrom(ranaID);
		}

		if (!email_.empty()) {
			registerRequest.mutable_email()->set_value(email_);
		}

		if (ctx->locationID.has_uuid()) {
			locationID.mutable_uuid()->set_value(ctx->locationID.uuid().value());
			registerRequest.mutable_locationid()->CopyFrom(locationID);
		}

		status = stub->CreateRana(&context, registerRequest, &registerResponse);

		if (status.ok()) {
			ctx->uuid = registerResponse.ranaid().uuid().value();

			ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                        ASSERT_E_R(output,
                                "Could not open info location.",
                                ctx);
                        output << registerResponse.ranaid().uuid().value() << endl;
                        output.close();

			ctx->rana_initialized = true;

			return INIT_CONN_KCONTEXT_REGISTER;
		}

		ctx->recent_error = status.error_message();

		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Lets go failed.");
			ADD_STR_LOG("error", ctx->recent_error.c_str());
			ADD_STR_LOG("type", "create");
		}

		switch (status.error_code()) {
			case grpc::StatusCode::PERMISSION_DENIED:
				return INIT_CONN_KCONTEXT_KEY;
			case grpc::StatusCode::ABORTED:
				return INIT_CONN_KCONTEXT_ABORT;
			case grpc::StatusCode::NOT_FOUND:
				return INIT_CONN_KCONTEXT_EMAIL;
			case grpc::StatusCode::INVALID_ARGUMENT:
				return INIT_CONN_KCONTEXT_LOCATION;
			default:
				return INIT_CONN_KCONTEXT_ABORT;
		}
        } else if (strcmp(ctx->core_system, "THEMIS") == 0) {
		LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Do not call create for Themis");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        } else {
                LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Unknown core system");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        }
}

bool Check_For_Update_KCONTEXT(KCONTEXT *ctx, char *verion) {
	std::unique_ptr<gaia::GATEWAY::Stub> stub;

	if (ctx->insecure_mode) {

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(
			INSECURE_GRPC_ADDRESS,
			grpc::InsecureChannelCredentials()));

	} else {
		char cacert_dir[MAX_DIRECTORY_LEN];

		Get_CACERT_Dir(cacert_dir);
		
		#ifdef __linux__

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#else

		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#endif
	}

	gaia::GetVersionStoreRequest request;
	gaia::GetVersionStoreResponse response;

	grpc::Status status;
	grpc::ClientContext context;
	chrono::system_clock::time_point deadline =
	chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
	context.set_deadline(deadline);

	request.mutable_name()->set_value(string(ctx->core_system));

	status = stub->GetVersionStore(&context, request, &response);

	if (!status.ok()) {
		ctx->recent_error = status.error_message();

		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not find version store.");
			ADD_STR_LOG("error", ctx->recent_error.c_str());
		}

		return false;
	}

	strcpy(verion, response.versionstore().version().value().c_str());

	return true;
}

bool Get_Location_KCONTEXT(KCONTEXT *ctx) {
	std::unique_ptr<gaia::GATEWAY::Stub> stub;
	
	if (ctx->insecure_mode) {

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(
			INSECURE_GRPC_ADDRESS,
			grpc::InsecureChannelCredentials()));

	} else {
		char cacert_dir[MAX_DIRECTORY_LEN];

		Get_CACERT_Dir(cacert_dir);
		
		#ifdef __linux__

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#else

		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#endif
	}

	gaia::GetLocationsRequest request;

	grpc::Status status;
	grpc::ClientContext context;
	chrono::system_clock::time_point deadline =
	chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
	context.set_deadline(deadline);

	status = stub->GetLocations(&context, request, &ctx->locations);
	ASSERT_E_R(status.ok(),
	"Could not get locations.",
	ctx);

	return true;
}

bool Get_Products_KCONTEXT(KCONTEXT *ctx) {
	if (!ctx->rana_initialized) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Rana has not already been "
				"initialized.");
		}
		return false;
	}

	std::unique_ptr<gaia::GATEWAY::Stub> stub;
	
	if (ctx->insecure_mode) {

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(
			INSECURE_GRPC_ADDRESS,
			grpc::InsecureChannelCredentials()));

	} else {
		char cacert_dir[MAX_DIRECTORY_LEN];

		Get_CACERT_Dir(cacert_dir);
		
		#ifdef __linux__

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#else

		kible_setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", cacert_dir, 1);

		stub = gaia::GATEWAY::NewStub(grpc::CreateChannel(GRPC_ADDRESS,
			grpc::SslCredentials(grpc::SslCredentialsOptions())));

		#endif
	}

	gaia::GetProductsRequest request;
	gaia::RanaUUID ranaID;

	grpc::Status status;
	grpc::ClientContext context;
	chrono::system_clock::time_point deadline =
	chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
	context.set_deadline(deadline);

	ranaID.mutable_uuid()->set_value(ctx->uuid);
	request.mutable_ranaid()->CopyFrom(ranaID);

	status = stub->GetProducts(&context, request, &ctx->products);

	if (!status.ok()) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not get products.");
		}

		return false;
	}

	return true;
}

SCREEN_DIM Get_Screen_Dim_KCONTEXT(KCONTEXT *ctx) {
	return ctx->screen_dim;
}

void Set_Screen_Dim_KCONTEXT(KCONTEXT *ctx, SCREEN_DIM screen_dim) {
	ctx->screen_dim = screen_dim;
}

void Set_System_Resource_Dir_KCONTEXT(KCONTEXT *ctx, char *str) {
        strcpy(ctx->system_resource_dir, str);
}

void Log_KCONTEXT(KCONTEXT *ctx, char *str) {
	Post_ELASTIC_SEARCH_CLIENT(&ctx->client, str);
}

void Delete_KCONTEXT(KCONTEXT *ctx) {
	Delete_ELASTIC_SEARCH_CLIENT(&ctx->client);
}
