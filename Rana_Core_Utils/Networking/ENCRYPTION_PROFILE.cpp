//PLATFORMS: Linux, Windows, OSX (TODO)

#include "ENCRYPTION_PROFILE.h"

bool Initialize_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	string file_name, int32_t file_size) {

	ifstream profile_file(file_name, ios::binary);

	encryption_profile->identifier			= file_name.substr(
	string(ENCRYPTION_PROFILE_PATH).length() + 1);
	log_dbg(((const JSON_TYPE){
		{"message", "adding profile"},
		{"profile", "encryption_profile->identifier"},
		JSON_TYPE_END}));
	encryption_profile->original_pool_size	= file_size;
	encryption_profile->entropy_pool		= new uint8_t[file_size];
	encryption_profile->master_nonce		= NULL;
	encryption_profile->master_key			= NULL;
	encryption_profile->poly1305_nonce		= NULL;
	encryption_profile->poly1305_key		= NULL;
	encryption_profile->generichash_buffer	= NULL;
	encryption_profile->entropy_pool_index	= 0;
	encryption_profile->saved				= true;

	if (!profile_file.is_open()) {
		log_err(((const JSON_TYPE){
			{"message", "could not open"},
			{"profile", "encryption_profile->identifier"},
			JSON_TYPE_END}));
		return false;
	}

	profile_file.read(
	(char*)encryption_profile->entropy_pool,
	encryption_profile->original_pool_size);

	profile_file.close();
	log_dbg(((const JSON_TYPE){
		{"message", "added profile"},
		{"profile", "encryption_profile->identifier"},
		JSON_TYPE_END}));
	return true;
}

bool Initialize_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	uint8_t* entropy_pool_input, int32_t entropy_pool_input_size,
	string identifier) {

	encryption_profile->identifier			= identifier;
	log_dbg(((const JSON_TYPE){
		{"message", "adding profile"},
		{"profile", "encryption_profile->identifier"},
		JSON_TYPE_END}));
	encryption_profile->original_pool_size	= entropy_pool_input_size;
	encryption_profile->entropy_pool		=
	new uint8_t[entropy_pool_input_size];

	encryption_profile->master_nonce		= NULL;
	encryption_profile->master_key			= NULL;
	encryption_profile->poly1305_nonce		= NULL;
	encryption_profile->poly1305_key		= NULL;
	encryption_profile->generichash_buffer	= NULL;
	encryption_profile->entropy_pool_index	= 0;
	encryption_profile->saved				= false;

	copy(entropy_pool_input, entropy_pool_input + entropy_pool_input_size,
	encryption_profile->entropy_pool);
	log_dbg(((const JSON_TYPE){
		{"message", "added profile"},
		{"profile", "encryption_profile->identifier"},
		JSON_TYPE_END}));
	return true;
}

bool Update_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	uint8_t* new_entropy_pool, uint32_t new_entropy_pool_size) {

	if (!encryption_profile->saved) {
		return true;
	}
	ofstream profile_file;

	profile_file.open(ENCRYPTION_PROFILE_PATH + string("/") +
	encryption_profile->identifier, ios::binary);

	if (!profile_file.is_open()) {
		log_err(((const JSON_TYPE){
			{"message", "could not open"},
			{"profile", "encryption_profile->identifier"},
			JSON_TYPE_END}));
		return false;
	}

	profile_file.write((char*)new_entropy_pool, new_entropy_pool_size);
	profile_file.close();
	return true;
}

bool Load_Master_Key_And_Nonce_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile) {

	if (
	encryption_profile->original_pool_size -
	encryption_profile->entropy_pool_index < crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES) {
		log_err(((const JSON_TYPE){
			{"message", "Entropy pool is not large enough"},
			JSON_TYPE_END}));
		return false;
	}

	if (sodium_init() < 0) {
		log_err(((const JSON_TYPE){
			{"message", "Sodium failed to initialize"},
			JSON_TYPE_END}));
		return false;
	}

	encryption_profile->master_nonce	=
	new uint8_t[crypto_stream_chacha20_NONCEBYTES];

	encryption_profile->master_key		=
	new uint8_t[crypto_stream_chacha20_KEYBYTES];

	copy(
	encryption_profile->entropy_pool + encryption_profile->entropy_pool_index,
	encryption_profile->entropy_pool + encryption_profile->entropy_pool_index +
	crypto_stream_chacha20_NONCEBYTES, encryption_profile->master_nonce);

	encryption_profile->entropy_pool_index += crypto_stream_chacha20_NONCEBYTES;

	copy(
	encryption_profile->entropy_pool + encryption_profile->entropy_pool_index,
	encryption_profile->entropy_pool + encryption_profile->entropy_pool_index +
	crypto_stream_chacha20_KEYBYTES, encryption_profile->master_key);

	encryption_profile->entropy_pool_index += crypto_stream_chacha20_KEYBYTES;

	return true;
}

bool Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile) {

	encryption_profile->generichash_buffer	=
	new uint8_t[crypto_onetimeauth_KEYBYTES];

	encryption_profile->poly1305_nonce		=
	new uint8_t[crypto_onetimeauth_KEYBYTES]; //arbitrarily decided

	encryption_profile->poly1305_key		=
	new uint8_t[crypto_onetimeauth_KEYBYTES];

	if (!Get_Entropy_ENCRYPTION_PROFILE(encryption_profile,
		encryption_profile->poly1305_nonce,
		crypto_onetimeauth_KEYBYTES)) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to get entropy"},
			JSON_TYPE_END}));
		return false;
	}

	sodium_increment(encryption_profile->master_nonce,
		crypto_stream_chacha20_NONCEBYTES);

	if (!Get_Entropy_ENCRYPTION_PROFILE(encryption_profile,
		encryption_profile->poly1305_key,
		crypto_onetimeauth_KEYBYTES)) {

		log_err(((const JSON_TYPE){
			{"message", "Failed to get entropy"},
			JSON_TYPE_END}));
		return false;
	}

	sodium_increment(encryption_profile->master_nonce,
		crypto_stream_chacha20_NONCEBYTES);

	return true;
}

bool Get_Entropy_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	uint8_t* buffer, uint32_t size) {

	if (crypto_stream_chacha20(buffer, size,
		encryption_profile->master_nonce,
		encryption_profile->master_key) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Sodium failed to initialize"},
			JSON_TYPE_END}));
		return false;
	}
	return true;
}

bool Encrypt_Data_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	uint8_t* message_buffer, uint32_t message_size, uint8_t* output_buffer,
	uint8_t* nonce_temporary) {

	if (crypto_stream_chacha20_xor(output_buffer, message_buffer,
		message_size, nonce_temporary,
		encryption_profile->master_key) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Could not encrypt data"},
			{"name", "name"},
			JSON_TYPE_END}));
		return false;
	}
	return true;
}

bool Encrypt_Data_ENCRYPTION_PROFILE(
ENCRYPTION_PROFILE* encryption_profile, uint8_t* message_buffer,
uint32_t message_size, uint8_t* output_buffer) {
	if (
	!Encrypt_Data_ENCRYPTION_PROFILE(encryption_profile, message_buffer,
	message_size, output_buffer, encryption_profile->master_nonce)) {
		return false;
	}

	sodium_increment(
	encryption_profile->master_nonce, crypto_stream_chacha20_NONCEBYTES);

	return true;
}

bool Decrypt_Data_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile,
	uint8_t* encrypted_buffer, uint32_t encrypted_size,
	uint8_t* output_buffer, uint8_t* nonce_temporary) {

	if (crypto_stream_chacha20_xor(output_buffer, encrypted_buffer,
		encrypted_size, nonce_temporary,
		encryption_profile->master_key) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Could not decrypt data"},
			JSON_TYPE_END}));
		return false;
	}
	return true;
}

bool Decrypt_Data_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE *encryption_profile,
	uint8_t *encrypted_buffer, uint32_t encrypted_size,
	uint8_t *output_buffer) {

	if (!Decrypt_Data_ENCRYPTION_PROFILE(encryption_profile,
		encrypted_buffer, encrypted_size, output_buffer,
		encryption_profile->master_nonce)) {

		return false;
	}

	sodium_increment(encryption_profile->master_nonce,
		crypto_stream_chacha20_NONCEBYTES);

	return true;
}

bool Generate_Auth_Code_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile, uint8_t* message_buffer,
	uint32_t message_size, uint8_t* output_buffer,
	uint8_t* nonce_temporary) {

	if (crypto_generichash(encryption_profile->generichash_buffer,
		crypto_onetimeauth_KEYBYTES, nonce_temporary,
		crypto_onetimeauth_KEYBYTES, encryption_profile->poly1305_key,
		crypto_onetimeauth_KEYBYTES) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Could not generate a generic hash"},
			JSON_TYPE_END}));
		return false;
	}

	if (crypto_onetimeauth(output_buffer, message_buffer, message_size,
		encryption_profile->generichash_buffer) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Could not generate a auth token"},
			JSON_TYPE_END}));
		return false;
	}

	return true;
}

bool Generate_Auth_Code_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile, uint8_t* message_buffer,
	uint32_t message_size, uint8_t* output_buffer) {

	if (!Generate_Auth_Code_ENCRYPTION_PROFILE(encryption_profile,
		message_buffer, message_size, output_buffer,
		encryption_profile->poly1305_nonce)) {

		return false;
	}

	sodium_increment(encryption_profile->poly1305_nonce,
		crypto_onetimeauth_KEYBYTES);

	return true;
}

bool Authenticate_Auth_Code_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile, uint8_t* message_buffer,
	uint32_t message_size, uint8_t* auth_buffer, uint8_t* nonce_temporary) {

	if (crypto_generichash(encryption_profile->generichash_buffer,
		crypto_onetimeauth_KEYBYTES, nonce_temporary,
		crypto_onetimeauth_KEYBYTES, encryption_profile->poly1305_key,
		crypto_onetimeauth_KEYBYTES) < 0) {

		log_err(((const JSON_TYPE){
			{"message", "Could not generate a generic hash"},
			JSON_TYPE_END}));
		return false;
	}

	if (crypto_onetimeauth_verify(auth_buffer, message_buffer, message_size,
		encryption_profile->generichash_buffer) != 0) {

		log_err(((const JSON_TYPE){
			{"message", "Buffer is either corrupted or has been tampered with"},
			JSON_TYPE_END}));
		return false;
	}

	return true;
}

bool Authenticate_Auth_Code_ENCRYPTION_PROFILE(
	ENCRYPTION_PROFILE* encryption_profile, uint8_t* message_buffer,
	uint32_t message_size, uint8_t* auth_buffer) {

	if (!Authenticate_Auth_Code_ENCRYPTION_PROFILE(encryption_profile,
		message_buffer, message_size, auth_buffer,
		encryption_profile->poly1305_nonce)) {
		return false;
	}

	sodium_increment(encryption_profile->poly1305_nonce,
		crypto_onetimeauth_KEYBYTES);

	return true;
}

void Delete_ENCRYPTION_PROFILE(ENCRYPTION_PROFILE* encryption_profile) {
	delete [] encryption_profile->entropy_pool;

	if (encryption_profile->master_nonce != NULL) {
		delete [] encryption_profile->master_nonce;
	}
	if (encryption_profile->master_key != NULL) {
		delete [] encryption_profile->master_key;
	}
	if (encryption_profile->poly1305_nonce != NULL) {
		delete [] encryption_profile->poly1305_nonce;
	}
	if (encryption_profile->poly1305_key != NULL) {
		delete [] encryption_profile->poly1305_key;
	}
	if (encryption_profile->generichash_buffer != NULL) {
		delete [] encryption_profile->generichash_buffer;
	}
}
