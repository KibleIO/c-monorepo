#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/UTILS.h>
#include "controller/THEMIS_HTTP_SERVER.h"
#include "client/EDGE_CLIENT.h"

int main() {
	KCONTEXT ctx;

	#ifdef TESTING_BUILD
	THEMIS_HTTP_SERVER http;
	if (!Initialize_THEMIS_HTTP_SERVER(&http, &ctx, THEMIS_HTTP_PORT)) {
		return 0;
	}
	#else
	std::string container_id;
	std::string path;

	ifstream containerid(CONTAINER_ID_LOC);
	if (!containerid) {
		return 0;
	}
	getline(containerid, container_id);
	containerid.close();

	if (!Path_EDGE_CLIENT(container_id, &path)) {
		return 0;
	}

	THEMIS_HTTP_SERVER http;
	if (!Initialize_THEMIS_HTTP_SERVER(&http, &ctx, THEMIS_HTTP_PORT, path)) {
		return 0;
	}
	#endif

	ASSERT_P_R(Initialize_KCONTEXT(&ctx, __CORE_SYSTEM__, true),
			   "failed to initialize context");

	Set_System_Resource_Dir_KCONTEXT(&ctx, (char *)ROOT_DIR.c_str());

	if (Initialize_Connection_KCONTEXT(&ctx, "") !=
		INIT_CONN_KCONTEXT_SUCCESS) {
		return 0;
	}

	LOGGER_INFO((&ctx), {
		{"message", "main begun"},
		{"version", SOFTWARE_VERSION},
	});
	
	Run_THEMIS_HTTP_SERVER(&http);

	Delete_KCONTEXT(&ctx);

	return 0;
}