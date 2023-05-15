#include <iostream>
#include "pb/EDGE_CLIENT.h"

int main() {
	pb::EDGE_CLIENT client;
	bool return_value;

	pb::Initialize_EDGE_CLIENT(&client, "https://hub.alienhub.xyz");

	kible::edge::ThemisRequest request;
	kible::edge::ThemisResponse response;

	request.mutable_sessionid()->mutable_uuid()->set_value("3ed9d88c-61b7-41c3-ac11-35ca23f0029f");

	return_value = pb::Themis_EDGE_CLIENT(&client, &request,
		&response);

	if (return_value) {
		std::cout << "got this " << response.address() << std::endl;
	}
	
	pb::Delete_EDGE_CLIENT(&client);

	return 0;
}