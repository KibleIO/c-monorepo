#include "CONTEXT.h"

bool Initialize_CONTEXT(CONTEXT *ctx, char *core_system) {
        ctx->uuid = "ERROR";
	generate_uuid(ctx->trace_uuid);
	strcpy(ctx->core_system, core_system);
        strcpy(ctx->system_resource_dir, "ERROR");

        ctx->connection_initialized = false;

	if (!Initialize_ELASTIC_SEARCH_CLIENT(&ctx->client)) {
		cout << "failed to initialize elastic search client" << endl;
		return false;
	}

	return true;
}

//email is optional for Themis
bool Initialize_Connection_CONTEXT(CONTEXT *ctx, string email_) {
        std::unique_ptr<project::GAIA::Stub> stub;

        stub = project::GAIA::NewStub(grpc::CreateChannel("api.kible.io:50052",
		grpc::InsecureChannelCredentials()));

        ASSERT_E_R(strcmp(ctx->system_resource_dir, "ERROR") != 0,
		"System resource directory has not been initialized.",
		ctx);
        
         ASSERT_E_R(!ctx->connection_initialized,
		"Ports have already been initialized.",
		ctx);

        if (strcmp(ctx->core_system, "RANA") == 0) {
                ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, ctx->uuid);
                        file.close();
                        
                        project::Rana rana;
                        project::Themis themis;
                        project::RanaUUID ranaID;

                        ranaID.mutable_uuid()->set_value(ctx->uuid);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetRana(&context, ranaID, &rana);
                                ASSERT_E_R(status.ok(),
                                "Could not look up rana UUID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetThemis(&context, rana.themisid(), &themis);
                                ASSERT_E_R(status.ok(),
                                "Could not look up themis UUID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetConnection(&context, themis.connectionid(), &ctx->connection);
                                ASSERT_E_R(status.ok(),
                                "Could not look up connection UUID.",
                                ctx);
                        }

                        ctx->connection_initialized = true;

                        return true;
                } else {
                        project::Rana rana;
                        project::Themis themis;
                        project::RanaUUID ranaID;
                        project::Email email;

                        email.set_value(email_);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->SearchRana(&context, email, &rana);
                                ASSERT_E_R(status.ok(),
                                "Could not look up rana UUID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetThemis(&context, rana.themisid(), &themis);
                                ASSERT_E_R(status.ok(),
                                "Could not look up themis UUID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetConnection(&context, themis.connectionid(), &ctx->connection);
                                ASSERT_E_R(status.ok(),
                                "Could not look up connection UUID.",
                                ctx);
                        }

                        ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                        ASSERT_E_R(output,
                                "Could not open info location.",
                                ctx);
                        output << rana.ranaid().uuid().value() << endl;
                        output.close();

                        ctx->connection_initialized = true;

                        ctx->uuid = rana.ranaid().uuid().value();

                        return true;
                }
        } else if (strcmp(ctx->core_system, "THEMIS") == 0) {
                ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, ctx->uuid);
                        file.close();

                        project::Themis themis;
                        project::ThemisUUID themisID;

                        themisID.mutable_uuid()->set_value(ctx->uuid);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetThemis(&context, themisID, &themis);
                                ASSERT_E_R(status.ok(),
                                "Could not look up themis UUID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetConnection(&context, themis.connectionid(), &ctx->connection);
                                ASSERT_E_R(status.ok(),
                                "Could not look up connection UUID.",
                                ctx);
                        }

                        ctx->connection_initialized = true;

                        return true;
                } else {
                        string containerID_;

                        ifstream containerid(CONTAINER_ID_LOC);
                        ASSERT_E_R(containerid,
                                "Could not open container id location.",
                                ctx);
                        
                        getline(containerid, containerID_);
                        containerid.close();

                        project::Themis themis;
                        project::ContainerID containerID;

                        containerID.mutable_id()->set_value(containerID_);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->SearchThemis(&context, containerID, &themis);
                                ASSERT_E_R(status.ok(),
                                "Could not look up container ID.",
                                ctx);
                        }

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);
                                
                                status = stub->GetConnection(&context, themis.connectionid(), &ctx->connection);
                                ASSERT_E_R(status.ok(),
                                "Could not look up connection UUID.",
                                ctx);
                        }

                        ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                        ASSERT_E_R(output,
                                "Could not open info location.",
                                ctx);
                        output << themis.themisid().uuid().value() << endl;
                        output.close();

                        ctx->uuid = themis.themisid().uuid().value();

                        ctx->connection_initialized = true;

                        return true;
                }
        } else {
                LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Unknown core system");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        }
}

SCREEN_DIM Get_Screen_Dim_CONTEXT(CONTEXT *ctx) {
	return ctx->screen_dim;
}

void Set_Screen_Dim_CONTEXT(CONTEXT *ctx, SCREEN_DIM screen_dim) {
	ctx->screen_dim = screen_dim;
}

void Set_System_Resource_Dir_CONTEXT(CONTEXT *ctx, char *str) {
        strcpy(ctx->system_resource_dir, str);
}

void Log_CONTEXT(CONTEXT *ctx, char *str) {
	Post_ELASTIC_SEARCH_CLIENT(&ctx->client, str);
}

void Delete_CONTEXT(CONTEXT *ctx) {
	Delete_ELASTIC_SEARCH_CLIENT(&ctx->client);
}
