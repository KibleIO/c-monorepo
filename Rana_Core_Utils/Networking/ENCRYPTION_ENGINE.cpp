//PLATFORMS: Linux, Windows, OSX (TODO)

#include "ENCRYPTION_ENGINE.h"

bool Initialize_ENCRYPTION_ENGINE(ENCRYPTION_ENGINE* encryption_engine) {
	encryption_engine->profiles_available	=
	new ENCRYPTION_PROFILE*[MAX_PROFILES];

	encryption_engine->number_of_profiles	= 0;
	encryption_engine->active_profile		= -1;
	encryption_engine->path					= new int8_t[PATH_MAX];

	for (int i = 0; i < MAX_PROFILES; i++) {
		encryption_engine->profiles_available[i] = NULL;
	}

	//okay go find all of the encryption profiles and load them up
	if ((encryption_engine->directory_pointer =
		opendir(ENCRYPTION_PROFILE_PATH)) == NULL) {

		log_err(((const JSON_TYPE){
			{"message", "Error opening input directory"},
			JSON_TYPE_END}));
		return false;
	}

	while ((encryption_engine->dirent_pointer =
		readdir(encryption_engine->directory_pointer)) != NULL) {

		sprintf((char*) encryption_engine->path, "%s/%s",
		ENCRYPTION_PROFILE_PATH, encryption_engine->dirent_pointer->d_name);
		log_dbg(((const JSON_TYPE){
			{"message", "string(encryption_engine->dirent_pointer->d_name)"},
			JSON_TYPE_END}));

		// Linux specific code {{{
		#ifdef __linux__
		lstat((char*)encryption_engine->path, &encryption_engine->file_info);
		#endif
		// }}} Windows specific code {{{
		#ifdef _WIN64
		stat((char*)encryption_engine->path, &encryption_engine->file_info);
		#endif
		// }}} OSX specific code {{{
		#ifdef __APPLE__
		//TODO apple code
		#endif
		// }}}

		if (!S_ISDIR(encryption_engine->file_info.st_mode)) {
			encryption_engine->profiles_available[
			encryption_engine->number_of_profiles] = new ENCRYPTION_PROFILE;

			if (
			!Initialize_ENCRYPTION_PROFILE(encryption_engine->profiles_available
			[encryption_engine->number_of_profiles],
			string((char*) encryption_engine->path),
			encryption_engine->file_info.st_size)) {
				log_err(((const JSON_TYPE){
					{"message", "Error initializign profile"},
					JSON_TYPE_END}));


				Delete_ENCRYPTION_PROFILE(
				encryption_engine->profiles_available[
				encryption_engine->number_of_profiles]);

				delete encryption_engine->profiles_available[
				encryption_engine->number_of_profiles];

				encryption_engine->profiles_available[
				encryption_engine->number_of_profiles] = NULL;

				return false;
			}
			encryption_engine->number_of_profiles++;
		}
	}
	closedir(encryption_engine->directory_pointer);

	//load up some master keys for all the profiles
	for (int i = 0; i < MAX_PROFILES; i++) {
		if (encryption_engine->profiles_available[i] != NULL) {
			if (
			!Load_Master_Key_And_Nonce_ENCRYPTION_PROFILE(
			encryption_engine->profiles_available[i])) {
				log_err(((const JSON_TYPE){
					{"message", "Failed to load master key and nonce for profile"},
					JSON_TYPE_END}));
				return false;
			}
		}
	}
	return true;
}

bool Add_Profile_ENCRYPTION_ENGINE(
ENCRYPTION_ENGINE* encryption_engine, string identifier,
uint8_t origin_profile_index) {
	if (encryption_engine->number_of_profiles >= MAX_PROFILES) {
		log_err(((const JSON_TYPE){
			{"message", "Maximum number of profiles reached"},
			JSON_TYPE_END}));
		return false;
	}

	if (origin_profile_index >= encryption_engine->number_of_profiles) {
		log_err(((const JSON_TYPE){
			{"message", "The profile index is out of range or no profiles currently exist"},
			JSON_TYPE_END}));
		return false;
	}

	uint8_t* origin_entropy_buffer =
	new uint8_t[crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES];

	if (
	!Get_Entropy_ENCRYPTION_PROFILE(
	encryption_engine->profiles_available[origin_profile_index],
	origin_entropy_buffer, crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES)) {
		log_err(((const JSON_TYPE){
			{"message", "unable to get entropy"},
			JSON_TYPE_END}));
		delete [] origin_entropy_buffer;
		return false;
	}

	sodium_increment(
	encryption_engine->profiles_available[origin_profile_index]->master_nonce,
	crypto_stream_chacha20_NONCEBYTES);

	encryption_engine->profiles_available[
	encryption_engine->number_of_profiles] = new ENCRYPTION_PROFILE;

	if (
	!Initialize_ENCRYPTION_PROFILE(encryption_engine->profiles_available
	[encryption_engine->number_of_profiles], origin_entropy_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES,
	identifier)) {
		log_err(((const JSON_TYPE){
			{"message", "Error initializign profile"},
			JSON_TYPE_END}));

		Delete_ENCRYPTION_PROFILE(
		encryption_engine->profiles_available[
		encryption_engine->number_of_profiles]);

		delete encryption_engine->profiles_available[
		encryption_engine->number_of_profiles];

		delete [] origin_entropy_buffer;

		encryption_engine->profiles_available[
		encryption_engine->number_of_profiles] = NULL;

		return false;
	}

	ENCRYPTION_PROFILE* enc_prof = encryption_engine->profiles_available[
	encryption_engine->number_of_profiles];

	if (!Load_Master_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to load master key and nonce for profile"},
			JSON_TYPE_END}));
		return false;
	}

	encryption_engine->number_of_profiles++;
	delete [] origin_entropy_buffer;
	return true;
}

ENCRYPTION_PROFILE* Get_New_Profile_ENCRYPTION_ENGINE(
ENCRYPTION_ENGINE* enc_eng, string name) {
	ENCRYPTION_PROFILE* enc_prof;

	if (
	!Add_Profile_ENCRYPTION_ENGINE(enc_eng, name, enc_eng->active_profile)) {
		return NULL;
	}

	enc_prof =
	enc_eng->profiles_available[enc_eng->number_of_profiles - 1];

	if (!Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE(enc_prof)) {
		return NULL;
	}

	return enc_prof;
}

bool Write_Active_Profile_ENCRYPTION_ENGINE(
ENCRYPTION_ENGINE* encryption_engine) {
	log_dbg(((const JSON_TYPE){
		{"message", "writing active profile"},
		JSON_TYPE_END}));
	ENCRYPTION_PROFILE* encryption_profile =
	encryption_engine->profiles_available[encryption_engine->active_profile];

	uint8_t* refill_buffer = new uint8_t[crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES];

	if (
	!Get_Entropy_ENCRYPTION_PROFILE(encryption_profile, refill_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES)) {
		log_err(((const JSON_TYPE){
			{"message", "unable to get entropy"},
			JSON_TYPE_END}));
		delete [] refill_buffer;
		return false;
	}

	sodium_increment(
	encryption_profile->master_nonce, crypto_stream_chacha20_NONCEBYTES);

	if (
	!Update_ENCRYPTION_PROFILE(encryption_profile, refill_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES)) {
		log_err(((const JSON_TYPE){
			{"message", "Failed to update profile file"},
			JSON_TYPE_END}));
		delete [] refill_buffer;
		return false;
    }

	delete [] refill_buffer;
	log_dbg(((const JSON_TYPE){
		{"message", "done writing active profile"},
		JSON_TYPE_END}));
	return true;
}

void Delete_ENCRYPTION_ENGINE(ENCRYPTION_ENGINE* encryption_engine) {
	log_dbg(((const JSON_TYPE){
		{"message", "deleting profiles"},
		JSON_TYPE_END}));
	for (int i = 0; i < MAX_PROFILES; i++) {
		if (encryption_engine->profiles_available[i] != NULL) {
			Delete_ENCRYPTION_PROFILE(encryption_engine->profiles_available[i]);
			delete encryption_engine->profiles_available[i];
		}
	}
	log_dbg(((const JSON_TYPE){
		{"message", "deleting profiles array"},
		JSON_TYPE_END}));
	delete [] encryption_engine->profiles_available;
	log_dbg(((const JSON_TYPE){
		{"message", "deleting path"},
		JSON_TYPE_END}));
	delete [] encryption_engine->path;
	log_dbg(((const JSON_TYPE){
		{"message", "done deleting path"},
		JSON_TYPE_END}));
}
