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

//email and uuid are optional for Themis
bool Initialize_Connection_CONTEXT(CONTEXT *ctx, string email_, string uuid_) {
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
			project::CreateInstanceRequest createRequest;
			project::CreateInstanceResponse createResponse;
			project::Email email;
			bool dont_create_rana;

                        ranaID.mutable_uuid()->set_value(ctx->uuid);
                        email.set_value(email_);
			createRequest.mutable_email()->set_value(email_);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetRana(&context, ranaID, &rana);

				//if we couldn't search for rana then create it
				dont_create_rana = status.ok();
                        }

			if (!dont_create_rana) {
				ASSERT_E_R((email_.find(string("@")) != std::string::npos),
					"Email address is invalid.",
					ctx);
				
				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->SearchRana(&context, email, &rana);

					//Okay so what happened? The UUID on
					//file is missing, but the email is
					//legit. Update the UUID to what is
					//passed into this function to check to
					//see if they know their UUID
					if (status.ok()) {
						ranaID.mutable_uuid()->set_value(uuid_);
						goto normal_flow;
					}
				}

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->CreateInstance(&context, createRequest, &createResponse);
					ASSERT_E_R(status.ok(),
					"Could not create instance.",
					ctx);
				}

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->GetRana(&context, createResponse.ranaid(), &rana);
					ASSERT_E_R(status.ok(),
					"Could not get Rana.",
					ctx);
				}
				
				ranaID.mutable_uuid()->set_value(createResponse.ranaid().uuid().value());
			}

			normal_flow:

			//yea this is sorta redundant, but required if the email
			//is legit, but the UUID on file isn't
			{
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetRana(&context, ranaID, &rana);
				ASSERT_E_R(status.ok(),
                                "Could not validate rana UUID.",
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
                } else {
                        project::Rana rana;
                        project::Themis themis;
                        project::RanaUUID ranaID;
                        project::Email email;
			project::CreateInstanceRequest createRequest;
			project::CreateInstanceResponse createResponse;
			bool dont_create_rana;

                        email.set_value(email_);
			createRequest.mutable_email()->set_value(email_);
			ranaID.mutable_uuid()->set_value(uuid_);

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->SearchRana(&context, email, &rana);

				//if we couldn't search for rana then create it
				dont_create_rana = status.ok();
                        }

			if (!dont_create_rana) {
				ASSERT_E_R((email_.find(string("@")) != std::string::npos),
					"Email address is invalid.",
					ctx);

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->CreateInstance(&context, createRequest, &createResponse);
					ASSERT_E_R(status.ok(),
					"Could not create instance.",
					ctx);
				}

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->GetRana(&context, createResponse.ranaid(), &rana);
					ASSERT_E_R(status.ok(),
					"Could not get Rana.",
					ctx);

					ranaID.mutable_uuid()->set_value(rana.ranaid().uuid().value());
				}
			}

			//yea this is sorta redundant, but required if the email
			//is legit, but the UUID on file isn't
			{
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetRana(&context, ranaID, &rana);
				ASSERT_E_R(status.ok(),
                                "Could not validate rana UUID.",
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

bool Check_For_Update_CONTEXT(CONTEXT *ctx, char *verion) {
	std::unique_ptr<project::GAIA::Stub> stub;

        stub = project::GAIA::NewStub(grpc::CreateChannel("api.kible.io:50052",
		grpc::InsecureChannelCredentials()));

	project::GetVersionStoreRequest request;
	project::GetVersionStoreResponse response;

	request.mutable_name()->set_value(string(ctx->core_system));

	{
		grpc::Status status;
		grpc::ClientContext context;
		chrono::system_clock::time_point deadline =
		chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
		context.set_deadline(deadline);

		status = stub->GetVersionStore(&context, request, &response);
		ASSERT_E_R(status.ok(),
		"Could not find version store.",
		ctx);
	}

	return strcmp(response.versionstore().version().value().c_str(), verion) != 0;
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
