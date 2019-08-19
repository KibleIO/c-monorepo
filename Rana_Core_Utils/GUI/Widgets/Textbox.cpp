#include "Textbox.h"

/*
TEXTBOX_STYLE
*/
TEXTBOX_STYLE Textbox_Style(
struct nk_style_edit active, struct nk_style_edit ghosting,
struct nk_style_edit inactive) {
	TEXTBOX_STYLE style;
	style.active = active;
	style.ghosting = ghosting;
	style.inactive = inactive;
	return style;
}

TEXTBOX_STYLE Textbox_Style_Inactive(
struct nk_style_edit active, struct nk_style_edit inactive) {
	return Textbox_Style(active, active, inactive);
}

TEXTBOX_STYLE Textbox_Style_Ghosting(
struct nk_style_edit active, struct nk_style_edit ghosting) {
	return Textbox_Style(active, ghosting, active);
}

TEXTBOX_STYLE Textbox_Style(struct nk_style_edit all) {
	return Textbox_Style(all, all, all);
}

/*
TEXTBOX
*/

void Initialize_Textbox(
TEXTBOX* textbox, TEXTBOX_STYLE style, uint32_t min, uint32_t max) {
	if (min > max) {
		uint32_t temp = min;
		min = max;
		max = temp;
	}

	textbox->inputBuffer = new char[max];
	textbox->currentInputLength = 0;
	textbox->style = style;
	textbox->minValidInput = min;
	textbox->maxValidInput = max;

	textbox->ghostTextActive = false;
	textbox->unfocusTextbox = false;
	textbox->focusTextbox = true;
}

bool Render_Textbox(
TEXTBOX* textbox, struct nk_context* ctx, TEXTBOX_ARGS args) {
	// Setup input filter and textbox style based on textbox interactability
	nk_plugin_filter using_filter =  Render_Textbox_Interactive_Setup(
	textbox, ctx, args.interactable, args.inputFilter);

	// Render the textbox
	nk_flags edit_flags = Render_Textbox_Main_Setup(
	textbox, ctx, args.concealed, args.concealChar, using_filter);

	// Check to see if ghost text needs to be activated
	Check_Ghost_Flags(textbox, edit_flags, args.ghostText);

	return args.interactable && edit_flags == NK_EDIT_COMMITED + NK_EDIT_ACTIVE
	&& Input_Valid(textbox);
}

bool Render_Textbox_With_Buffer(
TEXTBOX* textbox, struct nk_context* ctx, TEXTBOX_ARGS args) {
	nk_label(ctx, "", 0);
	return Render_Textbox(textbox, ctx, args);
}

void Delete_Textbox(TEXTBOX* textbox) {
	delete [] textbox->inputBuffer;
}

// ACCESSORS
bool Input_Valid(const TEXTBOX* textbox) {
	bool inputInRange =
	(uint32_t)textbox->currentInputLength >= textbox->minValidInput &&
	(uint32_t)textbox->currentInputLength <= textbox->maxValidInput;
	// Input valid if it is in range and the current input is not the ghost text
	return inputInRange && !textbox->ghostTextActive;
}

string Input_String(const TEXTBOX* textbox) {
	return string(textbox->inputBuffer, textbox->currentInputLength);
}

// MUTATORS
void Reset_Input_Buffer(TEXTBOX* textbox) {
	fill(
	textbox->inputBuffer, textbox->inputBuffer + textbox->maxValidInput, '\0');
	textbox->currentInputLength = 0;
}

void Reset_Textbox(TEXTBOX* textbox, const char* ghost_text) {
	if(ghost_text) {
		Enable_Ghost_Text(textbox, ghost_text);
	}
	else {
		Reset_Input_Buffer(textbox);
	}
}

void Enable_Ghost_Text(TEXTBOX* textbox, const char* ghost_text) {
	if (ghost_text) {
		Set_Input_Buffer(textbox, ghost_text);
		textbox->ghostTextActive = true;
	}
}

void Disable_Ghost_Text(TEXTBOX* textbox) {
	Reset_Input_Buffer(textbox);
	textbox->unfocusTextbox = true;
	textbox->ghostTextActive = false;
}

void Set_Input_Buffer(TEXTBOX* textbox, const char* buffer) {
	Set_Input_Buffer(textbox, buffer, strlen(buffer));
}

void Set_Input_Buffer(TEXTBOX* textbox, const char* buffer, int bufferLen) {
	if(bufferLen <= DEFAULT_TEXTBOX_MAX_INPUT) {
		copy(buffer, buffer + bufferLen, textbox->inputBuffer);
		textbox->currentInputLength = bufferLen;
	}
}

// RENDER HELPERS
nk_plugin_filter Render_Textbox_Interactive_Setup(
TEXTBOX* textbox, struct nk_context* ctx, bool interactable,
nk_plugin_filter default_filter) {
	// Set default style and filter if textbox is interactable
	if(interactable) {
		// Set style to ghosting or normal active,
		// depending on whether ghost text is currently active
		if(textbox->ghostTextActive) {
			ctx->style.edit = textbox->style.ghosting;
		}
		else {
			ctx->style.edit = textbox->style.active;
		}
		return default_filter;
	}
	// Use disabled style and filter out all input
	// if textbox is not interactable
	else {
		ctx->style.edit = textbox->style.inactive;
		return Full_Block_Filter;
	}
}

nk_flags Render_Textbox_Main_Setup(TEXTBOX* textbox, struct nk_context* ctx,
bool concealed, char conceal_char, nk_plugin_filter input_filter) {
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
		if(concealed && !textbox->ghostTextActive) {
			return nk_edit_string_concealed(
			ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, textbox->inputBuffer,
			&textbox->currentInputLength, textbox->maxValidInput,
			conceal_char, input_filter);
		}
		// If input is not being concealed or is displaying ghost text,
		// render the textbox normally
		else {
			return nk_edit_string(
			ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, textbox->inputBuffer,
			&textbox->currentInputLength, textbox->maxValidInput,
			input_filter);
		}
	}
}

void Render_False_Textbox(TEXTBOX* textbox, struct nk_context* ctx) {
	struct nk_vec2 textbox_position = nk_widget_position(ctx);
	struct nk_vec2 textbox_size = nk_widget_size(ctx);
	struct nk_rect textbox_rect = nk_recta(
	textbox_position, textbox_size);
	nk_fill_rect(
	nk_window_get_canvas(ctx),
	textbox_rect, textbox->style.active.rounding,
	textbox->style.active.normal.data.color);
}

void Check_Ghost_Flags(
TEXTBOX* textbox, nk_flags edit_flags, const char* ghost_text) {
	// If the textbox is not focused and there is no input currently,
	// enable the ghosting text
	if(edit_flags == NK_EDIT_INACTIVE + NK_EDIT_DEACTIVATED &&
	textbox->currentInputLength == 0 && ghost_text) {
		Enable_Ghost_Text(textbox, ghost_text);
	}
	// If the textbox was activated and it is still ghosting,
	// disable the ghosting text
	else if(edit_flags == NK_EDIT_ACTIVE + NK_EDIT_ACTIVATED &&
	textbox->ghostTextActive) {
		Disable_Ghost_Text(textbox);
	}
}

// Always returns false
int Full_Block_Filter(const struct nk_text_edit* edit, nk_rune rune) {
	// Bypass compiler warnings
	(void)edit;
	(void)rune;
	return nk_false;
}
