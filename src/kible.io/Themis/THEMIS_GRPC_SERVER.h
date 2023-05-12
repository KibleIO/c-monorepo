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
#include "idl/cpp/gen/themis.grpc.pb.h"

using namespace std;

class THEMIS_GRPC_SERVER final : public gaia::THEMIS::Service {
public:
	explicit THEMIS_GRPC_SERVER(THEMIS_EXT *t);

	grpc::Status Resize(grpc::ServerContext* context,
		const gaia::ResizeThemisRequest* request, gaia::Empty* response);

	grpc::Status Update(grpc::ServerContext* context,
		const gaia::Empty* request, gaia::Empty* response);

        grpc::Status Launch(grpc::ServerContext* context,
		const gaia::Empty* request, gaia::Empty* response);
	
	grpc::Status Check(grpc::ServerContext* context,
		const gaia::Empty* request, gaia::CheckResponse* response);

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
