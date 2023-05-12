#include "KEYBOARD_SERVICE.h"

bool Initialize_KEYBOARD_SERVICE(KEYBOARD_SERVICE *keyboard, KCONTEXT *ctx) {
	keyboard->ctx = ctx;
	keyboard->main_loop = NULL;
	keyboard->main_loop_running = false;

	return true;
}

void Main_Loop_KEYBOARD_SERVICE(KEYBOARD_SERVICE *keyboard) {
	KEYBOARD_EVENT_T buf;
	int size;

	while (keyboard->main_loop_running) {
		/*
		size = Receive_Unsafe_SERVER(keyboard->s, (char*)buf);

		if (size > 0) {
			Handle_Keyboard_X11_Single(&buf[C_ARRAY_QUEUE_SIZE - 1]);
		}
		*/

		if (Receive_SERVER(keyboard->s, (char*) &buf, sizeof(KEYBOARD_EVENT_T))) {
			Handle_Keyboard_X11_Single(&buf);
		}
	}
}

bool Connect_KEYBOARD_SERVICE(KEYBOARD_SERVICE *keyboard, SERVER *s) {
	keyboard->s = s;

	Open_Display_KEYBOARD();

	ASSERT_E_R(keyboard->s != NULL, "Server is NULL", keyboard->ctx);

	keyboard->main_loop_running = true;
	keyboard->main_loop = new thread(Main_Loop_KEYBOARD_SERVICE, keyboard);

	return true;
}

void Disconnect_KEYBOARD_SERVICE(KEYBOARD_SERVICE *keyboard) {
	keyboard->main_loop_running = false;
	if (keyboard->main_loop != NULL) {
		keyboard->main_loop->join();
		delete keyboard->main_loop;
		keyboard->main_loop = NULL;
	}
}

void Delete_KEYBOARD_SERVICE(KEYBOARD_SERVICE *keyboard) {

}
