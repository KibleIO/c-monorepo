// PLATFORMS LINUX
#ifndef UPDATE_UTILITY_H_
#define UPDATE_UTILITY_H_

#include <curl/curl.h>
#include <string.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include "UTILS.h"

using namespace std;

#define UPDATE_IP string("http://146.190.60.102")

// Directly modified by function pointer to get a string from a website
extern string str_ret;

struct Version {
	int major, minor, revision;
	Version(const string& version) {
		sscanf(version.c_str(), "%d.%d.%d", &major, &minor, &revision);
	}
	tuple<int, int, int> toTuple() const {
		return make_tuple(major, minor, revision);
	}
};

struct Update_UTILITY {
	string latestVersionSite;
	string fileArchiveName;
	string latestVersionRemoteSite;
	string rootDir;
	string localArchiveDir;
	string installBinaryDir;
};

bool operator<(const Version& v1, const Version& v2);
bool operator<=(const Version& v1, const Version& v2);
bool operator>(const Version& v1, const Version& v2);
bool operator>=(const Version& v1, const Version& v2);
bool operator==(const Version& v1, const Version& v2);

bool Initialize_UPDATE_UTILITY(Update_UTILITY* info, string relativePathToVersionHtml,
							   string fileArchiveName, string relativePathTofileArchive,
							   string rootDir);
// Check the remote site to see if this Rana machine needs an update
// Return true if it is up to date and false otherwise
bool Up_To_Date(string currVersion, Update_UTILITY* info);
// Update rana by downloading the package from the remote site
// and running the update shell command
void Update_System(Update_UTILITY* info, bool concurrent = true);
// Pointed to by curl function - sets global string based on data
// recieved from website
size_t Set_String_From_Data(void* ptr, size_t size, size_t nneb, void* stream);
// Pointed to by curl function - writes data from website into
// given file stream
size_t Save_Date_From_Website(void* ptr, size_t size, size_t nmemb, FILE*);
// Get a string from the specified ip address
string Get_Str_From_Website(string address);
// Download file from given IP and save it to given directory
bool Download_From_Website(string address, string place, uint32_t* runner, char* error);

void Delete_UPDATE_UTILITY(Update_UTILITY* info);

#endif	// UPDATE_UTILITY_H_
