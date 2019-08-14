#ifndef TEXTBOX_ARGS_H_
#define TEXTBOX_ARGS_H_

#include <cstdarg>
#include "../../NK_BASE.h"
#include "../../../Utilities/LOGGING.h"
#include "../../../Utilities/vararg_ext.h"

#define TEXTBOX_INTERACT_CODE 	'i'
#define TEXTBOX_CONCEAL_CODE 	'c'
#define TEXTBOX_GHOST_CODE		'g'
#define TEXTBOX_FILTER_CODE		'f'

struct TEXTBOX_ARGS {
	// Specify for code 'i'
	bool interactable;

	// Specify for code 'c'
	bool concealed;
	char concealChar;

	// Specify for code 'g'
	const char* ghostText;

	// Specify for code 'f'
	nk_plugin_filter inputFilter;
};

TEXTBOX_ARGS Textbox_Args_Code(
const char*, uint8_t num_args, /* encoded args */...);
TEXTBOX_ARGS Textbox_Args(
bool interactable, bool concealed, char conceal_char,
const char* ghost_text, nk_plugin_filter input_filter);

void Decode_Textbox_Arg(TEXTBOX_ARGS* args, char code, va_list varargs);

// CONSTANTS
static const TEXTBOX_ARGS TEXTBOX_DEFAULT_ARGS = Textbox_Args(
true, false, '*', NULL, nk_filter_default);

#endif
