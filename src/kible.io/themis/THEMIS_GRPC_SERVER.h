#ifndef _THEMIS_GRPC_SERVER_H_
#define _THEMIS_GRPC_SERVER_H_

#include <Utilities/Update_Utility.h>
#include "THEMIS_EXT.h"
#include "LIMITS.h"

#if defined (Status)
# undef Status
typedef int Status;
#endif

#include <iostream>
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "../gen/themis/themis.grpc.pb.h"

using namespace std;

class THEMIS_GRPC_SERVER final : public kible::themis::THEMIS::Service {
public:
	explicit THEMIS_GRPC_SERVER(THEMIS_EXT *t);

	grpc::Status Dimensions(grpc::ServerContext* context,
		const kible::themis::DimensionsRequest* request, kible::themis::DimensionsResponse* response);

	grpc::Status Update(grpc::ServerContext* context,
		const kible::themis::UpdateRequest* request, kible::themis::UpdateResponse* response);

        grpc::Status Launch(grpc::ServerContext* context,
		const kible::themis::LaunchRequest* request, kible::themis::LaunchResponse* response);
	
	grpc::Status Check(grpc::ServerContext* context,
		const kible::themis::CheckRequest* request, kible::themis::CheckResponse* response);

	void Start(int port);
	void Stop();
private:
	thread *update_thread;
	thread *launch_thread;
	THEMIS_EXT *themis_ext;
	Update_UTILITY updateInfo;
	volatile bool update_running;
	unique_ptr<grpc::Server> server;
};

#endif
