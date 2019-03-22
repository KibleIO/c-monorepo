#ifndef ENCRYPTION_ENGINE_H_
#define ENCRYPTION_ENGINE_H_

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <Utilities/LOGGING.h>
#include "ENCRYPTION_PROFILE.h"

#define MAX_PROFILES 25

using namespace std;

struct ENCRYPTION_ENGINE {
	DIR*					directory_pointer;
	dirent*					dirent_pointer;
	int8_t*					path;
	struct stat				file_info;
	ENCRYPTION_PROFILE**	profiles_available;
	uint8_t					number_of_profiles;
	int						active_profile;
};

bool Initialize_ENCRYPTION_ENGINE	(ENCRYPTION_ENGINE*);
bool Add_Profile_ENCRYPTION_ENGINE	(ENCRYPTION_ENGINE*, string, uint8_t = 0);
void Delete_ENCRYPTION_ENGINE		(ENCRYPTION_ENGINE*);

#endif
