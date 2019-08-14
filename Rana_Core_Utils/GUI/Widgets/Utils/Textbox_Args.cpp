#include "Textbox_Args.h"

TEXTBOX_ARGS Textbox_Args_Code(
const char* code, uint8_t num_args, /* encoded args */...) {
	TEXTBOX_ARGS args = TEXTBOX_DEFAULT_ARGS;
	va_character_decoder(code, num_args, &args, Decode_Textbox_Arg);
	return args;
}

TEXTBOX_ARGS Textbox_Args(
bool interactable, bool concealed, char conceal_char, const char* ghost_text,
nk_plugin_filter input_filter) {
	TEXTBOX_ARGS args;
	args.interactable = interactable;
	args.concealed = concealed;
	args.concealChar = conceal_char;
	args.ghostText = ghost_text;
	args.inputFilter = input_filter;
	return args;
}

void Decode_Textbox_Arg(TEXTBOX_ARGS* args, char code, va_list varargs) {
	switch (code) {
		case TEXTBOX_INTERACT_CODE:
			args->interactable = va_arg(varargs, int);
			break;
		case TEXTBOX_CONCEAL_CODE:
			args->concealed = va_arg(varargs, int);
			args->concealChar = va_arg(varargs, int);
			break;
		case TEXTBOX_GHOST_CODE:
			args->ghostText = va_arg(varargs, const char*);
			break;
		case TEXTBOX_FILTER_CODE:
			args->inputFilter = va_arg(varargs, nk_plugin_filter);
			break;
		default:
			log_err(string("Character code ") + to_string(code) +
			" not recognized as a textbox argument initializer code");
			break;
	}
}
