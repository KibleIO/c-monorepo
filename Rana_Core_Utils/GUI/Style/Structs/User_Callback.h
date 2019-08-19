#ifndef USER_CALLBACK_H_
#define USER_CALLBACK_H_

#include "../../NK_BASE.h"

struct USER_CALLBACK {
	nk_handle userdata;
	void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata);
	void(*draw_end)(struct nk_command_buffer*, nk_handle userdata);
};

USER_CALLBACK User_Callback_Empty();
USER_CALLBACK User_Callback(
nk_handle, void(*)(struct nk_command_buffer*, nk_handle userdata),
void(*)(struct nk_command_buffer*, nk_handle userdata));

#endif
