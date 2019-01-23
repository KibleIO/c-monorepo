#include "KEYBOARD.h"
//#include "nuklear.h"

bool KEYBOARD::Shift;
bool KEYBOARD::Caps_Lock;
char* KEYBOARD::Keys = (char*)(const char[]){
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', 0, 0,
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
char* KEYBOARD::Keys_Shifted = (char*)(const char[]){
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

void Initialize_Keyboard(KEYBOARD* keyboard, string path, EVENT* event_status) {
	keyboard->Shift =			false;
	keyboard->Caps_Lock = 		false;
	keyboard->path =			path;
	keyboard->fd = 				open(keyboard->path.c_str(), O_RDONLY);
	keyboard->Event_Status =	event_status;

	if (keyboard->fd == -1) {
		log_err("invalid path for keyboard " + path);
		keyboard->Listening =		false;
		keyboard->Event_Listener =	NULL;
	} else {
		keyboard->Listening = 		true;
		keyboard->Event_Listener = 	new thread(Listen_Keyboard, keyboard);
	}

	log_dbg("Keyboard " + path + " initialized");
}

KEYBOARD* Construct_Keyboard(string path, EVENT* event_status) {
	log_dbg("Keyboard " + path + " created");
	KEYBOARD* keyboard = new KEYBOARD();
	Initialize_Keyboard(keyboard, path, event_status);
	return keyboard;
}

void Delete_Keyboard(KEYBOARD* keyboard) {
	log_dbg("Keyboard " + keyboard->path + " deleted");
	keyboard->Listening = false;
	if (keyboard->Event_Listener) {
		keyboard->Event_Listener->join();
		delete keyboard->Event_Listener;
	}
	close(keyboard->fd);
	while (keyboard->Events.size() > 0) {
		KEYBOARD_EVENT* k_event;
		keyboard->Events.pop(k_event);
		delete k_event;
	}
	delete keyboard;
	keyboard = NULL;
}

void Listen_Keyboard(KEYBOARD* keyboard) {
	log_dbg("Beginning to listen to keyboard " + keyboard->path);
	while (keyboard->Listening) {
		input_event event;
		if (read(keyboard->fd, &event, sizeof(input_event)) != -1 &&
			event.type == EV_KEY && keyboard->Listening) {
			KEYBOARD_EVENT* k_event = 	new KEYBOARD_EVENT;
			k_event->code = 			event.code;
			k_event->value = 			event.value;
			keyboard->Events.push(k_event);
			Set_Event(keyboard->Event_Status);
		} 
	}
	log_dbg("done listening to device " + keyboard->path);
}

void Handle_Key(Display* dpy, KeySym key, int32_t value, bool shift = false) {
	switch (value) {
		case 0:
			if (shift) {
				KEYBOARD::Shift = false;
			}
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), False, 0);
			XFlush(dpy);
			break;
		case 1:
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), True, 0);
			XFlush(dpy);
			if (!shift) {
				break;
			}
		case 2:
			if (shift) {
				KEYBOARD::Shift = true;
			}
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), False, 0);
			XFlush(dpy);
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), True, 0);
			XFlush(dpy);
			break;
		default:
			log_err("invalid key event value");
			break;
	}
}

void Handle_Keyboard_X11(int display_ID, Queue<KEYBOARD_EVENT*>* events) {
	Display* dpy = XOpenDisplay(string(string(":") + to_string(display_ID)).c_str());
	if (!dpy) {
		log_err("Could not open display :" + to_string(display_ID));
		return;
	}
	KeyCode modcode = 0;

	for (int i = events->size(); i > 0; i--) {
		KEYBOARD_EVENT* k_event;
		events->pop(k_event);

		switch (k_event->code) {
		log_dbg("Key code " + to_string(k_event->code) + " received");
			case KEY_ESC:
				Handle_Key(dpy, XK_Escape, k_event->value, true);
				break;
			case KEY_LEFTCTRL:
				Handle_Key(dpy, XK_Control_L, k_event->value, true);
				break;
			case KEY_RIGHTCTRL:
				Handle_Key(dpy, XK_Control_R, k_event->value, true);
				break;
			case KEY_LEFTSHIFT:
				Handle_Key(dpy, XK_Shift_L, k_event->value, true);
				break;
			case KEY_RIGHTSHIFT:
				Handle_Key(dpy, XK_Shift_R, k_event->value, true);
				break;
			case KEY_CAPSLOCK:
				Handle_Key(dpy, XK_Caps_Lock, k_event->value);
				break;
			case KEY_TAB:
				Handle_Key(dpy, XK_Tab, k_event->value);
				break;
			case KEY_BACKSPACE:
				Handle_Key(dpy, XK_BackSpace, k_event->value);
				break;
			case KEY_UP:
				Handle_Key(dpy, XK_KP_Up, k_event->value);
				break;
			case KEY_DOWN:
				Handle_Key(dpy, XK_KP_Down, k_event->value);
				break;
			case KEY_LEFT:
				Handle_Key(dpy, XK_KP_Left, k_event->value);
				break;
			case KEY_RIGHT:
				Handle_Key(dpy, XK_KP_Right, k_event->value);
				break;
			case KEY_ENTER:
				Handle_Key(dpy, XK_Return, k_event->value);
				break;
			default:
				if (k_event->code <= 111){
					switch (k_event->value){
						case 0:
							break;
						case 1: // Key Press
						case 2: // Auto Repeat
							char key_value;
							key_value = KEYBOARD::Keys[k_event->code];
							if (key_value > 0) {
								//cout << "heyyyy " << key_value << " " << int(key_value) << " < " << XK_less << " > " << XK_greater << endl;
								modcode = XKeysymToKeycode(dpy, int(key_value));
								//cout << int(modcode) << endl;
								XTestFakeKeyEvent(dpy, modcode, False, 0);
								XFlush(dpy);
								XTestFakeKeyEvent(dpy, modcode, True, 0);
								XFlush(dpy);
								XTestFakeKeyEvent(dpy, modcode, False, 0);
								XFlush(dpy);
								//nk_input_char(gui->NK_Context, key_value);
							}
							break;
						default:
							//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
							break;
					}
				}
				break;
		}
		delete k_event;
	}

	XFlush(dpy);
	XCloseDisplay(dpy);
}
