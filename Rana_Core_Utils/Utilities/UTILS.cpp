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
