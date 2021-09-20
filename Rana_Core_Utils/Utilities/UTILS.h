//PLATFORMS: Linux, Windows, OSX (TODO)

#ifndef  UTILS_H_
#define  UTILS_H_

#include <iostream>
#include <sys/stat.h>

using namespace std;

#define NULLIFY {}

string system_output(string);

uint8_t file_exists(string);

#endif
