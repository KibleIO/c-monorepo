#include "Debugging.h"
using namespace std;

ostringstream DEBUGGING_MESSAGES::buffer;

void Output_Debug_Message_If(ostream& out, bool condition) {
	if (condition) {
		Output_Debug_Message(out);
	}
}
void Output_Debug_Message(ostream& out) {
	out << DEBUGGING_MESSAGES::buffer.str();
}
void Clear_Debug_Message() {
	DEBUGGING_MESSAGES::buffer = ostringstream();
}
