#include "Textbox.h"

/*
TEXTBOX_STYLE
*/
TEXTBOX_STYLE Textbox_Style(struct nk_style_edit active,
struct nk_style_edit ghosting, struct nk_style_edit inactive) {
	TEXTBOX_STYLE style;
	style.active = active;
	style.ghosting = ghosting;
	style.inactive = inactive;
	return style;
}

TEXTBOX_STYLE Textbox_Style(struct nk_style_edit all) {
	return Textbox_Style(all, all, all);
}

/*
GHOST_TEXT
*/

void Initialize_Ghost_Text(GHOST_TEXT* text, const char* buffer) {
	if(buffer != NULL) {
		text->buffer = new char[strlen(buffer) + 1];
		strcpy(text->buffer, buffer);
		text->bufferLen = strlen(buffer) + 1;
	}
	else {
		text->buffer = NULL;
		text->bufferLen = 0;
	}
	text->active = false;
}

void Delete_Ghost_Text(GHOST_TEXT* text) {
	if (text->buffer != NULL) {
		delete [] text->buffer;
	}
}

/*
TEXTBOX
*/

void Initialize_Textbox(TEXTBOX* textbox, struct nk_color background_color,
struct nk_color foreground_color, int max, int min) {
	Initialize_Textbox(textbox,
		background_color,
		foreground_color,
		background_color,
		foreground_color,
		max, min);
}

void Initialize_Textbox(TEXTBOX* textbox, struct nk_color background_color,
struct nk_color foreground_color, const char* ghost_text,
struct nk_color ghost_text_color, int max, int min) {
	Initialize_Textbox(textbox,
		background_color, foreground_color,
		background_color, foreground_color,
		ghost_text, ghost_text_color,
		max, min);
}

void Initialize_Textbox(TEXTBOX* textbox,
struct nk_color active_background_color,
struct nk_color active_foreground_color,
struct nk_color inactive_background_color,
struct nk_color inactive_foreground_color,
int max, int min) {
	Initialize_Textbox(textbox,
		active_background_color,
		active_foreground_color,
		inactive_background_color,
		inactive_foreground_color,
		nk_filter_default,
		max, min);
}

void Initialize_Textbox(TEXTBOX* textbox,
struct nk_color active_background_color,
struct nk_color active_foreground_color,
struct nk_color inactive_background_color,
struct nk_color inactive_foreground_color,
const char* ghost_text, struct nk_color ghost_text_color,
int max, int min) {
	Initialize_Textbox(textbox,
		active_background_color,
		active_foreground_color,
		inactive_background_color,
		inactive_foreground_color,
		ghost_text, ghost_text_color,
		nk_filter_default,	// Use default input filter
		max, min);
}


void Initialize_Textbox(TEXTBOX* textbox,
struct nk_color active_background_color,
struct nk_color active_foreground_color,
struct nk_color inactive_background_color,
struct nk_color inactive_foreground_color,
nk_plugin_filter filter,
int max, int min) {
	Initialize_Textbox(textbox,
		active_background_color,
		active_foreground_color,
		inactive_background_color,
		inactive_foreground_color,
		NULL, active_foreground_color,	// Ghost text and color
		filter,
		max, min);
}

void Initialize_Textbox(TEXTBOX* textbox,
struct nk_color active_background_color,
struct nk_color active_foreground_color,
struct nk_color inactive_background_color,
struct nk_color inactive_foreground_color,
const char* ghost_text, struct nk_color ghost_text_color,
nk_plugin_filter filter,
int max, int min) {
	Initialize_Textbox(textbox,
		active_background_color,
		active_foreground_color,
		inactive_background_color,
		inactive_foreground_color,
		ghost_text, ghost_text_color,
		filter, '*',	// Default conceal character is an asterisk
		max, min);
}

void Initialize_Textbox(TEXTBOX* textbox,
struct nk_color active_background_color,
struct nk_color active_foreground_color,
struct nk_color inactive_background_color,
struct nk_color inactive_foreground_color,
const char* ghost_text, struct nk_color ghost_text_color,
nk_plugin_filter filter, char concealChar,
int max, int min) {
	textbox->minValidInput = min;
	textbox->maxValidInput = max;

	textbox->inputBuffer = new char[max];
	Reset_Input_Buffer(textbox);

	textbox->concealChar = concealChar;

	textbox->defaultFilter = filter;
	textbox->currentFilter = filter;

	// Setup the textbox style
	textbox->style = Textbox_Style(
		Nk_Edit_Style_Plain(
			active_background_color, active_foreground_color
		),
		Nk_Edit_Style_Plain(
			active_background_color, ghost_text_color
		),
		Nk_Edit_Style_Dead(
			inactive_background_color, inactive_foreground_color
		)
	);

	// Setup the ghost text
	textbox->ghostable = ghost_text != NULL;

	Initialize_Ghost_Text(&textbox->ghostText, ghost_text);
	Set_Ghosting(textbox, true);

	textbox->unfocusTextbox = false;
	textbox->focusTextbox = false;

	Initialize_Multicast_Function_Pointer(&textbox->inputCommittedEvent);
}

bool Render_Textbox(TEXTBOX* textbox, struct nk_context* ctx, bool concealed,
bool interactable) {
	// Setup input filter and textbox style based on textbox interactability
	Render_Textbox_Interactive_Setup(textbox, ctx, interactable);

	// Flags returned from textbox render
	nk_flags edit_flags = Render_Textbox_Main_Setup(
	textbox, ctx, concealed);

	// Check the flags to set or unset ghosting
	Check_Ghost_Flags(textbox, edit_flags);

	// If box is interactable, input committed, and input is valid,
	// invoke the input committed event
	if(interactable && edit_flags == NK_EDIT_COMMITED + NK_EDIT_ACTIVE &&
	Input_Valid(textbox)) {
		Invoke_All_Function_Pointers(&textbox->inputCommittedEvent, textbox);
	}

	return interactable && edit_flags == NK_EDIT_COMMITED + NK_EDIT_ACTIVE &&
		Input_Valid(textbox);
}

bool Render_Textbox_With_Buffer(
TEXTBOX* textbox, struct nk_context* ctx, bool concealed, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Textbox(textbox, ctx, concealed, interactable);
}

void Delete_Textbox(TEXTBOX* textbox) {
	delete [] textbox->inputBuffer;
	Delete_Ghost_Text(&textbox->ghostText);
	Delete_Multicast_Function_Pointer(&textbox->inputCommittedEvent);
}

// RENDER HELPERS
void Render_Textbox_Interactive_Setup(TEXTBOX* textbox, struct nk_context* ctx,
bool interactable) {
	// Set default style and filter if textbox is interactable
	if(interactable) {
		// Set style to ghosting or normal active,
		// depending on whether ghost text is currently active
		if(textbox->ghostText.active) {
			ctx->style.edit = textbox->style.ghosting;
		}
		else {
			ctx->style.edit = textbox->style.active;
		}
		textbox->currentFilter = textbox->defaultFilter;
	}
	// Use disabled style and filter out all input
	// if textbox is not interactable
	else {
		ctx->style.edit = textbox->style.inactive;
		textbox->currentFilter = Full_Block_Filter;
	}
}

nk_flags Render_Textbox_Main_Setup(TEXTBOX* textbox, struct nk_context* ctx,
bool concealed) {
	// If we're skipping this render frame,
	// render and focus textbox on the next frame
	if(textbox->unfocusTextbox) {
		Render_False_Textbox(textbox, ctx);
		textbox->unfocusTextbox = false;
		textbox->focusTextbox = true;
		return NK_EDIT_INACTIVE;
	}
	// If we're not skipping this render frame
	else {
		// If we're focusing the textbox, focus and skip focus next frame
		if(textbox->focusTextbox) {
			nk_edit_focus(ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER);
			textbox->focusTextbox = false;
		}

		// If the input is being concealed and is not displaying ghost text,
		// call the fuction with concealChar
		if(concealed && !textbox->ghostText.active) {
			return nk_edit_string_concealed(ctx,
				NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, textbox->inputBuffer,
				&textbox->currentInputLength, textbox->maxValidInput,
				textbox->concealChar, textbox->currentFilter);
		}
		// If input is not being concealed or is displaying ghost text,
		// render the textbox normally
		else {
			return nk_edit_string(ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER,
				textbox->inputBuffer, &textbox->currentInputLength,
				textbox->maxValidInput, textbox->currentFilter);
		}
	}
}

bool Input_Valid(const TEXTBOX* textbox) {
	bool inputInRange = textbox->currentInputLength >= textbox->minValidInput &&
		textbox->currentInputLength <= textbox->maxValidInput;
	// Input valid if it is in range and the current input is not the ghost text
	return inputInRange && !textbox->ghostText.active;
}

string Input_String(const TEXTBOX* textbox) {
	return string(textbox->inputBuffer, textbox->currentInputLength);
}

void Reset_Input_Buffer(TEXTBOX* textbox) {
	fill(
	textbox->inputBuffer, textbox->inputBuffer + textbox->maxValidInput, '0');
	textbox->currentInputLength = 0;
}

void Reset_Textbox(TEXTBOX* textbox) {
	if(textbox->ghostable) {
		Set_Ghosting(textbox, true);
	}
	else {
		Reset_Input_Buffer(textbox);
	}
}

void Set_Input_Buffer(TEXTBOX* textbox, const char* buffer) {
	int bufferLen = strlen(buffer);
	Set_Input_Buffer(textbox, buffer, bufferLen);
}

void Set_Input_Buffer(TEXTBOX* textbox, const char* buffer, int bufferLen) {
	if(bufferLen <= DEFAULT_TEXTBOX_MAX_INPUT) {
		copy(buffer, buffer + bufferLen, textbox->inputBuffer);
		textbox->currentInputLength = bufferLen;
	}
}

void Set_Ghosting(TEXTBOX* textbox, bool active) {
	if(textbox->ghostable) {
		// Activate the ghost text
		textbox->ghostText.active = active;
		// Set or reset buffer if ghost text is activating/deactivating
		if(active) {
			Set_Input_Buffer(textbox, textbox->ghostText.buffer,
				textbox->ghostText.bufferLen);
		}
		else {
			Reset_Input_Buffer(textbox);
			textbox->unfocusTextbox = true;
		}
	}
}

void Check_Ghost_Flags(TEXTBOX* textbox, nk_flags edit_flags) {
	// If the textbox is not focused and there is no input currently,
	// set the ghosting text
	if(edit_flags == NK_EDIT_INACTIVE + NK_EDIT_DEACTIVATED &&
	textbox->currentInputLength == 0) {
		Set_Ghosting(textbox, true);
	}
	// If the textbox was activated and it is still ghosting,
	// disable the ghosting text
	else if(edit_flags == NK_EDIT_ACTIVE + NK_EDIT_ACTIVATED &&
	textbox->ghostText.active) {
		Set_Ghosting(textbox, false);
	}
}

void Render_False_Textbox(TEXTBOX* textbox, struct nk_context* ctx) {
	struct nk_vec2 textbox_position = nk_widget_position(ctx);
	struct nk_vec2 textbox_size = nk_widget_size(ctx);
	struct nk_rect textbox_rect = nk_rect(
		textbox_position.x, textbox_position.y,
		textbox_size.x, textbox_size.y
	);
	nk_fill_rect(
		nk_window_get_canvas(ctx),
		textbox_rect, textbox->style.active.rounding,
		textbox->style.active.normal.data.color
	);
}

// Always returns false
int Full_Block_Filter(const struct nk_text_edit* edit, nk_rune rune) {
	// Bypass compiler warnings
	(void)edit;
	(void)rune;
	return nk_false;
}
