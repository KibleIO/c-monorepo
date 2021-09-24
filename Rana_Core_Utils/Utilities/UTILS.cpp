//PLATFORMS: Linux, Windows, OSX (TODO)

#include "UTILS.h"

string system_output(string cmd) {
	string data;
	FILE * stream;
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
	//TODO apple code
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
		//TODO apple code
		#endif
		// }}}
	}
	return data;
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
	struct ifreq s;

        int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd < 0) {
		goto error_lbl;
	}

	strcpy(str, "");
	strcpy(s.ifr_name, NETWORK_CARD_NAME);
	if (ioctl(fd, SIOCGIFHWADDR, &s) == 0) {
		for (int i = 0; i < OCTETS_IN_MAC_ADDRESS; i++) {
			snprintf(str, DIGITS_IN_OCTET_IN_MAC_ADDRESS + 1,
				"%02x", (unsigned char) s.ifr_addr.sa_data[i]);
			str += DIGITS_IN_OCTET_IN_MAC_ADDRESS;
			if (i < OCTETS_IN_MAC_ADDRESS - 1) {
				strcat(str, ":");
				str += 1;
			}
		}
	} else {
		goto error_lbl;
	}
	close(fd);
	return;

	error_lbl:
	strcpy(str, "FATAL");
}
