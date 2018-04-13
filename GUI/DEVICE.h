#ifndef  DEVICE_H_
#define  DEVICE_H_

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <thread>
#include <Utilities/DynArr.h>
#include "../UTILITIES/LOGGING.h"

struct  EVENT_ELEMENT : public NXTElement{
	input_event Event;
};

struct  DEVICE                           {
	int           File;
	char*         Path;
	DynArr        Event_Stack;
	volatile bool Listening;
	thread*       Event_Listener;
};

void    Initialize_Device(DEVICE*, char*);
DEVICE* Construct_Device (char*);
void    Delete_Device    (DEVICE*);
void    Listen_Device    (DEVICE*);

#endif