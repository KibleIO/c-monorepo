#include <Utilities/SODIUM_WRAPPER.h>
#include <Networking/Server.h>
#include <Networking/Client.h>

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
		Server server;

		if (!server.Listen(PORT)) {
			cout << "Couldn't find a connection on port " <<
				PORT << endl;
			return false;
		}

		if (!server.Receive((char*)sodium.partner_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't receive partner public key." << endl;
			return false;
		}

		if (!server.Send((char*)sodium.local_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't send local public key." << endl;
			return false;
		}

		server.CloseConnection();
	} else {
		Client client;

		if (!client.OpenConnection(PORT, argv[2])) {
			cout << "Couldn't open connection to " << argv[2] <<
				" on port " << PORT << endl;
			return false;
		}

		if (!client.Send((char*)sodium.local_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't send local public key." << endl;
			return false;
		}

		if (!client.Receive((char*)sodium.partner_pub_key,
			crypto_sign_PUBLICKEYBYTES)) {

			cout << "Couldn't receive partner public key." << endl;
			return false;
		}

		client.CloseConnection();
	}

	if (!Interactive_Create_Key_Step_2_SODIUM_WRAPPER(&sodium)) {
		return false;
	}

	cout << "Success!" << endl;
}
