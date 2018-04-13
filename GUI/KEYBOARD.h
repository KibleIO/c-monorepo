#ifndef  KEYBOARD_H_
#define  KEYBOARD_H_

#include "DEVICE.h"

struct       KEYBOARD {
	DEVICE* Device;
	bool    Shift;
	bool    Caps_Lock;
	char*   Keys;
	char*   Keys_Shifted;
};

void      Initialize_Keyboard(KEYBOARD*, string);
KEYBOARD* Construct_Keyboard (string);
void      Delete_Keyboard    (KEYBOARD*);

#endif