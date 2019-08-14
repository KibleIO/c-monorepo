#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "Utils/Textbox_Args.h"
#include "../NK_BASE.h"
#include "../../Utilities/Function_Pointers.h"
#include "../Style.h"
#include "../Layout/nuklear_plus.h"

/*
TEXTBOX_STYLE
*/

struct TEXTBOX_STYLE {
	// Style while the textbox is accepting input
	struct nk_style_edit active;
	// Style while the textbox is displaying ghost text
	struct nk_style_edit ghosting;
	// Style while the textbox input is locked
	struct nk_style_edit inactive;
};

TEXTBOX_STYLE Textbox_Style(
struct nk_style_edit active, struct nk_style_edit ghosting,
struct nk_style_edit inactive);
TEXTBOX_STYLE Textbox_Style_Inactive(
struct nk_style_edit active, struct nk_style_edit inactive);
TEXTBOX_STYLE Textbox_Style_Ghosting(
struct nk_style_edit active, nk_style_edit ghosting);
TEXTBOX_STYLE Textbox_Style(struct nk_style_edit all);

/*
TEXTBOX
*/

#define DEFAULT_TEXTBOX_MIN_INPUT 8
#define DEFAULT_TEXTBOX_MAX_INPUT 64

struct TEXTBOX {
	char* inputBuffer;	// Holds the data typed in by the user
	int currentInputLength;	// Current length of the buffer input by user

	// Style of the textbox
	TEXTBOX_STYLE style;

	// Input is valid if the length of it is between these numbers
	uint32_t minValidInput;
	uint32_t maxValidInput;

	bool ghostTextActive;
	// True in the frame that the textbox loses focus
	// The textbox is not focused on the frame after ghost text disappears
	// to prevent text selection errors
	bool unfocusTextbox;
	// True in the frame the textbox is focused
	// Textbox is focused the frame after it is unfocused
	bool focusTextbox;

	// Event called when "enter" key is pressed while textbox is active
	// The event is only invoked if the input is valid, i.e. the length is
	// within the given limits
	// PASS: TEXTBOX*
	MULTICAST_FUNCTION_POINTER inputCommittedEvent;
};

void Initialize_Textbox(
TEXTBOX*, TEXTBOX_STYLE, uint32_t min = DEFAULT_TEXTBOX_MIN_INPUT,
uint32_t max = DEFAULT_TEXTBOX_MAX_INPUT);
// Render the textbox. Returns true if the input is valid, the enter button is
// pressed, and the textbox is interactable
bool Render_Textbox(
TEXTBOX*, struct nk_context*, TEXTBOX_ARGS = TEXTBOX_DEFAULT_ARGS);
bool Render_Textbox_With_Buffer(
TEXTBOX*, struct nk_context*, TEXTBOX_ARGS = TEXTBOX_DEFAULT_ARGS);
void Delete_Textbox(TEXTBOX*);

// ACCESSORS
bool Input_Valid(const TEXTBOX*);
string Input_String(const TEXTBOX*);

// MUTATORS
// Reset buffer by making all characters null-terminators
void Reset_Input_Buffer(TEXTBOX*);
// Reset the textbox
// If ghost text is given, this will be displayed,
// otherwise nothing will be displayed
void Reset_Textbox(TEXTBOX*, const char* ghost_text = NULL);
void Enable_Ghost_Text(TEXTBOX*, const char* ghost_text);
void Disable_Ghost_Text(TEXTBOX*);
// Set the input buffer from the given character array
void Set_Input_Buffer(TEXTBOX*, const char*);
void Set_Input_Buffer(TEXTBOX*, const char*, int bufferLen);

// RENDER HELPERS
nk_plugin_filter Render_Textbox_Interactive_Setup(
TEXTBOX*, struct nk_context*, bool interactable,
nk_plugin_filter default_filter);
nk_flags Render_Textbox_Main_Setup(
TEXTBOX*, struct nk_context*, bool concealed, char conceal_char,
nk_plugin_filter input_filter);
// Manually render a rect where the textbox should be with the same colors
void Render_False_Textbox(TEXTBOX*, struct nk_context*);
// Check the flags given to see if the textbox should
// display the given ghost text
void Check_Ghost_Flags(TEXTBOX*, nk_flags, const char* ghost_text);

// Used to block all input
int Full_Block_Filter(const struct nk_text_edit*, nk_rune);

#endif // TEXTBOX_H_
