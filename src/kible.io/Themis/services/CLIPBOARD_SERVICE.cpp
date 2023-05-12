#include "CLIPBOARD_SERVICE.h"

bool Initialize_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE *clipboard, KCONTEXT *ctx) {
	clipboard->ctx = ctx;
	clipboard->main_loop = NULL;
	clipboard->main_loop_running = false;

	return true;
}

void Main_Loop_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE *clipboard) {
	string value;
	char curr_clipboard_str[MAX_CLIPBOARD_SIZE];
	char temp_clipboard_str[MAX_CLIPBOARD_SIZE];

	//probably redundant
	memset(curr_clipboard_str, 0, MAX_CLIPBOARD_SIZE);
	memset(temp_clipboard_str, 0, MAX_CLIPBOARD_SIZE);

	clip::get_text(value);
	strncpy(curr_clipboard_str, value.c_str(), MAX_CLIPBOARD_SIZE - 1);

	while (clipboard->main_loop_running) {
		if (Receive_SERVER(clipboard->s, curr_clipboard_str, MAX_CLIPBOARD_SIZE)) {
			clip::set_text(curr_clipboard_str);
		} else {
			clip::get_text(value);
			strncpy(temp_clipboard_str, value.c_str(), MAX_CLIPBOARD_SIZE - 1);

			if (strcmp(curr_clipboard_str, temp_clipboard_str) != 0) {
				strcpy(curr_clipboard_str, temp_clipboard_str);

				if (!Send_SERVER(clipboard->s, curr_clipboard_str, MAX_CLIPBOARD_SIZE)) {
					LOG_WARN_CTX(clipboard->ctx) {
						ADD_STR_LOG("message", "For some reason I couldn't send the clipboard");
					}
				}
			}
		}
	}
}

bool Connect_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE *clipboard, SERVER *s) {
	clipboard->s = s;

	ASSERT_E_R(clipboard->s != NULL, "Server is NULL", clipboard->ctx);

	clipboard->main_loop_running = true;
	clipboard->main_loop = new thread(Main_Loop_CLIPBOARD_SERVICE, clipboard);

	return true;
}

void Disconnect_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE *clipboard) {
	clipboard->main_loop_running = false;
	if (clipboard->main_loop != NULL) {
		clipboard->main_loop->join();
		delete clipboard->main_loop;
		clipboard->main_loop = NULL;
	}
}

void Delete_CLIPBOARD_SERVICE(CLIPBOARD_SERVICE *clipboard) {

}
