//PLATFORMS: Linux, Windows, OSX (TODO)

#include "KEYBOARD.h"

#ifdef __linux__
Display* KEYBOARD::dpy;
#endif
bool KEYBOARD::Shift;
bool KEYBOARD::Caps_Lock;

char keys_mem[] = {
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

// Linux specific code {{{
#ifdef __linux__
void Handle_Key(Display* dpy, KeySym key, int32_t value) {
	switch (value) {
		case 0:
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), False, 0);
			XFlush(dpy);
			break;
		case 1:
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), True, 0);
			XFlush(dpy);
			break;
		case 2:
			break;
		default:
			log_err(((const JSON_TYPE){
				{"message", "invalid key event value"},
				JSON_TYPE_END}));
			break;
	}
}

void Open_Display_KEYBOARD() {
	KEYBOARD::dpy = XOpenDisplay(":1");
}

void Handle_Keyboard_X11_Single(KEYBOARD_EVENT_T* k_event) {
	if (k_event->code < 0x20) { //please don't ask me why
		k_event->code |= 0xff00;
	}

	switch (k_event->code) {
		//case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ESCAPE):
		//	Handle_Key(KEYBOARD::dpy, XK_Escape, k_event->value);
		//	break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL):
			Handle_Key(KEYBOARD::dpy, XK_Control_L, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL):
			Handle_Key(KEYBOARD::dpy, XK_Control_R, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT):
			Handle_Key(KEYBOARD::dpy, XK_Shift_L, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT):
			Handle_Key(KEYBOARD::dpy, XK_Shift_R, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT):
			Handle_Key(KEYBOARD::dpy, XK_Alt_L, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT):
			Handle_Key(KEYBOARD::dpy, XK_Alt_R, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK):
			Handle_Key(KEYBOARD::dpy, XK_Caps_Lock, k_event->value);
			break;
		//case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_TAB):
		//	Handle_Key(KEYBOARD::dpy, XK_Tab, k_event->value);
		//	break;
		//case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BACKSPACE):
		//	Handle_Key(KEYBOARD::dpy, XK_BackSpace, k_event->value);
		//	break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP):
			Handle_Key(KEYBOARD::dpy, XK_KP_Up, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN):
			Handle_Key(KEYBOARD::dpy, XK_KP_Down, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT):
			Handle_Key(KEYBOARD::dpy, XK_KP_Left, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT):
			Handle_Key(KEYBOARD::dpy, XK_KP_Right, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RETURN):
			Handle_Key(KEYBOARD::dpy, XK_Return, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1):
			Handle_Key(KEYBOARD::dpy, XK_F1, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2):
			Handle_Key(KEYBOARD::dpy, XK_F2, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3):
			Handle_Key(KEYBOARD::dpy, XK_F3, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4):
			Handle_Key(KEYBOARD::dpy, XK_F4, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5):
			Handle_Key(KEYBOARD::dpy, XK_F5, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6):
			Handle_Key(KEYBOARD::dpy, XK_F6, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7):
			Handle_Key(KEYBOARD::dpy, XK_F7, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8):
			Handle_Key(KEYBOARD::dpy, XK_F8, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9):
			Handle_Key(KEYBOARD::dpy, XK_F9, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10):
			Handle_Key(KEYBOARD::dpy, XK_F10, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11):
			Handle_Key(KEYBOARD::dpy, XK_F11, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12):
			Handle_Key(KEYBOARD::dpy, XK_F12, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT):
			Handle_Key(KEYBOARD::dpy, XK_Insert, k_event->value);
			break;
		//case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DELETE):
		//	Handle_Key(KEYBOARD::dpy, XK_Delete, k_event->value);
		//	break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME):
			Handle_Key(KEYBOARD::dpy, XK_Home, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP):
			Handle_Key(KEYBOARD::dpy, XK_Page_Up, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN):
			Handle_Key(KEYBOARD::dpy, XK_Page_Down, k_event->value);
			break;
		case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END):
			Handle_Key(KEYBOARD::dpy, XK_End, k_event->value);
			break;
		default:
			Handle_Key(KEYBOARD::dpy, k_event->code, k_event->value);
			break;
	}

	XFlush(KEYBOARD::dpy);
}

void Handle_Keyboard_X11(int display_ID, Queue<KEYBOARD_EVENT_T*>* events) {
	if (!KEYBOARD::dpy) {
		log_err(((const JSON_TYPE){
			{"message", "Could not open display"},
			JSON_TYPE_END}));
		return;
	}
	KeyCode modcode = 0;

	for (int i = events->size(); i > 0; i--) {
		KEYBOARD_EVENT_T* k_event = NULL;
		events->pop(k_event);

		switch (k_event->code) {
			case KEY_ESC:
				Handle_Key(KEYBOARD::dpy, XK_Escape, k_event->value);
				break;
			case KEY_LEFTCTRL:
				Handle_Key(KEYBOARD::dpy, XK_Control_L, k_event->value);
				break;
			case KEY_RIGHTCTRL:
				Handle_Key(KEYBOARD::dpy, XK_Control_R, k_event->value);
				break;
			case KEY_LEFTSHIFT:
				Handle_Key(KEYBOARD::dpy, XK_Shift_L, k_event->value);
				break;
			case KEY_RIGHTSHIFT:
				Handle_Key(KEYBOARD::dpy, XK_Shift_R, k_event->value);
				break;
			case KEY_LEFTALT:
				Handle_Key(KEYBOARD::dpy, XK_Alt_L, k_event->value);
				break;
			case KEY_RIGHTALT:
				Handle_Key(KEYBOARD::dpy, XK_Alt_R, k_event->value);
				break;
			case KEY_CAPSLOCK:
				Handle_Key(KEYBOARD::dpy, XK_Caps_Lock, k_event->value);
				break;
			case KEY_TAB:
				Handle_Key(KEYBOARD::dpy, XK_Tab, k_event->value);
				break;
			case KEY_BACKSPACE:
				Handle_Key(KEYBOARD::dpy, XK_BackSpace, k_event->value);
				break;
			case KEY_UP:
				Handle_Key(KEYBOARD::dpy, XK_KP_Up, k_event->value);
				break;
			case KEY_DOWN:
				Handle_Key(KEYBOARD::dpy, XK_KP_Down, k_event->value);
				break;
			case KEY_LEFT:
				Handle_Key(KEYBOARD::dpy, XK_KP_Left, k_event->value);
				break;
			case KEY_RIGHT:
				Handle_Key(KEYBOARD::dpy, XK_KP_Right, k_event->value);
				break;
			case KEY_ENTER:
				Handle_Key(KEYBOARD::dpy, XK_Return, k_event->value);
				break;
			case KEY_F1:
				Handle_Key(KEYBOARD::dpy, XK_F1, k_event->value);
				break;
			case KEY_F2:
				Handle_Key(KEYBOARD::dpy, XK_F2, k_event->value);
				break;
			case KEY_F3:
				Handle_Key(KEYBOARD::dpy, XK_F3, k_event->value);
				break;
			case KEY_F4:
				Handle_Key(KEYBOARD::dpy, XK_F4, k_event->value);
				break;
			case KEY_F5:
				Handle_Key(KEYBOARD::dpy, XK_F5, k_event->value);
				break;
			case KEY_F6:
				Handle_Key(KEYBOARD::dpy, XK_F6, k_event->value);
				break;
			case KEY_F7:
				Handle_Key(KEYBOARD::dpy, XK_F7, k_event->value);
				break;
			case KEY_F8:
				Handle_Key(KEYBOARD::dpy, XK_F8, k_event->value);
				break;
			case KEY_F9:
				Handle_Key(KEYBOARD::dpy, XK_F9, k_event->value);
				break;
			case KEY_F10:
				Handle_Key(KEYBOARD::dpy, XK_F10, k_event->value);
				break;
			case KEY_F11:
				Handle_Key(KEYBOARD::dpy, XK_F11, k_event->value);
				break;
			case KEY_F12:
				Handle_Key(KEYBOARD::dpy, XK_F12, k_event->value);
				break;
			case KEY_INSERT:
				Handle_Key(KEYBOARD::dpy, XK_Insert, k_event->value);
				break;
			case KEY_DELETE:
				Handle_Key(KEYBOARD::dpy, XK_Delete, k_event->value);
				break;
			case KEY_HOME:
				Handle_Key(KEYBOARD::dpy, XK_Home, k_event->value);
				break;
			case KEY_PAGEUP:
				Handle_Key(KEYBOARD::dpy, XK_Page_Up, k_event->value);
				break;
			case KEY_PAGEDOWN:
				Handle_Key(KEYBOARD::dpy, XK_Page_Down, k_event->value);
				break;
			case KEY_END:
				Handle_Key(KEYBOARD::dpy, XK_End, k_event->value);
				break;
			default:
				if (k_event->code <= 111){
					switch (k_event->value){
						case 0:
							break;
						case 1: // Key Press
						case 2: // Auto Repeat
							char key_value;
							key_value = keys_mem[k_event->code];
							if (key_value > 0) {
								//cout << "heyyyy " << key_value << " " << int(key_value) << " < " << XK_less << " > " << XK_greater << endl;
								modcode = XKeysymToKeycode(KEYBOARD::dpy, int(key_value));
								//cout << int(modcode) << endl;
								XTestFakeKeyEvent(KEYBOARD::dpy, modcode, False, 0);
								XFlush(KEYBOARD::dpy);
								XTestFakeKeyEvent(KEYBOARD::dpy, modcode, True, 0);
								XFlush(KEYBOARD::dpy);
								XTestFakeKeyEvent(KEYBOARD::dpy, modcode, False, 0);
								XFlush(KEYBOARD::dpy);
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

	XFlush(KEYBOARD::dpy);
}
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Handle_Key(int key, int32_t value, bool shift = false) {
	INPUT ip;
	DWORD code;

	//Set up the INPUT structure
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.wVk = 0; //We're doing scan codes instead
	ip.ki.dwExtraInfo = 0;

	code = KEYEVENTF_SCANCODE;

	switch (key) {
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_RCONTROL: case VK_RMENU:
	case VK_LWIN: case VK_RWIN: case VK_APPS:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		code |= KEYEVENTF_EXTENDEDKEY;
		break;
	}

	//This let's you do a hardware scan instead of a virtual keypress
	ip.ki.dwFlags = code;
	ip.ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);  //Set a unicode character to use (A)

	switch (value) {
	case 0:
		if (shift) {
			KEYBOARD::Shift = false;
		}
		ip.ki.dwFlags |= KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));
		break;
	case 1:
		SendInput(1, &ip, sizeof(INPUT));
		if (!shift) {
			break;
		}
	case 2:
		if (shift) {
			KEYBOARD::Shift = true;
		}

		ip.ki.dwFlags |= KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));

		ip.ki.dwFlags = code;
		SendInput(1, &ip, sizeof(INPUT));
		break;
	default:
		log_err("invalid key event value");
		break;
	}
}

void Handle_Keyboard_WINAPI(int display_ID, Queue<KEYBOARD_EVENT_T*>* events) {
	//KeyCode modcode = 0;

	INPUT ip;

	//Set up the INPUT structure
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.wVk = 0; //We're doing scan codes instead
	ip.ki.dwExtraInfo = 0;

	for (int i = events->size(); i > 0; i--) {
		KEYBOARD_EVENT_T* k_event;
		events->pop(k_event);

		switch (k_event->code) {
			log_dbg("Key code " + to_string(k_event->code) + " received");
		case KEY_ESC:
			Handle_Key(VK_ESCAPE, k_event->value, true);
			break;
		case KEY_LEFTCTRL:
			Handle_Key(VK_LCONTROL, k_event->value, true);
			break;
		case KEY_RIGHTCTRL:
			Handle_Key(VK_RCONTROL, k_event->value, true);
			break;
		case KEY_LEFTSHIFT:
			Handle_Key(VK_LSHIFT, k_event->value, true);
			break;
		case KEY_RIGHTSHIFT:
			Handle_Key(VK_RSHIFT, k_event->value, true);
			break;
		case KEY_CAPSLOCK:
			Handle_Key(VK_CAPITAL, k_event->value);
			break;
		case KEY_TAB:
			Handle_Key(VK_TAB, k_event->value);
			break;
		case KEY_BACKSPACE:
			Handle_Key(VK_BACK, k_event->value);
			break;
		case KEY_UP:
			Handle_Key(VK_UP, k_event->value);
			break;
		case KEY_DOWN:
			Handle_Key(VK_DOWN, k_event->value);
			break;
		case KEY_LEFT:
			Handle_Key(VK_LEFT, k_event->value);
			break;
		case KEY_RIGHT:
			Handle_Key(VK_RIGHT, k_event->value);
			break;
		case KEY_ENTER:
			Handle_Key(VK_RETURN, k_event->value);
			break;
		default:
			if (k_event->code <= 111) {
				switch (k_event->value) {
				case 0:
					break;
				case 1: // Key Press
				case 2: // Auto Repeat
					char key_value;
					key_value = keys_mem[k_event->code];
					if (key_value > 0) {
						//This let's you do a hardware scan instead of a virtual keypress
						ip.ki.dwFlags = KEYEVENTF_SCANCODE;
						ip.ki.wScan = k_event->code;  //Set a unicode character to use (A)

						//Send the press
						SendInput(1, &ip, sizeof(INPUT));

						//Prepare a keyup event
						ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
						SendInput(1, &ip, sizeof(INPUT));
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
}

void Handle_Keyboard_WINAPI(KEYBOARD_EVENT_T* k_event) {
	INPUT ip;

	//Set up the INPUT structure
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.wVk = 0; //We're doing scan codes instead
	ip.ki.dwExtraInfo = 0;

	switch (k_event->code) {
		log_dbg("Key code " + to_string(k_event->code) + " received");
	case KEY_ESC:
		Handle_Key(VK_ESCAPE, k_event->value, true);
		break;
	case KEY_LEFTCTRL:
		Handle_Key(VK_LCONTROL, k_event->value, true);
		break;
	case KEY_RIGHTCTRL:
		Handle_Key(VK_RCONTROL, k_event->value, true);
		break;
	case KEY_LEFTSHIFT:
		Handle_Key(VK_LSHIFT, k_event->value, true);
		break;
	case KEY_RIGHTSHIFT:
		Handle_Key(VK_RSHIFT, k_event->value, true);
		break;
	case KEY_CAPSLOCK:
		Handle_Key(VK_CAPITAL, k_event->value);
		break;
	case KEY_TAB:
		Handle_Key(VK_TAB, k_event->value);
		break;
	case KEY_BACKSPACE:
		Handle_Key(VK_BACK, k_event->value);
		break;
	case KEY_UP:
		Handle_Key(VK_UP, k_event->value);
		break;
	case KEY_DOWN:
		Handle_Key(VK_DOWN, k_event->value);
		break;
	case KEY_LEFT:
		Handle_Key(VK_LEFT, k_event->value);
		break;
	case KEY_RIGHT:
		Handle_Key(VK_RIGHT, k_event->value);
		break;
	case KEY_ENTER:
		Handle_Key(VK_RETURN, k_event->value);
		break;
	default:
		if (k_event->code <= 111) {
			switch (k_event->value) {
			case 0:
				break;
			case 1: // Key Press
			case 2: // Auto Repeat
				char key_value;
				key_value = keys_mem[k_event->code];
				if (key_value > 0) {
					//This let's you do a hardware scan instead of a virtual keypress
					ip.ki.dwFlags = KEYEVENTF_SCANCODE;
					ip.ki.wScan = k_event->code;  //Set a unicode character to use (A)

					//Send the press
					SendInput(1, &ip, sizeof(INPUT));

					//Prepare a keyup event
					ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					SendInput(1, &ip, sizeof(INPUT));
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
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
//TODO apple code
#endif
// }}}
