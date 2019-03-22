#ifndef CLIENT_CPP_
#define CLIENT_CPP_

#include "Client.h"

void Client::Set_Encryption_Profile(ENCRYPTION_PROFILE* _enc) {
	if (_enc) {
		enc = _enc;
		if (enc_buf_auth) {
			delete enc_buf_auth;
		}
		enc_buf_auth = new char[3000];
		enc_buf_data = enc_buf_auth + crypto_onetimeauth_BYTES;
	}
}

Client::Client() {
	Init();
}

bool Client::Init() {
	StartService();
	enc = NULL;
	enc_buf_auth = NULL;
	enc_buf_data = NULL;
	int iSetOption = 1;
	destination.sin_family = AF_INET; //ipv4
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption)); //potentially dangerous

	startedUp = (!(mainSocket < 0));
	if (startedUp) {
		log_dbg("Client initialized successfully");
	} else {
	 	log_err("Client socket failed to open");
	}

	return (startedUp);
}

bool Client::OpenConnection(int port, string ip) {
	if (!startedUp)
	{
		log_err("Cannot open connection because client is not started up");
		return false;
	}
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = inet_addr(ip.c_str());
	bool r = connect(mainSocket, (sockaddr *) &destination, sizeof destination) == 0;
	int buff_size = 700000;
	setsockopt(mainSocket, SOL_SOCKET, SO_RCVBUF, &buff_size, (int) sizeof(buff_size));
	if (r) {
		log_dbg("Connection successful " + to_string(port) + " : " + ip);
	} 
	return r;
}

void Client::CloseConnection() {
	EndService();
	log_dbg("Client connection closed");
	startedUp = false;
}

void print_bytes2(uint8_t* bytes, int n) {
	string str;
	for (int i = 0; i < n; i++) {
		str += to_string(bytes[i]) + " ";
	}
	log_dbg(str);
}

bool Client::Send(char *data, int size) {
	if (!startedUp) {
		return false;
	}

	if (enc) {
		//log_dbg(enc->identifier);
		//print_bytes2(enc->poly1305_key, crypto_onetimeauth_KEYBYTES);
		//print_bytes2(enc->poly1305_nonce, crypto_onetimeauth_KEYBYTES);
		if (!Encrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)data, size, (uint8_t*)enc_buf_data)) {
			log_err("unable to encrypt data");
			return false;
		}
		if (!Generate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err("unable to generate auth code");
			return false;
		}
		size += crypto_onetimeauth_BYTES;
		data = enc_buf_auth;
	}

	return send(mainSocket, data, size, MSG_WAITALL) == size;
}

bool Client::Receive(char *data, int size) {
	if (!startedUp) {
		return false;
	}

	if (enc) {
		//log_dbg(enc->identifier);
		//print_bytes2(enc->poly1305_key, crypto_onetimeauth_KEYBYTES);
		//print_bytes2(enc->poly1305_nonce, crypto_onetimeauth_KEYBYTES);
		size += crypto_onetimeauth_BYTES;
		if (recv(mainSocket, enc_buf_auth, size, MSG_WAITALL) != size) {
			log_err("unable to receive");
			return false;
		}
		size -= crypto_onetimeauth_BYTES;
		if (!Authenticate_Auth_Code_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)enc_buf_auth)) {
			log_err("unable to authenticate data");
			return false;
		}
		if (!Decrypt_Data_ENCRYPTION_PROFILE(
		enc, (uint8_t*)enc_buf_data, size, (uint8_t*)data)) {
			log_err("unable to decrypt data");
			return false;
		}
	} else {
		if (recv(mainSocket, data, size, MSG_WAITALL) != size) {
			log_err("unable to receive");
			return false;
		}
	}

	return true;
}

//bool Client::Receive_b(char *data, int size) {
//	bool ret = (startedUp && recv(mainSocket, data, size, MSG_DONTWAIT) == size);
//
//	if (ret) { 
//		//TODO decrypt 
//	}
//	
//	return ret;
//}

#endif /* CLIENT_H_ */
