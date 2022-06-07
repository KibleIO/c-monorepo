// PLATFORMS: Linux, Windows, OSX (TODO)

#include "UTILS.h"

string system_output(string cmd) {
    string data;
    FILE *stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

// Linux specific code {{{
#ifdef __linux__
    stream = popen(cmd.c_str(), "r");
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
    stream = _popen(cmd.c_str(), "r");
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
// TODO apple code
#endif
    // }}}

    if (stream) {
        while (!feof(stream)) {
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        }

// Linux specific code {{{
#ifdef __linux__
        pclose(stream);
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
        _pclose(stream);
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
// TODO apple code
#endif
        // }}}
    }
    return data;
}

string trim(const string &s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));

    return string(start, end + 1);
}

vector<string> split(const string &str, const string &delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

uint8_t file_exists(string file) {
    struct stat buffer;
    return (stat(file.c_str(), &buffer) == 0);
}

void generate_uuid(char *str) {
	#ifndef _WIN64

	unsigned char binuuid[16];
	uuid_generate_random(binuuid);

	uuid_unparse(binuuid, str);

	#else

	UUID uuid;
	UuidCreate(&uuid);
	char *str1;
	UuidToStringA(&uuid, (RPC_CSTR*)&str1);
	strcpy(str, str1);
	RpcStringFreeA((RPC_CSTR*)&str1);
	
	#endif

	
}

void get_mac_address(char *str) {
        #ifdef linux
    struct ifreq ifr;
    struct ifaddrs *ifap, *ifa;
    int success = 0;
    int retry = MAC_ADDRESS_RETRY;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        goto error_lbl;
    }

	if (getifaddrs(&ifap) < 0) {
		goto error_lbl;
	}

    do {
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
	    strcpy(ifr.ifr_name, ifa->ifa_name);
            if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
                // don't count loopback and devices

                if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
			    //we have failed to find a running interface,
			    //instead just choose the first one that isn't a
			    //loopback interface
			    if ((ifr.ifr_flags & IFF_RUNNING) || retry <= 1) {
				    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
		                        success = 1;
		                        break;
		                    }
			    }
                }
            }

        }
        if (!success) {
            Sleep_Milli(MAC_ADDRESS_RETRY_SLEEP);
        }
        retry--;
    } while (retry > 0 && !success);  // demorgan came knocking...

	freeifaddrs(ifap);

    if (success) {
        for (int i = 0; i < OCTETS_IN_MAC_ADDRESS; i++) {
            snprintf(str, DIGITS_IN_OCTET_IN_MAC_ADDRESS + 1, "%02x",
                     (unsigned char)ifr.ifr_hwaddr.sa_data[i]);

            str += DIGITS_IN_OCTET_IN_MAC_ADDRESS;
            if (i < OCTETS_IN_MAC_ADDRESS - 1) {
                strcat(str, ":");
                str += 1;
            }
        }

	//this is slightly confusing. should re-do eventually
        close(sock);
        return;
    }

error_lbl:
    close(sock);
    
    #endif
    strcpy(str, "FATAL");
}

#ifdef _WIN64

int gettimeofday(struct timeval* tp, struct timezone* tzp) {
	namespace sc = std::chrono;
	sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
	sc::seconds s = sc::duration_cast<sc::seconds>(d);
	tp->tv_sec = s.count();
	tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();

	return 0;
}

#endif

void get_current_time(char *str) {
	struct timespec ts;

	#ifdef __linux__

	timespec_get(&ts, TIME_UTC);

	char buff[100];
	strftime(buff, sizeof buff, "%Y-%m-%dT%T", gmtime(&ts.tv_sec));
	sprintf(str, "%s.%09ldZ", buff, ts.tv_nsec);

	#elif __APPLE__

	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts.tv_sec = mts.tv_sec;
	ts.tv_nsec = mts.tv_nsec;

	char buff[100];
	strftime(buff, sizeof buff, "%Y-%m-%dT%T", gmtime(&ts.tv_sec));
	sprintf(str, "%s.%09ldZ", buff, ts.tv_nsec);

	#elif _WIN64
	
	struct timeval tv;
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];

	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;
	nowtm = gmtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%dT%H:%M:%S", nowtm);
	snprintf(str, 64, "%s.%09ldZ", tmbuf, tv.tv_usec);
 
	#endif
}

void Sleep_Milli(unsigned int milli) {
        usleep(milli * 1000);
#ifdef __linux__
    usleep(milli * 1000);
#endif
#ifdef _WIN64
    Sleep(milli);
#endif
}

const char *itoa(const char *str, int input) {
    snprintf((char *)str, ITOA_STR_SIZE, "%d", input);
    return str;
}

bool Write_Bin_To_File(char *file_name, char *buffer, int size) {
	fstream file(file_name, ios::binary | ios::out);

	if (!file.is_open()) {
		return false;
	}

	file.write(buffer, size);
	file.close();

	return true;
}

bool Read_Bin_From_File(char *file_name, char *buffer, int size) {
	fstream file(file_name, ios::binary | ios::in);

	if (!file.is_open()) {
		return false;
	}

	file.read(buffer, size);
	file.close();

	return true;
}

bool Check_If_White_Space(char *str) {
	while (str[0] != '\0') {
		if (!isspace(str[0])) {
			return false;
		}
		str++;
	}
	return true;
}

bool Is_Connected_To_Internet() {
	return strcmp(Get_Str_From_Website(UPDATE_IP).c_str(),
		"nothing here... move along\n") == 0;
}

int Get_Number_Of_Cores() {
        #ifdef __linux__
	
        return get_nprocs();

        #else

        //god help us
        return 8;

        #endif
}