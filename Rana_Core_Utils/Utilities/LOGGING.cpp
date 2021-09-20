//PLATFORMS: Linux, Windows, OSX (TODO)

#include "LOGGING.h"

uint8_t INITIALIZED_ELASTIC_SEARCH_CLIENT = false;
ELASTIC_SEARCH_CLIENT ELASTIC_CLIENT;

void initialize_elastic_search_client() {
	if (INITIALIZED_ELASTIC_SEARCH_CLIENT) {
		return;
	}
	INITIALIZED_ELASTIC_SEARCH_CLIENT = true;

	Initialize_ELASTIC_SEARCH_CLIENT(&ELASTIC_CLIENT);
}
void Write_Message(const JSON_TYPE payload, char *file, uint32_t line,
	char *function, char *type) {

	char buffer [TEMP_BUFFER_SIZE];
	int ret;

	initialize_elastic_search_client();

	ret = snprintf(buffer, TEMP_BUFFER_SIZE, "%d", line);
	if (ret > 0) {
		Post_ELASTIC_SEARCH_CLIENT(&ELASTIC_CLIENT, file, buffer,
			function, type);
	}
}
