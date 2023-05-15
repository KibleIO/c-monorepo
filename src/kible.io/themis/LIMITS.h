#ifndef LIMITS_H_
#define LIMITS_H_

#define __CORE_SYSTEM__ "THEMIS"
#define SOFTWARE_VERSION "3.1.11" //poseidon
#define FPS 60
#define HOST_PORT 3330
#define THEMIS_PORT 4460
#define THEMIS_HTTP_PORT 4461
#define FILEBROWSER_PORT 4461
#define THEMIS_BASE_PORT 5550
#define SHM_BASE 12340

//#define __UBUNTU__
#define __DEBIAN__

#define ROOT_DIR \
	std::string( \
		"/root/THEMIS/")

/*======= Rana update data ========*/
#define PATH_TO_VERSION "/beta/themis/version.html"

// Define the remote site with the latest executable
#define REMOTE_FILE_ARCHIVE_NAME "themis.tar.gz"

#define PATH_TO_FILE_ARCHIVE (string("/beta/themis/") + REMOTE_FILE_ARCHIVE_NAME)

// #define EXTERNAL_LOGS_APIS
#endif
