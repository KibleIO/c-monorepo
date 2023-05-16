#include "EDGE_CLIENT.h"

bool Path_EDGE_CLIENT(std::string container_id, std::string* path) {
	pb::EDGE_CLIENT client;
	bool return_value;

	pb::Initialize_EDGE_CLIENT(&client, EDGE_SERVER_ADDRESS);

	kible::edge::PathRequest request;
	kible::edge::PathResponse response;

	request.set_containerid(container_id);

	return_value = pb::Path_EDGE_CLIENT(&client, &request,
		&response);

	if (return_value) {
		*path = response.path();
	}
	
	pb::Delete_EDGE_CLIENT(&client);

	return return_value;
}