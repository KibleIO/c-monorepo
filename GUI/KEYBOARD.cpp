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
	keyboard->Device       = Construct_Device((char*) path.c_str(), event_status);
	log_dbg("Keyboard " + path + " initialized");
}

KEYBOARD*    Construct_Keyboard (string path, EVENT* event_status)                                                                                           {
	log_dbg("Keyboard " + path + " created");
	KEYBOARD* keyboard = new KEYBOARD();
	Initialize_Keyboard(keyboard, path, event_status);
	return keyboard;
}

void         Delete_Keyboard    (KEYBOARD* keyboard)                                                                         {
	log_dbg("Keyboard " + string(keyboard->Device->Path) + " deleted");
	Delete_Device(keyboard->Device);
	delete keyboard;
	keyboard = NULL;
}

void Handle_Keyboard_X11(int display_ID, Queue<EVENT_ELEMENT*>* events) {
	Display*   dpy = XOpenDisplay(string(string(":") + to_string(display_ID)).c_str());
	if (!dpy) {
		log_err("Could not open display :" + to_string(display_ID));
		return;
	}
	KeyCode modcode = 0;

  	for (int i = events->size(); i > 0; i--) {
  		EVENT_ELEMENT* element;
		events->pop(element);

  		switch (element->Event.type){
  			case EV_SYN:
  			case EV_REL:
  				break;
  			case EV_KEY:
  				switch (element->Event.code){
  				log_dbg("Key code " + to_string(element->Event.code) + " received");
  					case KEY_ESC:
                          switch (element->Event.value){
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
                                          //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                          break;
                          }
                          break;
  					case KEY_LEFTCTRL:
                          switch (element->Event.value) {
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
                                          //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                          break;
                          }
                          break;
  					case KEY_RIGHTCTRL:
                          switch (element->Event.value) {
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
                                          //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                          break;
                          }
                          break;
  					case KEY_LEFTSHIFT:
                          switch (element->Event.value) {
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
                                          //Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
                                          break;
                          }
                          break;
  					case KEY_RIGHTSHIFT:
  						//nk_input_key(gui->NK_Context, NK_KEY_SHIFT, element->Event.value);
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
  								break;
  						}
  						break;
  					case KEY_CAPSLOCK:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_TAB:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_BACKSPACE:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_UP:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_DOWN:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_LEFT:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_RIGHT:
  						switch (element->Event.value){
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					case KEY_ENTER:
  						switch (element->Event.value) {
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
  								//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  								break;
  						}
  						break;
  					default:
  						if (element->Event.code <= 111){
  							switch (element->Event.value){
  								case 0:
  									break;
  								case 1: // Key Press
  								case 2: // Auto Repeat
  									char key_value;
  									key_value = KEYBOARD::Keys[element->Event.code];
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
  									//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
  									break;
  							}
  						}
  						break;
  				}
  				break;
  			case EV_ABS:
  			case EV_MSC:
  			case EV_SW:
  			case EV_LED:
  			case EV_SND:
  			case EV_REP:
  			case EV_FF:
  			case EV_PWR:
  			case EV_FF_STATUS:
  			case EV_MAX:
  			case EV_CNT:
  				break;
  			default:
  				//Write_Notice(string("@Listen_Keyboard() Unexpected event type: ") + to_string(element->Event.type) + " (code: " + to_string(element->Event.code) + " and value: " + to_string(element->Event.value) + ")");
  				break;
  		}
  		delete element;
  	}

	XFlush(dpy);
  	XCloseDisplay(dpy);
}
