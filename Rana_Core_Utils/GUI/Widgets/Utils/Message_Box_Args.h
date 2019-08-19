#ifndef MESSAGE_BOX_ARGS_H_
#define MESSAGE_BOX_ARGS_H_

#include <cstdint>
#include "../../NK_BASE.h"
#include "../../../Utilities/vararg_ext.h"

#define MAX_MESSAGE_BOX_BUTTONS 4
#define MESSAGE_BOX_NO_DEFAULT_BUTTON 	-1

struct MESSAGE_BOX_ARGS {
	struct nk_context* ctx;
	const char* title;
	struct nk_rect bounds;
	nk_flags flags;
	bool modal;
	const char* text;
	const char* informativeText;
	int8_t defaultButton;
	const char* buttonLabels[MAX_MESSAGE_BOX_BUTTONS];
	uint8_t totalButtons;
};

MESSAGE_BOX_ARGS Message_Box_Args_With_Default_And_Info(
struct nk_context*, const char*, struct nk_rect, nk_flags, bool, const char*,
const char*, int8_t, uint8_t totalButtons, /* const char* */...);
MESSAGE_BOX_ARGS Message_Box_Args_With_Default(
struct nk_context*, const char*, struct nk_rect, nk_flags, bool,
const char* text, int8_t, uint8_t totalButtons, /* const char* */...);
MESSAGE_BOX_ARGS Message_Box_Args_With_Info(
struct nk_context*, const char*, struct nk_rect, nk_flags, bool, const char*,
const char*, uint8_t totalButtons, /* const char* */...);
MESSAGE_BOX_ARGS Message_Box_Args(
struct nk_context*, const char*, struct nk_rect, nk_flags, bool, const char*,
uint8_t totalButtons, /* const char* */...);

void Initialize_MESSAGE_BOX_ARGS(
MESSAGE_BOX_ARGS*, struct nk_context*, const char*, struct nk_rect, nk_flags,
bool, const char*, const char*, int8_t, const char* button_labels[],
uint8_t totalButtons);

#endif
