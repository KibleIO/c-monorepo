// PLATFORMS LINUX
#ifdef __linux__
#include "Update_Utility.h"

string str_ret;

bool operator<(const Version& v1, const Version& v2) {
	return v1.toTuple() < v2.toTuple();
}
bool operator<=(const Version& v1, const Version& v2) {
	return v1.toTuple() <= v2.toTuple();
}
bool operator>(const Version& v1, const Version& v2) {
	return v1.toTuple() > v2.toTuple();
}
bool operator>=(const Version& v1, const Version& v2) {
	return v1.toTuple() >= v2.toTuple();
}
bool operator==(const Version& v1, const Version& v2) {
	return v1.toTuple() == v2.toTuple();
}

bool Initialize_UPDATE_UTILITY(Update_UTILITY* info, string pathToVersionHtml,
							   string fileArchiveName, string pathTofileArchive,
							   string rootDir) {
	info->latestVersionSite = UPDATE_IP + pathToVersionHtml;
	info->fileArchiveName = fileArchiveName;
	info->latestVersionRemoteSite = UPDATE_IP + pathTofileArchive;
	info->rootDir = rootDir;
	info->localArchiveDir = rootDir + fileArchiveName;
	info->installBinaryDir = rootDir + "install.sh";
	return true;
}

bool Up_To_Date(string currVersion, Update_UTILITY* info) {
	cout << "Grabbing latest version" << endl;
	// Get the latest version
	string latest_version = Get_Str_From_Website(info->latestVersionSite);
	//    Remove_Whitespace(&latest_version);
	//    Remove_Whitespace(&current_version);
	cout << "Latest version grabbed: " << latest_version << endl;
	cout << "Current version:        " << currVersion << endl;
	// The system needs an update if the latest_version
	// does not equal this version
	return Version(latest_version) == Version(currVersion);
}

void Update_System(Update_UTILITY* info, bool concurrent) {
	cout << "started update" << endl;
	// Number of tasks in the update process, and the ratios of
	// how much time it is expected to take for each task relative to the others
	const int total_tasks = 3;
	float* task_time_ratios = new float[total_tasks]{0.1, 0.1, 0.3};
	uint32_t temp = 0;
	// Start tracking progress
	//    Start_Progress_Tracker(progress, task_time_ratios, total_tasks);
	// Download the tar file
	cout << "downloading" << endl;
	Download_From_Website(info->latestVersionRemoteSite,
						  info->rootDir + info->fileArchiveName, &temp);
	//    Update_Progress_Tracker(progress);
	// Extract the contents of the tar file
	cout << "extracting" << endl;
	system((string("tar -zxf ") + info->rootDir + info->fileArchiveName +
			" -C " + info->rootDir)
			   .c_str());
	//    Update_Progress_Tracker(progress);
	// Run the install file that was packaged in the tar file
	cout << "installing" << endl;
	if (concurrent)
		system((string("bash ") + info->rootDir + "install.sh" + " &").c_str());
	else {
		system((string("bash ") + info->rootDir + "install.sh").c_str());
	}
	//    Update_Progress_Tracker(progress);

	//    if(Progress_Finished(progress)) {
	//        cout << "Progress is finished" << endl;
	//    }
}

void Set_String_From_Data(void* ptr, size_t size, size_t nneb, void* stream) {
	// Bypass compiler warnings
	(void)size;
	(void)stream;
	str_ret = string((char*)ptr, nneb);
}

size_t Save_Date_From_Website(void* ptr, size_t size, size_t nmemb,
							  FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

string Get_Str_From_Website(string address) {
	str_ret = "";
	CURL* curl;
	CURLcode res;
	(void)res;
	const char* url = address.c_str();
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Set_String_From_Data);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return str_ret;
}

void Download_From_Website(string address, string place, uint32_t* runner) {
	CURL* curl;
	FILE* fp;
	CURLcode res;
	(void)res;
	const char* url = address.c_str();
	const char* outfilename = place.c_str();
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(outfilename, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Save_Date_From_Website);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	*runner = false;
}

void Delete_UPDATE_UTILITY(Update_UTILITY* info) {
}

#endif
