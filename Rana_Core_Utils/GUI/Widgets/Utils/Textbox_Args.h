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
	bool interactable;
	bool concealed;
	char concealChar;
	const char* ghostText;
	nk_plugin_filter inputFilter;
};

TEXTBOX_ARGS Textbox_Args_Code(
const char*, uint8_t num_args, /* encoded args */...);
TEXTBOX_ARGS Textbox_Args(
bool interactable = true, bool concealed = false, char conceal_char = '*',
const char* ghost_text = NULL,
nk_plugin_filter input_filter = nk_filter_default);

void Decode_Textbox_Arg(TEXTBOX_ARGS* args, char code, va_list varargs);

#endif
