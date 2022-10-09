#include <Utilities/SODIUM_WRAPPER.h>
#include <Hermes/SERVER.h>
#include <Hermes/CLIENT.h>
#include <Utilities/CONTEXT.h>

//little program to generate some keys

//idk why 3337...
#define PORT 3337

void usage() {
	cout << "USAGE:" << endl;
	cout << "SERVER: ./keygen s" << endl;
	cout << "CLIENT: ./keygen c <insert server IP>" << endl;
	exit(0);
}

int main(int argc, char** argv) {
	SODIUM_WRAPPER sodium;
	CONTEXT ctx;
	bool is_server = false;

	switch (argc) {
		case 2:
			if (strcmp(argv[1], "s") != 0) {
				usage();
			}
			is_server = true;
			break;
		case 3:
			if (strcmp(argv[1], "c") != 0) {
				usage();
			}
			break;
		default:
			usage();
	}

	if (!Initialize_CONTEXT(&ctx, "INTERACTIVE_CREATE_KEY")) {
		cout << "Couldn't initialize context" << endl;
		return false;
	}

	if (Initialize_SODIUM_WRAPPER(&sodium, &ctx,
		SODIUM_WRAPPER_INTERACTIVE_CREATE_KEY_TYPE)) {

		//in a very rare case this could signify a much deeper issue
		//with libsodium. unlikely though
		cout << "Looks like you are all set." << endl;
		return false;
	}

	if (!Interactive_Create_Key_Step_1_SODIUM_WRAPPER(&sodium)) {
		return false;
	}

	if (is_server) {
		SERVER server;

		if (!Initialize_SERVER(&server, &ctx, NETWORK_TYPE_TCP)) {
			cout << "Couldn't initialize server." << endl;
			return false;
		}

		if (!Accept_SERVER(&server, PORT)) {
			cout << "Couldn't find a connection on port " <<
				PORT << endl;
			return false;
		}

		if (!Receive_SERVER(&server, (char*)sodium.partner_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't receive partner public key." << endl;
			return false;
		}

		if (!Send_SERVER(&server, (char*)sodium.local_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't send local public key." << endl;
			return false;
		}

		Delete_SERVER(&server);
	} else {
		CLIENT client;

		if (!Initialize_CLIENT(&client, &ctx, NETWORK_TYPE_TCP)) {
			cout << "Couldn't initialize server." << endl;
			return false;
		}

		if (!Connect_CLIENT(&client, PORT, argv[2])) {
			cout << "Couldn't open connection to " << argv[2] <<
				" on port " << PORT << endl;
			return false;
		}

		if (!Send_CLIENT(&client, (char*)sodium.local_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't send local public key." << endl;
			return false;
		}

		if (!Receive_CLIENT(&client, (char*)sodium.partner_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't receive partner public key." << endl;
			return false;
		}

		Delete_CLIENT(&client);
	}

	if (!Interactive_Create_Key_Step_2_SODIUM_WRAPPER(&sodium)) {
		return false;
	}

	cout << "Success!" << endl;
}
