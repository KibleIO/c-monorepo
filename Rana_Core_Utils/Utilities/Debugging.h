#ifndef DEBUGGING_H_
#define DEBUGGING_H_

#include <sstream>

struct DEBUGGING_MESSAGES {
	static std::ostringstream buffer;
};

void Output_Debug_Message_If(std::ostream&, bool);
void Output_Debug_Message(std::ostream&);
void Clear_Debug_Message();

#endif
