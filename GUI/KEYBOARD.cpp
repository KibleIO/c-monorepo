#include "KEYBOARD.h"
//#include "nuklear.h"

bool    KEYBOARD::Shift;
bool    KEYBOARD::Caps_Lock;
char* KEYBOARD::Keys         = (char*)(const char[]){
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
	keyboard->Shift        = false;
	keyboard->Caps_Lock    = false;
	keyboard->path = path;
	keyboard->fd = open(keyboard->path.c_str(), O_RDONLY);
	keyboard->Event_Status = event_status;
	if (keyboard->fd == -1) {
		log_err("invalid path for keyboard " + path);
		keyboard->Listening = false;
		keyboard->Event_Listener = NULL;
	} else {
		keyboard->Listening = true;
		keyboard->Event_Listener = new thread(Listen_Keyboard, keyboard);
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
			KEYBOARD_EVENT* k_event = new KEYBOARD_EVENT;
			k_event->code = event.code;
			k_event->value = event.value;
			keyboard->Events.push(k_event);
			Set_Event(keyboard->Event_Status);
		} 
	}
	log_dbg("done listening to device " + keyboard->path);
}

void Handle_Keyboard_X11(int display_ID, Queue<KEYBOARD_EVENT*>* events) {
	Display*   dpy = XOpenDisplay(string(string(":") + to_string(display_ID)).c_str());
	if (!dpy) {
		log_err("Could not open display :" + to_string(display_ID));
		return;
	}
	KeyCode modcode = 0;

  	for (int i = events->size(); i > 0; i--) {
  		KEYBOARD_EVENT* k_event;
		events->pop(k_event);

  		switch (k_event->code){
  		log_dbg("Key code " + to_string(k_event->code) + " received");
  			case KEY_ESC:
                  switch (k_event->value){
                          case 0: // Key Release
                                  KEYBOARD::Shift = false;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Escape), False, 0);
                                  XFlush(dpy);
                                  break;
                          case 1: // Key Press
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Escape), True, 0);
                                  XFlush(dpy);
                          case 2: // Auto Repeat
                                  KEYBOARD::Shift = true;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Escape), False, 0);
                                  XFlush(dpy);
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Escape), True, 0);
                                  XFlush(dpy);
                                  break;
                          default:
                                  //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                  break;
                  }
                  break;
  			case KEY_LEFTCTRL:
                  switch (k_event->value) {
                          case 0: // Key Release
                                  KEYBOARD::Shift = false;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_L), False, 0);
                                  XFlush(dpy);
                                  break;
                          case 1: // Key Press
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_L), True, 0);
                                  XFlush(dpy);
                          case 2: // Auto Repeat
                                  KEYBOARD::Shift = true;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_L), False, 0);
                                  XFlush(dpy);
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_L), True, 0);
                                  XFlush(dpy);
                                  break;
                          default:
                                  //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                  break;
                  }
                  break;
  			case KEY_RIGHTCTRL:
                  switch (k_event->value) {
                          case 0: // Key Release
                                  KEYBOARD::Shift = false;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_R), False, 0);
                                  XFlush(dpy);
                                  break;
                          case 1: // Key Press
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_R), True, 0);
                                  XFlush(dpy);
                          case 2: // Auto Repeat
                                  KEYBOARD::Shift = true;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_R), False, 0);
                                  XFlush(dpy);
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Control_R), True, 0);
                                  XFlush(dpy);
                                  break;
                          default:
                                  //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                  break;
                  }
                  break;
  			case KEY_LEFTSHIFT:
                  switch (k_event->value) {
                          case 0: // Key Release
                                  KEYBOARD::Shift = false;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0);
                                  XFlush(dpy);
                                  break;
                          case 1: // Key Press
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0);
                                  XFlush(dpy);
                          case 2: // Auto Repeat
                                  KEYBOARD::Shift = true;
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0);
                                  XFlush(dpy);
                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0);
                                  XFlush(dpy);
                                  break;
                          default:
                                  //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                  break;
                  }
                  break;
  			case KEY_RIGHTSHIFT:
  				//nk_input_key(gui->NK_Context, NK_KEY_SHIFT, k_event->value);
  				switch (k_event->value){
  					case 0: // Key Release
  						KEYBOARD::Shift = false;
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_R), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_R), True, 0);
                                                                  XFlush(dpy);
  					case 2: // Auto Repeat
  						KEYBOARD::Shift = true;
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_R), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Shift_R), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
  						break;
  				}
  				break;
  			case KEY_CAPSLOCK:
  				switch (k_event->value){
  					case 0: // Key Release
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Caps_Lock), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						KEYBOARD::Caps_Lock = !KEYBOARD::Caps_Lock;
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Caps_Lock), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Caps_Lock), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Caps_Lock), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_TAB:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Tab), False, 0);
                          XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Tab), True, 0);
                          XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Tab), False, 0);
                          XFlush(dpy);
                          XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Tab), True, 0);
                          XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_BACKSPACE:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_BackSpace), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_BackSpace), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_BackSpace), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_BackSpace), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_UP:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_UP, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Up), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_UP, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Up), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_UP, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_UP, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Up), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Up), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_DOWN:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_DOWN, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Down), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_DOWN, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Down), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_DOWN, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_DOWN, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Down), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Down), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_LEFT:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_LEFT, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Left), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_LEFT, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Left), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_LEFT, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_LEFT, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Left), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Left), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_RIGHT:
  				switch (k_event->value){
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Right), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Right), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Right), False, 0);
                                                                  XFlush(dpy);
                                                                  XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_KP_Right), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
  				break;
  			case KEY_ENTER:
  				switch (k_event->value) {
  					case 0: // Key Release
  						//nk_input_key(gui->NK_Context, NK_KEY_ENTER, 0);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Return), False, 0);
                                                                  XFlush(dpy);
  						break;
  					case 1: // Key Press
  						//nk_input_key(gui->NK_Context, NK_KEY_ENTER, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Return), True, 0);
                                                                  XFlush(dpy);
  						break;
  					case 2: // Auto Repeat
  						//nk_input_key(gui->NK_Context, NK_KEY_ENTER, 0);
  						//nk_input_key(gui->NK_Context, NK_KEY_ENTER, 1);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Return), False, 0);
                                                                  XFlush(dpy);
  						XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, XK_Return), True, 0);
                                                                  XFlush(dpy);
  						break;
  					default:
  						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(k_event->value) + " for type: " + to_string(k_event.type) + " and code: KEY_CAPSLOCK");
  						break;
  				}
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
