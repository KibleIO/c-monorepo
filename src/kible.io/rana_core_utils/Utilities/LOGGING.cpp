#include "LOGGING.h"

bool Initialize_LOGGING(LOGGING *logging, std::string core_system,
	std::string trace_uuid, std::string uuid) {

	logging->core_system = core_system;
	logging->trace_uuid = trace_uuid;
	logging->uuid = uuid;
	
	return Initialize_ELASTIC_SEARCH_CLIENT(&logging->client);
}

void Delete_LOGGING(LOGGING *logging) {
	Delete_ELASTIC_SEARCH_CLIENT(&logging->client);
}

void Log_LOGGING(LOGGING *logging, nlohmann::json obj, char *level, char *file,
	int line, const char *function) {

	char time_str[CURRENT_TIME_LEN];

	get_current_time(time_str);

	obj["@_timestamp"] = std::string(time_str);
	obj["file"] = std::string(file);
	obj["line"] = line;
	obj["function"] = std::string(function);
	obj["system"] = logging->core_system;
	obj["trace_uuid"] = logging->trace_uuid;
	obj["uuid"] = logging->uuid;
	obj["type"] = std::string(level);

	Post_ELASTIC_SEARCH_CLIENT(&logging->client,
		(char*) obj.dump().c_str());
}