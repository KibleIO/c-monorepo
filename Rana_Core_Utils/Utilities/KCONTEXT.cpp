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
        #ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

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
		LOG_WARN_CTX(ctx) {
			ADD_STR_LOG("message", "Do not attempt to initialize connection in Rana.");
			ADD_STR_LOG("sys", ctx->core_system);
		}

                return false;
        } else if (strcmp(ctx->core_system, "THEMIS") == 0) {
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
int Create_Rana_KCONTEXT(KCONTEXT *ctx, string email_, string password) {
        #ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

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
		gaia::CreateRanaAccountRequest registerRequest;
		gaia::CreateRanaAccountResponse registerResponse;
		gaia::LocationUUID locationID;

		registerRequest.mutable_email()->set_value(email_);
		registerRequest.set_password(password);

		if (ctx->locationID.has_uuid()) {
			locationID.mutable_uuid()->set_value(ctx->locationID.uuid().value());
			registerRequest.mutable_locationid()->CopyFrom(locationID);
		}

		status = stub->CreateRanaAccount(&context, registerRequest, &registerResponse);

		if (status.ok()) {
			ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                        ASSERT_E_R(output,
                                "Could not open info location.",
                                ctx);
                        output << registerResponse.sessiontoken() << endl;
                        output.close();

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
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetVersionStoreRequest request;
	gaia::GetVersionStoreResponse response;

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
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetLocationsRequest request;

	status = stub->GetLocations(&context, request, &ctx->locations);
	ASSERT_E_R(status.ok(),
	"Could not get locations.",
	ctx);

	return true;
}

bool Get_Apps_KCONTEXT(KCONTEXT *ctx) {
	if (!ctx->rana_initialized) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Rana has not already been "
				"initialized.");
		}
		return false;
	}

	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetAppsRequest request;
	string token;

	ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	if (file) {
		getline(file, token);
		file.close();

		request.set_sessiontoken(token);
	}

	status = stub->GetApps(&context, request, &ctx->apps);

	if (!status.ok()) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not get apps.");
		}

		ctx->recent_error = status.error_message();

		return false;
	}

	return true;
}

bool Get_Ads_KCONTEXT(KCONTEXT *ctx, gaia::AdType ad_type) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::ListAdsRequest request;

	request.set_adtype(ad_type);

	status = stub->ListAds(&context, request, &ctx->ads);

	if (!status.ok()) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not get ads.");
		}

		ctx->recent_error = status.error_message();

		return false;
	}

	return true;
}

bool GetCheckoutUrl(KCONTEXT *ctx, gaia::ProductUUID productID, char *str) {
	if (!ctx->rana_initialized) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Rana has not already been "
				"initialized.");
		}
		return false;
	}

	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::CreatePaymentUrlAuthRequest request;
	gaia::CreatePaymentUrlAuthResponse response;
	string token;

	ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	if (file) {
		getline(file, token);
		file.close();

		request.set_sessiontoken(token);
	}
	request.mutable_productid()->CopyFrom(productID);


	status = stub->CreatePaymentUrlAuth(&context, request, &response);

	if (!status.ok()) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not get GetCheckoutUrl.");
		}

		ctx->recent_error = status.error_message();

		return false;
	}

	strcpy(str, response.checkoutsession().c_str());

	return true;
}

bool GetCheckPayment(KCONTEXT *ctx, gaia::ProductUUID productID) {
        if (!ctx->rana_initialized) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Rana has not already been "
				"initialized.");
		}
		return false;
	}

	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::CheckPaymentAuthRequest request;
	gaia::CheckPaymentAuthResponse response;
	string token;

	ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	if (file) {
		getline(file, token);
		file.close();

		request.set_sessiontoken(token);
	}

	request.mutable_productid()->CopyFrom(productID);

	status = stub->CheckPaymentAuth(&context, request, &response);

	if (!status.ok()) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "Could not get GetCheckPayment.");
		}

		ctx->recent_error = status.error_message();

		return false;
	}

	return response.paid();
}

int Check_Existing_Token_KCONTEXT(KCONTEXT *ctx) {
        #ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

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
		gaia::CheckSessionTokenRequest request;
		gaia::CheckSessionTokenResponse response;
		string token;

		ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
                if (file) {
                        getline(file, token);
                        file.close();

			request.set_sessiontoken(token);
		}

		status = stub->CheckSessionToken(&context, request, &response);

		if (status.ok()) {
			ctx->uuid = response.ranaid().uuid().value();

			ctx->rana_initialized = true;

			return INIT_CONN_KCONTEXT_SUCCESS;
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

bool Check_Password_Strength_KCONTEXT(KCONTEXT *ctx, string password) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::CheckPasswordStrengthRequest request;
	gaia::CheckPasswordStrengthResponse response;

	request.set_password(password);

	status = stub->CheckPasswordStrength(&context, request, &response);

	if (status.ok()) {
		ctx->recent_error = response.reason();
		return response.passed();
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Reset_Password_KCONTEXT(KCONTEXT *ctx, string email) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::ResetPasswordRequest request;
	gaia::ResetPasswordResponse response;

	request.set_email(email);

	status = stub->ResetPassword(&context, request, &response);

	if (status.ok()) {
		ctx->recent_error = "Successfully Sent password reset email.";
		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Login_Rana_KCONTEXT(KCONTEXT *ctx, string email, string password) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "System resource directory has "
				"not been initialized.");
		}
		return false;
	}

	gaia::LoginRanaRequest request;
	gaia::LoginRanaResponse response;

	request.mutable_email()->set_value(email);
	request.set_password(password);

	status = stub->LoginRana(&context, request, &response);

	if (status.ok()) {
		ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
		ASSERT_E_R(output,
			"Could not open info location.",
			ctx);
		output << response.sessiontoken() << endl;
		output.close();

		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

void Sign_Out_Of_Session_KCONTEXT(KCONTEXT *ctx) {
	ctx->connection_initialized = false;
	ctx->rana_initialized = false;
	
	ofstream output(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	output << "nil" << endl;
	output.close();
}

bool Wake_Up_App_KCONTEXT(KCONTEXT *ctx, gaia::AppUUID appID) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "System resource directory has "
				"not been initialized.");
		}
		return false;
	}

	gaia::WakeUpAppRequest request;
	gaia::WakeUpAppResponse response;
	string token;

	ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	if (file) {
		getline(file, token);
		file.close();

		request.set_sessiontoken(token);
	}
	
	request.mutable_appid()->CopyFrom(appID);

	status = stub->WakeUpApp(&context, request, &response);

	if (status.ok()) {
		ctx->connection = response.connection();
		ctx->connection_initialized = true;

		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Get_App_KCONTEXT(KCONTEXT *ctx, gaia::AppUUID appID) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	if (strcmp(ctx->system_resource_dir, "ERROR") == 0) {
		LOG_ERROR_CTX(ctx) {
			ADD_STR_LOG("message", "System resource directory has "
				"not been initialized.");
		}
		return false;
	}

	gaia::GetAppRequest request;
	string token;

	ifstream file(string(ctx->system_resource_dir) + INFO_FILE_NAME);
	if (file) {
		getline(file, token);
		file.close();

		request.set_sessiontoken(token);
	}
	
	request.mutable_appid()->CopyFrom(appID);

	status = stub->GetApp(&context, request, &ctx->app);

	if (status.ok()) {
		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Get_Product_KCONTEXT(KCONTEXT *ctx, gaia::ProductUUID productID) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetProductRequest request;
	request.mutable_productid()->CopyFrom(productID);

	status = stub->GetProduct(&context, request, &ctx->product);

	if (status.ok()) {
		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Get_Available_Products_KCONTEXT(KCONTEXT *ctx) {
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetAvailableProductsRequest request;

	status = stub->GetAvailableProducts(&context, request, &ctx->products);

	if (status.ok()) {
		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
}

bool Get_Product_From_Connection_KCONTEXT(KCONTEXT *ctx,
	gaia::ConnectionUUID connectionID) {
	
	#ifdef __linux__
	INIT_GRPC_STUB_LINUX
	#else
	INIT_GRPC_STUB
	#endif

	gaia::GetProductFromConnectionRequest request;
	request.mutable_connectionid()->CopyFrom(connectionID);

	status = stub->GetProductFromConnection(&context, request,
		&ctx->productFromConnection);

	if (status.ok()) {
		return true;
	}

	ctx->recent_error = status.error_message();
	return false;
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
