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
    uuid_t binuuid;
    uuid_generate_random(binuuid);

    uuid_unparse(binuuid, str);
}

void get_mac_address(char *str) {
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
    strcpy(str, "FATAL");
}

void get_current_time(char *str) {
	time_t rawtime;
	struct tm *timeinfo;

	time ( &rawtime );
	timeinfo = gmtime ( &rawtime );

	sprintf(str, "%d-%02d-%02dT%02d:%02d:%02dZ", timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
		timeinfo->tm_min, timeinfo->tm_sec);
}

void Sleep_Milli(unsigned int milli) {
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
