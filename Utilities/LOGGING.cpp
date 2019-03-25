#include "LOGGING.h"

uint8_t GOT_LOCAL_TIME = false;
string LOG_FILE = string(LOG_DIR) + "/log";

void set_local_start_time() {
	if (GOT_LOCAL_TIME) return;
	GOT_LOCAL_TIME = true;

    dirent**							file_name_list;
	uint32_t							num_files;
	ostringstream						ss;
	chrono::system_clock::time_point	now;
	time_t								now_t;

	//path	= new char[PATH_MAX];
	now		= chrono::system_clock::now();
	now_t	= chrono::system_clock::to_time_t(now);

	ss << put_time(localtime(&now_t), "_%F_%H.%M.%S");
	LOG_FILE += ss.str();

	num_files = scandir(LOG_DIR, &file_name_list, NULL, alphasort);
	if (num_files < 0) {
		cout << "logging is disabled: " << to_string(errno) << endl;
		return;
	}

	//don't need to account for directories '.' and '..'
	free(file_name_list[0]);
	free(file_name_list[1]);

	for (int i = 2; i < num_files; i++) {
		if (i < num_files - 20) {
			unlink(
			string(string(LOG_DIR) + "/" + file_name_list[i]->d_name).c_str());
		}
		free(file_name_list[i]);
	}
	free(file_name_list);

	return;
}

void Write_Message(
string type, string message, string file, uint32_t line, string func) {
	set_local_start_time();
	ofstream out(LOG_FILE, ofstream::out | ofstream::app);
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);

	out << type << "[" <<  put_time(localtime(&now_t), "%T thr") <<
	syscall(__NR_gettid) << " \"" << file << "\":" << line << " " << func <<
	"()] " << message << endl;

	out.close();
}
