#include "pb/THEMIS_SERVER.h"

int main() {
	pb::THEMIS_SERVER server;

	pb::Initialize_THEMIS_SERVER(&server, "http://0.0.0.0:4461", NULL);
	pb::Run_THEMIS_SERVER(&server);

	return 0;
}