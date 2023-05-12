#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/UTILS.h>
#include <Utilities/TIMER.h>
#include "LIMITS.h"
#include "THEMIS_EXT.h"
#include "THEMIS_GRPC_SERVER.h"

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
	
	while (true) {
		if (Connect_THEMIS_EXT(&themis_ext)) {
			TIMER t;
			Start_TIMER(&t);
			LOG_INFO_CTX(&ctx) {
				ADD_STR_LOG("message", "Started Themis Session.");
			}
			while (Running_THEMIS_EXT(&themis_ext)) {
				Sleep_Milli(1000); //busy wait
			}
			LOG_INFO_CTX(&ctx) {
				ADD_STR_LOG("message", "Ended Themis Session.");
				ADD_INT_LOG("time", Stop_TIMER(&t));
			}
			break;
		}
		break;
	}
	Disconnect_THEMIS_EXT(&themis_ext);

#endif
	running = false;
	//filebrowser_thread.join();

	Delete_THEMIS_EXT(&themis_ext);
	Delete_KCONTEXT(&ctx);

	LOG_INFO_CTX(&ctx) { ADD_STR_LOG("message", "clean exit"); }
	return 0;
}