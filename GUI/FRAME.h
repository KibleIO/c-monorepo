#ifndef  FRAME_H_
#define  FRAME_H_

#include <fstream>
#include <unistd.h>

#include <Utilities/Difference.h>
#include <Utilities/DynArr.h>
#include "../UTILITIES/BlackBox.h"

struct BUFFER_ELEMENT : public NXTElement{
	char*        Buffer;
	unsigned int Length;  // In Bytes
};
struct FRAME {
	DynArr* Update_Stack; // Concurrent protected "BUFFER_ELEMENT" stack
	int*    Buffer;       // History Buffer
};
void   Initialize_Frame(FRAME*, unsigned int);
FRAME* Construct_Frame (unsigned int);
void   Delete_Frame    (FRAME*);
void   Damage_Frame    (FRAME*, char*, unsigned int);
int    Render_Frame    (FRAME*, char**);

#endif /* FRAME */