#include "LOGGING.h"

bool GOT_LOCAL_TIME = false;
string LOG_FILE = "logs/log";

int get_local_start_time() {
	if (GOT_LOCAL_TIME) return 0;
	GOT_LOCAL_TIME = true;
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	ostringstream ss;
	ss << put_time(localtime(&now_t), "_%F_%H.%M.%S");
	LOG_FILE += ss.str();
	return 0;
}

void Write_Message(string type, string message, const char* file, int line, const char* func) {
	get_local_start_time();
	ofstream out(LOG_FILE, ofstream::out | ofstream::app);
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_t = chrono::system_clock::to_time_t(now);
	out << type << "[" <<  put_time(localtime(&now_t), "%T thr") << syscall(__NR_gettid) << " \"" << file << "\":" << line << " " << func << "()] " << message << endl;
	out.close();
}
