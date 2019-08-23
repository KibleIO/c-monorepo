//PLATFORMS: Linux, Windows, OSX (TODO)

#include "LOGGING.h"

uint8_t GOT_LOCAL_TIME = false;
string LOG_FILE = string(LOG_DIR) + "/log";

#ifdef __linux__
void Check_Logs() {
	string lswc = system_output(string("ls ") + LOG_DIR + " | wc -l");
	lswc.pop_back();

	if (lswc == "0") {
		log_dbg("there does not appear to be any old logs");
		return;
	}

	string prev_log = system_output(
	string("ls ") + LOG_DIR + " -t | head -n2 | tail -n1");
	prev_log.pop_back();
	prev_log = string(LOG_DIR) + "/" + prev_log;

	string exit_status = system_output(
	"cat " + prev_log + " | tail -n1 | awk '{print $1}'");
	exit_status.pop_back();

	if (exit_status != "CLOSE:") {
		log_dbg("previous log shows rana exited unexpectedly, sending log");
		system(("echo 'log file' | mail -s 'Rana Crash Report' -A '" +
		prev_log + "' pleasedontblockme1@yandex.com &").c_str());
	} else {
		log_dbg("previous log shows rana exited successfully");
	}
}
#endif //__linux__

void set_local_start_time() {
	// Linux specific code {{{
	#ifdef __linux__
	if (GOT_LOCAL_TIME) {
		return;
	}
	GOT_LOCAL_TIME = true;

    dirent**							file_name_list;
	int32_t							num_files;
	ostringstream						ss;
	chrono::system_clock::time_point	now;
	time_t								now_t;

	//path	= new char[PATH_MAX];
	now		= chrono::system_clock::now();
	now_t	= chrono::system_clock::to_time_t(now);

	ss << put_time(localtime(&now_t), "_%F_%H.%M.%S");
	LOG_FILE += ss.str();

	num_files = scandir(LOG_DIR, &file_name_list, NULL, alphasort);
	if (num_files < 2) {
		cout << "logging is disabled: " << to_string(errno) << endl;
		return;
	}

	//don't need to account for directories '.' and '..'
	free(file_name_list[0]);
	free(file_name_list[1]);

	for (int i = 2; i < num_files; i++) {
		if (i < num_files - MAX_LOGS) {
			unlink(
			string(string(LOG_DIR) + "/" + file_name_list[i]->d_name).c_str());
		}
		free(file_name_list[i]);
	}
	free(file_name_list);

	Check_Logs();

	return;
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	if (GOT_LOCAL_TIME) return;
	GOT_LOCAL_TIME = true;

	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	ostringstream ss;
	ss << put_time(localtime(&now_t), "_%F_%H.%M.%S");
	LOG_FILE += ss.str();
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}
}

void Write_Message(
string type, string message, string file, uint32_t line, string func) {
	// Linux specific code {{{
	#ifdef __linux__
	set_local_start_time();
	ofstream out(LOG_FILE, ofstream::out | ofstream::app);
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	int thread = syscall(__NR_gettid);
	out << type << "[" << put_time(localtime(&now_t), "%T thr") <<
		thread << " \"" << file << "\":" << line << " " << func <<
		"()] " << message << endl;
	out.close();
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	set_local_start_time();
	ofstream out(LOG_FILE, ofstream::out | ofstream::app);
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	out << type << "[" << put_time(localtime(&now_t), "%T thr") << " \"" <<
	file << "\":" << line << " " << func << "()] " << message << endl;

	out.close();
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}
}
