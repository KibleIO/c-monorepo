#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include "../GUI.h"
#include "Label.h"
#include "Image.h"
#include "../Style.h"
#include "../Layout/Rect_Transform.h"
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
bool Render_Button_Label(
BUTTON*, struct nk_context*, const char*, bool trailing,
bool interactable = true);
bool Render_Button_Image(
BUTTON*, struct nk_context*, IMAGE*, bool interactable = true);
bool Render_Button_Symbol_Label(
BUTTON*, struct nk_context*, enum nk_symbol_type, const char*, bool trailing,
nk_flags alignment, bool interactable = true);

bool Render_Button_Label_With_Buffer(
BUTTON*, struct nk_context*, const char*, bool trailing,
bool interactable = true);
bool Render_Button_Image_With_Buffer(
BUTTON*, struct nk_context*, IMAGE*, bool interactable = true);
bool Render_Button_Symbol_Label_With_Buffer(
BUTTON*, struct nk_context*, enum nk_symbol_type, const char*,
nk_flags alignment, bool interactable = true);

bool Render_Button_Label_Buffered(
BUTTON*, struct nk_context*, const char*, bool trailing, bool buffered,
bool interactable = true);

void Delete_Button(BUTTON*);

void Set_Button_Toggle_State(BUTTON*, bool state);
void Toggle_Button_State(BUTTON*);

// BUTTON HELPERS
void Setup_Button_Style(BUTTON*, struct nk_context*, bool interactable);
bool Check_And_Run_Button_Clicked(BUTTON*, bool clicked, bool interactable);

/*
MISC BUTTON FUNCTIONS
*/
struct nk_rect Button_Content_Rect(
struct nk_rect button_rect, struct nk_style_button);

#endif // BUTTON_H_
