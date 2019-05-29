#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include "../GUI.h"
#include "Label.h"
#include "../Style.h"
#include "../../Utilities/Function_Pointers.h"

/*
BUTTON STYLE
*/

// Encapsulates the three possible styles for a button
struct BUTTON_STYLE {
	struct nk_style_button normal;
	struct nk_style_button toggled;
	struct nk_style_button dormant;
};

BUTTON_STYLE Button_Style(struct nk_style_button normal,
	struct nk_style_button toggled, struct nk_style_button dormant);
BUTTON_STYLE Button_Style_Dormant(struct nk_style_button normal,
	struct nk_style_button dormant);
BUTTON_STYLE Button_Style_Toggle(struct nk_style_button normal,
	struct nk_style_button toggled);
BUTTON_STYLE Button_Style_Normal(struct nk_style_button normal);

/*
BUTTON
*/

#define BUTTON_TYPE_PUSH 0
#define BUTTON_TYPE_TOGGLE 1

struct BUTTON {
	uint8_t type;	// Button type, either push or toggle
	// Flips to true-false each time the button is clicked
	// For a non-toggle button, the value is always false
	bool toggleState;
	// Style options for the button
	BUTTON_STYLE style;
	// Event invoked when the button is clicked
	// PASS: BUTTON*
	MULTICAST_FUNCTION_POINTER buttonClickedEvent;
};

void Initialize_Button(BUTTON*, uint8_t type, BUTTON_STYLE);

void Initialize_Toggle_Button(BUTTON*, BUTTON_STYLE);
void Initialize_Toggle_Button(
BUTTON*, struct nk_style_button normal, struct nk_style_button toggled,
struct nk_style_button dormant);
void Initialize_Toggle_Button(
BUTTON*, struct nk_style_button normal, struct nk_style_button toggled);

void Initialize_Push_Button(BUTTON*, BUTTON_STYLE);
void Initialize_Push_Button(
BUTTON*, struct nk_style_button normal, struct nk_style_button dormant);
void Initialize_Push_Button(BUTTON*, struct nk_style_button normal);

// Render the button with a label
// Return true if the button was clicked and is interactable
bool Render_Button_Label(BUTTON*, struct nk_context*, const char*,
	bool interactable = true);
bool Render_Button_Image(BUTTON*, struct nk_context*, struct nk_image,
	bool interactable = true);
bool Render_Button_Label_With_Buffer(BUTTON*, struct nk_context*, const char*,
	bool interactable = true);
bool Render_Button_Label_Buffered(BUTTON*, struct nk_context*, const char*,
	bool buffered, bool interactable = true);
void Delete_Button(BUTTON*);

void Set_Button_Toggle_State(BUTTON*, bool state);
void Toggle_Button_State(BUTTON*);

// BUTTON HELPERS
void Setup_Button_Style(BUTTON*, struct nk_context*, bool interactable);
bool Check_And_Run_Button_Clicked(BUTTON*, bool clicked, bool interactable);

/*
BUTTON PACKAGE
*/


// No buttons are toggled
#define BUTTON_PACKAGE_TYPE_PUSH 0
// Only one button can be toggled at a time
#define BUTTON_PACKAGE_TYPE_RADIO 1
// All buttons are toggle - any number can be selected at a time
#define BUTTON_PACKAGE_TYPE_SELECT 2
#define BUTTON_PACKAGE_DEFAULT_INITIAL_CAPACITY 8

struct BUTTON_PACKAGE {
	uint8_t type;	// Button package's type
	// Array of buttons in the package
	BUTTON* buttons;
	uint8_t totalButtons;
	uint8_t buttonCapacity;

	// If true, one button in the package must always be selected
	bool requireOneSelected;
	// Max number of buttons that can be selected
	uint8_t maxSelected;

	// Event invoked when one button within the package is clicked
	// PASS: BUTTON_PACKAGE*
	MULTICAST_FUNCTION_POINTER buttonClickedEvent;
	// Index of the current button being rendered
	int8_t currentButton;
};

// Initialize all buttons with the same style
void Initialize_Button_Package(BUTTON_PACKAGE*, uint8_t type, BUTTON_STYLE,
	uint8_t total_buttons, bool require_one_selected);
// Initialize all buttons with possibly unique styles
void Initialize_Button_Package(BUTTON_PACKAGE*, uint8_t type, BUTTON_STYLE*,
	uint8_t total_buttons, bool require_one_selected);

// Initialize all buttons as push buttons with the same style
void Initialize_Button_Package_Push(BUTTON_PACKAGE*,
	struct nk_style_button normal, struct nk_style_button dormant,
	uint8_t total_buttons);
// Initialize push buttons with unique styles
void Initialize_Button_Package_Push(BUTTON_PACKAGE*, BUTTON_STYLE*,
	uint8_t total_buttons);

// Intialize all buttons as toggle buttons selected one at a time
// All have same style
void Initialize_Button_Package_Radio(BUTTON_PACKAGE*,
	struct nk_style_button normal, struct nk_style_button toggled,
	struct nk_style_button dormant, uint8_t total_buttons,
	bool require_one_selected = false);
// Radio button package where all could have different styles
void Initialize_Button_Package_Radio(BUTTON_PACKAGE*, BUTTON_STYLE*,
	uint8_t total_buttons, bool require_one_selected = false);

// Initialize all buttons as toggle buttons where any number can be
// selected at a time.  All have same style
void Initialize_Button_Package_Select(BUTTON_PACKAGE*,
	struct nk_style_button normal, struct nk_style_button toggled,
	struct nk_style_button dormant, uint8_t total_buttons,
	bool require_one_selected = false);

// Render the button specified by index in the package
bool Render_Button_Label(BUTTON_PACKAGE*, struct nk_context*, const char*,
	uint8_t button_index, bool interactable = true);
bool Render_Button_Image(BUTTON_PACKAGE*, struct nk_context*, struct nk_image,
	uint8_t button_index, bool interactable = true);
bool Render_Button_Label_With_Buffer(BUTTON_PACKAGE*, struct nk_context*,
	const char*, uint8_t button_index, bool interactable = true);
bool Render_Button_Label_Buffered(BUTTON_PACKAGE*, struct nk_context*,
	const char*, uint8_t button_index, bool buffered, bool interactable = true);

// Render the next button with label
bool Render_Next_Button_Label(BUTTON_PACKAGE*, struct nk_context*, const char*,
	bool interactable = true);
bool Render_Next_Button_Image(BUTTON_PACKAGE*, struct nk_context*,
	struct nk_image, bool interactable = true);
bool Render_Next_Button_Label_With_Buffer(BUTTON_PACKAGE*, struct nk_context*,
	const char*, bool interactable = true);

void Delete_Button_Package(BUTTON_PACKAGE*);

// Button selection
// Called when buttons are pressed, but can also be called by client code
// Button types are still enforced. Select button does nothing if the package
// is of push buttons. Select button de-selects all other buttons if the
// package is of type "radio"
void Select_Button(BUTTON_PACKAGE*, uint8_t button_index);
void Deselect_Button(BUTTON_PACKAGE*, uint8_t button_index);
void Deselect_All_Buttons(BUTTON_PACKAGE*);

// Move to the next button
// Client code could use this to skip rendering a button
void Next_Button(BUTTON_PACKAGE*);
void Update_Button_Toggle_States(BUTTON_PACKAGE*, uint8_t button_selected);

// Return the first toggled button
// Makes more sense to use if only one button at a time can be selected
int8_t Toggled_Button_Index(const BUTTON_PACKAGE*);
BUTTON Toggled_Button(const BUTTON_PACKAGE*);

uint8_t* Toggled_Button_Indeces(const BUTTON_PACKAGE*, uint8_t* total_selected);
BUTTON* Toggled_Buttons(const BUTTON_PACKAGE*, uint8_t* total_selected);

// BUTTON PACKAGE HELPERS
void Enforce_At_Least_One_Selected(BUTTON_PACKAGE*, uint8_t prev_selected);
bool Check_And_Run_Button_Clicked(BUTTON_PACKAGE*, bool clicked);

/*
MISC BUTTON FUNCTIONS
*/
struct nk_rect Button_Content_Rect(
struct nk_rect button_rect, struct nk_style_button);

#endif // BUTTON_H_
