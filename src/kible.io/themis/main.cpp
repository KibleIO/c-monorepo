#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/UTILS.h>
#include <Utilities/TIMER.h>
#include "LIMITS.h"
#include "THEMIS_EXT.h"
#include "THEMIS_GRPC_SERVER.h"
#include "controller/THEMIS_HTTP_SERVER.h"

#define CTOA(character) int(character - '0')
void start_filebrowser(volatile bool *running, KCONTEXT *ctx) {
	while (*running) {
		if (CTOA(system_output(
				"ps aux | grep filebrowser | grep -v grep | wc -l")[0]) < 1) {
			system(
				string(
					string("filebrowser -r /home/user -a 0.0.0.0 -p ") +
#ifdef EXTERNAL_LOGS_APIS
					to_string(ctx->connection.filesport()) +
					" --username \"admin\" --password $(filebrowser hash \"" +
					ctx->uuid + "\") &")
#else
					to_string(FILEBROWSER_PORT) +
					" --username \"admin\" --password $(filebrowser hash \"" +
					ctx->uuid + "\") &")
#endif
					.c_str());
		}
		Sleep_Milli(1000);
	}
}

int main() {
	KCONTEXT ctx;
	THEMIS_EXT themis_ext;
#ifdef EXTERNAL_LOGS_APIS
	THEMIS_GRPC_SERVER grpc(&themis_ext);
#else
	THEMIS_HTTP_SERVER http;
	if (!Initialize_THEMIS_HTTP_SERVER(&http, &themis_ext,
		THEMIS_HTTP_PORT)) {
		
		LOG_ERROR_CTX(&ctx) {
			ADD_STR_LOG("message",
				"failed to initialize themis http");
		}
		return 0;
	}
#endif
	volatile bool running = true;

	ASSERT_P_R(Initialize_KCONTEXT(&ctx, __CORE_SYSTEM__, true),
			   "failed to initialize context");

	LOG_INFO_CTX(&ctx) {
		ADD_STR_LOG("message", "main begun");
		ADD_STR_LOG("version", SOFTWARE_VERSION);
	}

	Set_System_Resource_Dir_KCONTEXT(&ctx, (char *)ROOT_DIR.c_str());

	if (Initialize_Connection_KCONTEXT(&ctx, "") !=
		INIT_CONN_KCONTEXT_SUCCESS) {
		LOG_ERROR_CTX(&ctx) {
			ADD_STR_LOG("message", "failed to initialize connection context");
		}
		return 0;
	}

	//thread filebrowser_thread(start_filebrowser, &running, &ctx);

#ifdef EXTERNAL_LOGS_APIS
	ASSERT_E_R(
		Initialize_THEMIS_EXT(&themis_ext, &ctx, ctx.connection.themisport()),
		"failed to initialize themis_ext", &ctx);

	grpc.Start(ctx.connection.grpcport());
#else
	ASSERT_E_R(Initialize_THEMIS_EXT(&themis_ext, &ctx, THEMIS_PORT),
			   "failed to initialize themis_ext", &ctx);
	
	Run_THEMIS_HTTP_SERVER(&http);
#endif
	running = false;
	//filebrowser_thread.join();

	Delete_THEMIS_EXT(&themis_ext);
	Delete_KCONTEXT(&ctx);

	LOG_INFO_CTX(&ctx) { ADD_STR_LOG("message", "clean exit"); }
	return 0;
}