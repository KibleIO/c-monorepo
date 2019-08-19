#include "User_Callback.h"

USER_CALLBACK User_Callback_Empty() {
	return User_Callback(nk_handle_ptr(NULL), NULL, NULL);
}
USER_CALLBACK User_Callback(
nk_handle userdata,
void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata),
void(*draw_end)(struct nk_command_buffer*, nk_handle userdata)) {
	USER_CALLBACK callback;
	callback.userdata = userdata;
	callback.draw_begin = draw_begin;
	callback.draw_end = draw_end;
	return callback;
}
