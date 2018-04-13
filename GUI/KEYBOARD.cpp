#include "KEYBOARD.h"

void         Initialize_Keyboard(KEYBOARD* keyboard, string path)                                                                         {
	keyboard->Shift        = false;
	keyboard->Caps_Lock    = false;
	keyboard->Device       = Construct_Device((char*) path.c_str());
	keyboard->Keys         = (char*)(const char[]){
		0, 0,
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
		0, 0,
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
		0, 0, 0, 0,
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
		'\'', '`', 0,
		'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
		0,
		0,
		0, ' ', 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0,
		0,
		0, 0, 0,
		0,
		0, 0, 0, 0,
		0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0
	};
	keyboard->Keys_Shifted = (char*)(const char[]){
		0, 0,
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
		0, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'{', '}', 0, 0,
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
		'"', '~', 0,
		'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
		0,
		0,
		0, ' ', 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0,
		0,
		0, 0, 0,
		0,
		0, 0, 0, 0,
		0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0
	};
}

KEYBOARD*    Construct_Keyboard (string path)                                                                                           {
	KEYBOARD* keyboard = new KEYBOARD();
	Initialize_Keyboard(keyboard, path);
	return keyboard;
}

void         Delete_Keyboard    (KEYBOARD* keyboard)                                                                         {
	Delete_Device(keyboard->Device);
	delete keyboard;
}
