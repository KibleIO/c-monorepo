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
    if (
    (encryption_engine->directory_pointer = opendir(ENCRYPTION_PROFILE_PATH)) ==
    NULL) {
		log_err("Error opening input directory " + to_string(errno));
		return false;
    }

    while (
    (encryption_engine->dirent_pointer =
    readdir(encryption_engine->directory_pointer)) != NULL) {
		sprintf((char*) encryption_engine->path, "%s/%s",
		ENCRYPTION_PROFILE_PATH, encryption_engine->dirent_pointer->d_name);
		log_dbg(string(encryption_engine->dirent_pointer->d_name));

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
				log_err(
				"Error initializign profile " +
				string((char*) encryption_engine->path));


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
				log_err("Failed to load master key and nonce for profile");
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
		log_err("Maximum number of profiles reached");
		return false;
	}

	if (origin_profile_index >= encryption_engine->number_of_profiles) {
		log_err(
		"The profile index is out of range or no profiles currently exist");
		return false;
	}

	uint8_t* origin_entropy_buffer =
	new uint8_t[crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES];

	Get_Entropy_ENCRYPTION_PROFILE(
	encryption_engine->profiles_available[origin_profile_index],
	origin_entropy_buffer, crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES);

	encryption_engine->profiles_available[
	encryption_engine->number_of_profiles] = new ENCRYPTION_PROFILE;

	if (
	!Initialize_ENCRYPTION_PROFILE(encryption_engine->profiles_available
	[encryption_engine->number_of_profiles], origin_entropy_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES,
	identifier)) {
		log_err("Error initializign profile " + identifier);

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
		log_err("Failed to load master key and nonce for profile");
		return false;
	}

	encryption_engine->number_of_profiles++;
	delete [] origin_entropy_buffer;
	return true;
}

bool Write_Active_Profile_ENCRYPTION_ENGINE(
ENCRYPTION_ENGINE* encryption_engine) {
	log_dbg("writing active profile");
	ENCRYPTION_PROFILE* encryption_profile = 
	encryption_engine->profiles_available[encryption_engine->active_profile];

	uint8_t* refill_buffer = new uint8_t[crypto_stream_chacha20_NONCEBYTES +
	crypto_stream_chacha20_KEYBYTES];

	if (
	!Get_Entropy_ENCRYPTION_PROFILE(encryption_profile, refill_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES)) {
		log_err("unable to get entropy");
		delete [] refill_buffer;
		return false;
	}

	sodium_increment(
	encryption_profile->master_nonce, crypto_stream_chacha20_NONCEBYTES);

	if (
	!Update_ENCRYPTION_PROFILE(encryption_profile, refill_buffer,
	crypto_stream_chacha20_NONCEBYTES + crypto_stream_chacha20_KEYBYTES)) {
		log_err("Failed to update profile file");
		delete [] refill_buffer;
		return false;
    }

	delete [] refill_buffer;
	log_dbg("done writing active profile");
	return true;
}

void Delete_ENCRYPTION_ENGINE(ENCRYPTION_ENGINE* encryption_engine) {
	log_dbg("deleting profiles");
	for (int i = 0; i < MAX_PROFILES; i++) {
		if (encryption_engine->profiles_available[i] != NULL) {
			Delete_ENCRYPTION_PROFILE(encryption_engine->profiles_available[i]);
			delete encryption_engine->profiles_available[i];
		}
	}
	log_dbg("deleting profiles array");
	delete [] encryption_engine->profiles_available;
	log_dbg("deleting path");
	delete [] encryption_engine->path;
	log_dbg("done deleting path");
}
