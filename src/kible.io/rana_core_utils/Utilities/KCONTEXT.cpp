#include "KCONTEXT.h"

bool Initialize_KCONTEXT(KCONTEXT *ctx, char *core_system, bool insecure) {
	ctx->uuid = "ERROR";
	ctx->insecure_mode = !insecure;

	generate_uuid(ctx->trace_uuid);
	strcpy(ctx->core_system, core_system);
	strcpy(ctx->system_resource_dir, "ERROR");

	ctx->connection_initialized = false;
	ctx->rana_initialized = false;

	if (!Initialize_LOGGING(&ctx->logging, std::string(ctx->core_system),
		std::string(ctx->trace_uuid), ctx->uuid)) {
		
		cout << "failed to initialize logger" << endl;
		return false;
	}

	return true;
}

int Initialize_Connection_KCONTEXT(KCONTEXT *ctx, string uuid_) {
#ifdef EXTERNAL_LOGS_APIS_UTILS
#ifdef __linux__
	INIT_GRPC_STUB_LINUX
#else
	INIT_GRPC_STUB
#endif
#endif
	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOGGER_ERROR(ctx, {
			{"message", "System resource directory has not "
			"been initialized."},
		});
		return INIT_CONN_KCONTEXT_ABORT;
	}

	if (ctx->connection_initialized) {
		LOGGER_WARN(ctx, {
			{"message", "Ports have already been "
						"initialized."},
		});
		return INIT_CONN_KCONTEXT_SUCCESS;
	}

	if (strcmp(ctx->core_system, "RANA") == 0) {
		LOGGER_WARN(ctx, {
			{"message", "Do not attempt to initialize connection in Rana."},
		});

		return false;
	} else if (strcmp(ctx->core_system, "THEMIS") == 0) {
#ifdef EXTERNAL_LOGS_APIS_UTILS
		kible::gaia::RegisterThemisRequest registerRequest;
		kible::gaia::RegisterThemisResponse registerResponse;
		kible::gaia::ContainerID containerID;
#endif

		string containerID_;

		ifstream containerid(CONTAINER_ID_LOC);
		ASSERT_E_R(containerid, "Could not open container id location.", ctx);
		getline(containerid, containerID_);
		containerid.close();

#ifdef EXTERNAL_LOGS_APIS_UTILS
		containerID.mutable_id()->set_value(containerID_);
		registerRequest.mutable_containerid()->CopyFrom(containerID);

		status =
			stub->RegisterThemis(&context, registerRequest, &registerResponse);
		if (!status.ok()) {
			LOGGER_ERROR(&ctx->logging, {
				{"message", "Failed to register themis."},
				{"error", status.error_message()},
			});
			return INIT_CONN_KCONTEXT_ABORT;
		}

		ctx->uuid = registerResponse.ranaid().uuid().value();
		ctx->connection = registerResponse.connection();
		ctx->connection_initialized = true;
#else
	char tmp[UUID_STR_SIZE];
	generate_uuid(tmp);
	ctx->uuid = tmp;
#endif

		return INIT_CONN_KCONTEXT_SUCCESS;
	} else {
		LOGGER_ERROR(ctx, {
			{"message", "Unknown core system"},
			{"sys", ctx->core_system},
		});

		return false;
	}

}

SCREEN_DIM Get_Screen_Dim_KCONTEXT(KCONTEXT *ctx) { return ctx->screen_dim; }

void Set_Screen_Dim_KCONTEXT(KCONTEXT *ctx, SCREEN_DIM screen_dim) {
	ctx->screen_dim = screen_dim;
}

void Set_System_Resource_Dir_KCONTEXT(KCONTEXT *ctx, char *str) {
	strcpy(ctx->system_resource_dir, str);
}

void Delete_KCONTEXT(KCONTEXT *ctx) {
	Delete_LOGGING(&ctx->logging);
}
