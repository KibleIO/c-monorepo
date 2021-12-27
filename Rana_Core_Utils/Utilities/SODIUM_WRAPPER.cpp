#include "SODIUM_WRAPPER.h"

bool Initialize_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium, CONTEXT *ctx, int type) {
	char buffer[LOCAL_KEY_SIZE];
	char *temp_buffer_ptr;

	sodium->ctx = ctx;
	sodium->type = type;
	sodium->initialized = false;
	sodium->partner_ephemeral_pub_key_initialized = false;
	sodium->session_key_generated = false;
	sodium->signed_local_ephemeral_pub_key_size = crypto_sign_BYTES +
		crypto_kx_PUBLICKEYBYTES;
	sodium->signed_partner_ephemeral_pub_key_size = crypto_sign_BYTES +
		crypto_kx_PUBLICKEYBYTES;

	memset(sodium->nonce_rx, 0, crypto_secretbox_NONCEBYTES);
	memset(sodium->nonce_tx, 0, crypto_secretbox_NONCEBYTES);
	sodium_increment(sodium->nonce_tx, crypto_secretbox_NONCEBYTES);

	if (sodium_init() < 0) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Couldn't initialize libsodium");
		}

		return false;
	}

	if (!Read_Bin_From_File(LOCAL_KEY_NAME, buffer, LOCAL_KEY_SIZE)) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Failed to read key. Please "
				"run Interactive_Create_Key().");
		}
		return false;
	}

	temp_buffer_ptr = buffer;
	memcpy(sodium->local_pub_key, temp_buffer_ptr,
		crypto_sign_PUBLICKEYBYTES);

	temp_buffer_ptr += crypto_sign_PUBLICKEYBYTES;

	memcpy(sodium->local_priv_key, temp_buffer_ptr,
		crypto_sign_SECRETKEYBYTES);

	temp_buffer_ptr += crypto_sign_SECRETKEYBYTES;

	memcpy(sodium->partner_pub_key, temp_buffer_ptr,
		crypto_sign_PUBLICKEYBYTES);

	temp_buffer_ptr += crypto_sign_PUBLICKEYBYTES;

	if (crypto_kx_keypair(sodium->local_ephemeral_pub_key,
		sodium->local_ephemeral_priv_key) < 0) {

		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Couldn't generate "
				"local keypair.");
		}
		return false;
	}

	sodium->initialized = true;

	if (!Sign_Local_Pub_Key_SODIUM_WRAPPER(sodium)) {
		return false;
	}

	return true;
}

bool Interactive_Create_Key_Step_1_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium) {
	if (crypto_sign_keypair(sodium->local_pub_key,
		sodium->local_priv_key) < 0) {

		cout << "Couldn't generate local signing keypair." << endl;
		return false;
	}
	return true;
}

bool Interactive_Create_Key_Step_2_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium) {
	char buffer[LOCAL_KEY_SIZE];
	char *temp_buffer_ptr;

	temp_buffer_ptr = buffer;
	memcpy(temp_buffer_ptr, sodium->local_pub_key,
		crypto_sign_PUBLICKEYBYTES);

	temp_buffer_ptr += crypto_sign_PUBLICKEYBYTES;

	memcpy(temp_buffer_ptr, sodium->local_priv_key,
		crypto_sign_SECRETKEYBYTES);

	temp_buffer_ptr += crypto_sign_SECRETKEYBYTES;

	memcpy(temp_buffer_ptr, sodium->partner_pub_key,
		crypto_sign_PUBLICKEYBYTES);

	temp_buffer_ptr += crypto_sign_PUBLICKEYBYTES;

	if (!Write_Bin_To_File(LOCAL_KEY_NAME, buffer, LOCAL_KEY_SIZE)) {
		cout << "Failed to write file at " << LOCAL_KEY_NAME <<
			endl;
		return false;
	}

	return true;
}

bool Sign_Local_Pub_Key_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium) {
	unsigned long long signed_message_len;

	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return false;
	}

	if (crypto_sign(sodium->signed_local_ephemeral_pub_key,
		&signed_message_len, sodium->local_ephemeral_pub_key,
		crypto_kx_PUBLICKEYBYTES, sodium->local_priv_key) != 0 ||
		signed_message_len != crypto_sign_BYTES +
		crypto_kx_PUBLICKEYBYTES) {

		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Couldn't sign "
				"local ephemeral public key.");
		}
		return false;
	}

	return true;
}

bool Check_Partner_Ephemeral_Pub_Key_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium,
	char *payload, long long payload_size) {

	unsigned long long unsigned_message_len;

	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return false;
	}

	if (sodium->partner_ephemeral_pub_key_initialized) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Partner ephemeral public key "
				"already initialized.");
		}
		return false;
	}

	//possible segfault here...
	if (crypto_sign_open(sodium->partner_ephemeral_pub_key,
		&unsigned_message_len, (unsigned char*) payload, payload_size,
		sodium->partner_pub_key) != 0 || unsigned_message_len !=
		crypto_kx_PUBLICKEYBYTES) {

			LOG_ERROR_CTX((sodium->ctx)) {
				ADD_STR_LOG("message", "Couldn't validate "
					"partner ephemeral public key.");
			}
			return false;
	}

	sodium->partner_ephemeral_pub_key_initialized = true;

	return true;
}

bool Generate_Session_Key_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium) {
	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return false;
	}

	if (!sodium->partner_ephemeral_pub_key_initialized) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Partner ephemeral public key "
				"has not been initialized.");
		}

		return false;
	}

	if (sodium->session_key_generated) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Session keys have already "
				"been generated.");
		}
		return false;
	}

	switch (sodium->type) {
		case SODIUM_WRAPPER_SERVER_TYPE:
			if (crypto_kx_server_session_keys(sodium->local_rx_key,
				sodium->local_tx_key,
				sodium->local_ephemeral_pub_key,
				sodium->local_ephemeral_priv_key,
				sodium->partner_ephemeral_pub_key) != 0) {

				LOG_ERROR_CTX((sodium->ctx)) {
					ADD_STR_LOG("message", "Parner public \
					key is invalid.");
				}
				return false;
			}
			break;
		case SODIUM_WRAPPER_CLIENT_TYPE:
			if (crypto_kx_client_session_keys(sodium->local_rx_key,
				sodium->local_tx_key,
				sodium->local_ephemeral_pub_key,
				sodium->local_ephemeral_priv_key,
				sodium->partner_ephemeral_pub_key) != 0) {

				LOG_ERROR_CTX((sodium->ctx)) {
					ADD_STR_LOG("message", "Parner public \
					key is invalid.");
				}
				return false;
			}
			break;
		case SODIUM_WRAPPER_INTERACTIVE_CREATE_KEY_TYPE:
			LOG_ERROR_CTX((sodium->ctx)) {
				ADD_STR_LOG("message", "Please do not "
					"generate session keys from "
					"INTERACTIVE_CREATE_KEY.");
			}
			return false;
		default:
			LOG_ERROR_CTX((sodium->ctx)) {
				ADD_STR_LOG("message", "Unknown type.");
				ADD_INT_LOG("type", sodium->type);
			}
			return false;
	}

	sodium->session_key_generated = true;

	return true;
}

//quick diagram of how output_payload looks
//[nonce][encrypted data]

//output_payload should at least be crypto_secretbox_MACBYTES +
//crypto_secretbox_NONCEBYTES + input_payload_size
int Encrypt_Message_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium, char *input_payload,
	int input_payload_size, char *output_payload) {

	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return -1;
	}

	if (!sodium->session_key_generated) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Session keys have not been "
				"generated yet.");
		}

		return -1;
	}

	if (crypto_secretbox_easy((unsigned char*) output_payload +
		crypto_secretbox_NONCEBYTES, (unsigned char*) input_payload,
		input_payload_size, sodium->nonce_tx,
		sodium->local_tx_key) != 0) {

		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Failed to encrypt payload.");
		}

		return -1;
	}

	memcpy(output_payload, sodium->nonce_tx, crypto_secretbox_NONCEBYTES);

	sodium_increment(sodium->nonce_tx, crypto_secretbox_NONCEBYTES);

	return crypto_secretbox_MACBYTES + crypto_secretbox_NONCEBYTES +
		input_payload_size;
}

//quick diagram of how input_payload looks
//[nonce][encrypted data]

int Decrypt_Message_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium, char *input_payload,
	int input_payload_size, char *output_payload) {

	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return -1;
	}

	if (!sodium->session_key_generated) {
		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Session keys have not been "
				"generated yet.");
		}

		return -1;
	}

	if (input_payload_size <= crypto_secretbox_MACBYTES +
		crypto_secretbox_NONCEBYTES) {

		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Input payload size is crazy.");
			ADD_INT_LOG("input_payload_size",
				input_payload_size);
		}

		return -1;
	}

	if (sodium_compare((unsigned char*) input_payload, sodium->nonce_rx,
		crypto_secretbox_NONCEBYTES) < 1) {

		//this really only applies in the UDP case. just drop packets
		//that come out of order. may need to eventually change this
		//if you don't want to drop packets that come out of order.
		//also not printing an error here because UDP does pretty often
		//re-order packets. Will be a performance hit to print
		//everytime. FYI this is technically not required. libsodium
		//should be able to decode out of order so long as the nonce
		//is known.
		return -1;
	}

	memcpy(sodium->nonce_rx, input_payload, crypto_secretbox_NONCEBYTES);

	if (crypto_secretbox_open_easy((unsigned char*) output_payload,
		(unsigned char*) input_payload + crypto_secretbox_NONCEBYTES,
		input_payload_size - crypto_secretbox_NONCEBYTES,
		sodium->nonce_rx, sodium->local_rx_key) != 0) {

		LOG_ERROR_CTX((sodium->ctx)) {
			ADD_STR_LOG("message", "Failed to decrypt payload.");
		}

		return -1;
	}

	return input_payload_size - crypto_secretbox_MACBYTES -
		crypto_secretbox_NONCEBYTES;
}

void Delete_SODIUM_WRAPPER(SODIUM_WRAPPER *sodium) {
	if (!sodium->initialized) {
		//can't log, ctx might be missing... carry on
		return;
	}

	sodium->initialized = false;
}
