#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "../Layout/nuklear_plus.h"
#include "../GUI.h"
#include "../../Utilities/Function_Pointers.h"
#include "../Style.h"

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
TEXTBOX_STYLE Textbox_Style(struct nk_style_edit all);

/*
GHOST_TEXT
*/

struct GHOST_TEXT {
	char* buffer;	// Characters in the ghost text
	int bufferLen;	// Length of the ghost text
	bool active;	// True if the textbox is currently using the ghost text
};

void Initialize_Ghost_Text(GHOST_TEXT*, const char*);
void Delete_Ghost_Text(GHOST_TEXT*);

/*
TEXTBOX
*/

#define DEFAULT_TEXTBOX_MIN_INPUT 8
#define DEFAULT_TEXTBOX_MAX_INPUT 64

struct TEXTBOX {
	char* inputBuffer;	// Holds the data typed in by the user
	int currentInputLength;	// Current length of the buffer input by user

	// Input is valid if the length of it is between these numbers
	int minValidInput;
	int maxValidInput;

	// Used to filter certain characters out of the input
	nk_plugin_filter defaultFilter;
	// Current filter being used to filter characters
	nk_plugin_filter currentFilter;

	// Style of the textbox
	TEXTBOX_STYLE style;

	// Character used to conceal the input if being concealed
	char concealChar;

	// Handles "ghost text" for the textbox
	// Ghost text appears if the textbox has no focus and there is no input
	// in the textbox, and dissappears as soon as the user clicks on it
	GHOST_TEXT ghostText;
	bool ghostable;	// True if the textbox should display ghost text
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

// Initialize textbox with all styles the same and default input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color background_color,
	struct nk_color foreground_color, int max = DEFAULT_TEXTBOX_MAX_INPUT,
	int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize the textbox with active style, no inactive style, ghost text
// and default input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color background_color,
	struct nk_color foreground_color,
	const char* ghost_text, struct nk_color ghost_text_color,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize textbox with active an inactive colors, no ghost text
// and the default input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color active_background_color,
	struct nk_color active_foreground_color,
	struct nk_color inactive_background_color,
	struct nk_color inactive_foreground_color,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize textbox with active colors, inactive colors, ghost text color
// and default input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color active_background_color,
	struct nk_color active_foreground_color,
	struct nk_color inactive_background_color,
	struct nk_color inactive_foreground_color,
	const char* ghost_text, struct nk_color ghost_text_color,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize textbox with active colors, inactive colors, no ghost text
// and unique input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color active_background_color,
	struct nk_color active_foreground_color,
	struct nk_color inactive_background_color,
	struct nk_color inactive_foreground_color,
	nk_plugin_filter filter,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize textbox with active colors, inactive colors, ghost text color
// color and unique input filter
void Initialize_Textbox(TEXTBOX*, struct nk_color active_background_color,
	struct nk_color active_foreground_color,
	struct nk_color inactive_background_color,
	struct nk_color inactive_foreground_color,
	const char* ghost_text, struct nk_color ghost_text_color,
	nk_plugin_filter filter,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);
// Initialize textbox with a unique conceal character
void Initialize_Textbox(TEXTBOX*, struct nk_color active_background_color,
	struct nk_color active_foreground_color,
	struct nk_color inactive_background_color,
	struct nk_color inactive_foreground_color,
	const char* ghost_text, struct nk_color ghost_text_color,
	nk_plugin_filter filter, char concealChar,
	int max = DEFAULT_TEXTBOX_MAX_INPUT, int min = DEFAULT_TEXTBOX_MIN_INPUT);

// Render the textbox. Returns true if the input is valid, the enter button is
// pressed, and the textbox is interactable
bool Render_Textbox(
TEXTBOX*, struct nk_context*, bool concealed = false, bool interactable = true);
bool Render_Textbox_With_Buffer(
TEXTBOX*, struct nk_context*, bool concealed = false, bool interactable = true);
void Delete_Textbox(TEXTBOX*);

// RENDER HELPERS
void Render_Textbox_Interactive_Setup(
TEXTBOX*, struct nk_context*, bool interactable);
nk_flags Render_Textbox_Main_Setup(
TEXTBOX*, struct nk_context*, bool concealed);

bool Input_Valid(const TEXTBOX*);
string Input_String(const TEXTBOX*);
// Reset buffer by making all characters null-terminators
void Reset_Input_Buffer(TEXTBOX*);

// Reset the textbox
// If the textbox has ghost text, this will be displayed,
// otherwise nothing will be displayed
void Reset_Textbox(TEXTBOX*);

// Set the input buffer from the given character array
void Set_Input_Buffer(TEXTBOX*, const char*);
void Set_Input_Buffer(TEXTBOX*, const char*, int bufferLen);

// Set the textbox to display the ghost text
void Set_Ghosting(TEXTBOX*, bool);
// Check the flags given to see if the textbox should
// start displaying the ghost text
void Check_Ghost_Flags(TEXTBOX*, nk_flags);
// Manually render a rect where the textbox should be with the same colors
void Render_False_Textbox(TEXTBOX*, struct nk_context*);

// Used to block all input
int Full_Block_Filter(const struct nk_text_edit*, nk_rune);

#endif // TEXTBOX_H_
