#include "Message_Box_Args.h"

MESSAGE_BOX_ARGS Message_Box_Args_With_Default_And_Info(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, const char* text, const char* informative_text,
int8_t default_button, uint8_t total_buttons, /* const char* */...) {
	MESSAGE_BOX_ARGS args;
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);
	Initialize_MESSAGE_BOX_ARGS(
	&args, ctx, title, bounds, flags, modal, text, informative_text,
	default_button, button_labels, total_buttons);
	return args;
}

MESSAGE_BOX_ARGS Message_Box_Args_With_Default(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, const char* text,
int8_t default_button, uint8_t total_buttons, /* const char* */...) {
	MESSAGE_BOX_ARGS args;
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);
	Initialize_MESSAGE_BOX_ARGS(
	&args, ctx, title, bounds, flags, modal, text, NULL,
	default_button, button_labels, total_buttons);
	return args;
}

MESSAGE_BOX_ARGS Message_Box_Args_With_Info(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, const char* text, const char* informative_text,
uint8_t total_buttons, /* const char* */...) {
	MESSAGE_BOX_ARGS args;
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);
	Initialize_MESSAGE_BOX_ARGS(
	&args, ctx, title, bounds, flags, modal, text, informative_text,
	MESSAGE_BOX_NO_DEFAULT_BUTTON, button_labels, total_buttons);
	return args;
}

MESSAGE_BOX_ARGS Message_Box_Args(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, const char* text, uint8_t total_buttons,
/* const char* */...) {
	MESSAGE_BOX_ARGS args;
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);
	Initialize_MESSAGE_BOX_ARGS(
	&args, ctx, title, bounds, flags, modal, text, NULL,
	MESSAGE_BOX_NO_DEFAULT_BUTTON, button_labels, total_buttons);
	return args;
}

void Initialize_MESSAGE_BOX_ARGS(
MESSAGE_BOX_ARGS* args, struct nk_context* ctx, const char* title,
struct nk_rect bounds, nk_flags flags, bool modal, const char* text,
const char* informative_text, int8_t default_button,
const char* button_labels[], uint8_t total_buttons) {
	args->ctx = ctx;
	args->title = title;
	args->bounds = bounds;
	args->flags = flags;
	args->modal = modal;
	args->text = text;
	args->informativeText = informative_text;
	args->defaultButton = default_button;
	args->totalButtons = total_buttons;

	for (uint8_t i = 0; i < total_buttons; i++) {
		args->buttonLabels[i] = button_labels[i];
	}
}
