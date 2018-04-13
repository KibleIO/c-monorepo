#ifndef LOG_H_
#define LOG_H_

#define LOG_CONSOLE cout
#define LOG_FILE "Log.txt"

#include <stdlib.h> // exit, EXIT_FAILURE
#include <string>   // string
#include <sstream>  // std::ostringstream

#ifdef LOG_CONSOLE
#include <iostream>
#endif // LOG_CONSOLE

#ifdef LOG_FILE
#include <fstream>
#endif //  LOG_FILE

using namespace std;

void Log       (string message); // Private - do not call

void Error     (string message); // An unexpected, unhandled exception; execution is terminated
void Warning   (string message); // An unexpected, handled exception; execution continues but may be hindered
void Notice    (string message); // An expected and handled exception; execution continues
void Debugging (string message); // Generic output for debugging use; execution continues

#endif // LOG_H_
