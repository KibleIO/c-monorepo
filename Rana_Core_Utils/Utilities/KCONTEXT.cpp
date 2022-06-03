#include "KCONTEXT.h"

bool Initialize_KCONTEXT(KCONTEXT *ctx, char *core_system) {
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
bool Initialize_Connection_KCONTEXT(KCONTEXT *ctx, string email_, string uuid_) {
        std::unique_ptr<project::GAIA::Stub> stub;

        stub = project::GAIA::NewStub(grpc::CreateChannel("45.57.227.210:50052",
		grpc::InsecureChannelCredentials()));

        ASSERT_E_R(strcmp(ctx->system_resource_dir, "ERROR") != 0,
		"System resource directory has not been initialized.",
		ctx);
	
	if (ctx->connection_initialized) {
		LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Ports have already been initialized.");
		}
		return true;
	}

        if (strcmp(ctx->core_system, "RANA") == 0) {
                ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, ctx->uuid);
                        file.close();
                        
                        project::Rana rana;
                        project::RanaUUID ranaID;
			project::CreateInstanceRequest createRequest;
			project::CreateInstanceResponse createResponse;
			project::RecreateInstanceRequest recreateRequest;
			project::RecreateInstanceResponse recreateResponse;
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

			{
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->LoginRana(&context, ranaID, &rana);
				ASSERT_E_R(status.ok(),
                                "Could not validate rana UUID.",
                                ctx);
                        }

			if (!rana.has_connectionid()) {
				LOG_WARN_CTX(ctx) {
					ADD_STR_LOG("message", "Recreating the account");
				}

				recreateRequest.mutable_ranaid()->mutable_uuid()->set_value(ranaID.uuid().value());

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->RecreateInstance(&context, recreateRequest, &recreateResponse);
					ASSERT_E_R(status.ok(),
					"Could not recreate instance.",
					ctx);
				}

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->LoginRana(&context, ranaID, &rana);
					ASSERT_E_R(status.ok(),
					"Could not validate rana UUID.",
					ctx);

					ASSERT_E_R(rana.has_connectionid(),
					"Connection ID is still null.",
					ctx);
				}
			}

                        {
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->GetConnection(&context, rana.connectionid(), &ctx->connection);
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
                        project::RanaUUID ranaID;
                        project::Email email;
			project::CreateInstanceRequest createRequest;
			project::CreateInstanceResponse createResponse;
			project::RecreateInstanceRequest recreateRequest;
			project::RecreateInstanceResponse recreateResponse;
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

			{
                                grpc::Status status;
                                grpc::ClientContext context;
                                chrono::system_clock::time_point deadline =
                                chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
                                context.set_deadline(deadline);

                                status = stub->LoginRana(&context, ranaID, &rana);
				ASSERT_E_R(status.ok(),
                                "Could not validate rana UUID.",
                                ctx);
                        }

			if (!rana.has_connectionid()) {
				LOG_WARN_CTX(ctx) {
					ADD_STR_LOG("message", "Recreating the account");
				}


				recreateRequest.mutable_ranaid()->mutable_uuid()->set_value(ranaID.uuid().value());

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->RecreateInstance(&context, recreateRequest, &recreateResponse);
					ASSERT_E_R(status.ok(),
					"Could not recreate instance.",
					ctx);
				}

				{
					grpc::Status status;
					grpc::ClientContext context;
					chrono::system_clock::time_point deadline =
					chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
					context.set_deadline(deadline);

					status = stub->LoginRana(&context, ranaID, &rana);
					ASSERT_E_R(status.ok(),
					"Could not validate rana UUID.",
					ctx);

					ASSERT_E_R(rana.has_connectionid(),
					"Connection ID is still null.",
					ctx);
				}
			}

			{
				grpc::Status status;
				grpc::ClientContext context;
				chrono::system_clock::time_point deadline =
				chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
				context.set_deadline(deadline);

				status = stub->GetConnection(&context, rana.connectionid(), &ctx->connection);
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
		string containerID_;

		ifstream containerid(CONTAINER_ID_LOC);
		ASSERT_E_R(containerid,
			"Could not open container id location.",
			ctx);
		
		getline(containerid, containerID_);
		containerid.close();

		project::ContainerID containerID;
		project::GetRanaFromConnectionRequest getRana;
		project::GetRanaFromConnectionResponse gotRana;
		project::Rana rana;

		containerID.mutable_id()->set_value(containerID_);

		{
			grpc::Status status;
			grpc::ClientContext context;
			chrono::system_clock::time_point deadline =
			chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
			context.set_deadline(deadline);

			status = stub->SearchConnection(&context, containerID, &ctx->connection);
			ASSERT_E_R(status.ok(),
			"Could not look up container ID.",
			ctx);
		}

		getRana.mutable_connectionid()->mutable_uuid()->set_value(ctx->connection.connectionid().uuid().value());

		{
			grpc::Status status;
			grpc::ClientContext context;
			chrono::system_clock::time_point deadline =
			chrono::system_clock::now() + chrono::seconds(DEFAULT_GRPC_TIMEOUT);
			context.set_deadline(deadline);
			
			status = stub->GetRanaFromConnection(&context, getRana, &gotRana);
			ASSERT_E_R(status.ok(),
			"Could not look up connection UUID.",
			ctx);
		}

		ctx->uuid = gotRana.ranaid().uuid().value();

		ctx->connection_initialized = true;

		return true;
        } else {
                LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Unknown core system");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        }
}

bool Check_For_Update_KCONTEXT(KCONTEXT *ctx, char *verion) {
	std::unique_ptr<project::GAIA::Stub> stub;

        stub = project::GAIA::NewStub(grpc::CreateChannel("45.57.227.210:50052",
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

	strcpy(verion, response.versionstore().version().value().c_str());

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
