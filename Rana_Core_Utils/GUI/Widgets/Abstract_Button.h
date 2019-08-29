#ifndef ABSTRACT_BUTTON_H_
#define ABSTRACT_BUTTON_H_

#include "Utils/Abstract_Button_Utils.h"
#include "../Style/Abstract_Button_Style.h"

// A panel that the user can push any number of widgets into.
// The struct keeps internal state of whether ANY of the widgets are
// hovered or clicked, stretching button-like functionality
// over an entire group of widgets
struct ABSTRACT_BUTTON {
	uint8_t state;	// State of the button - normal, hover, active
	bool clicked;	// True in the frame that the button is clicked
	ABSTRACT_BUTTON_STYLE style;
};

void Initialize_ABSTRACT_BUTTON(ABSTRACT_BUTTON*, ABSTRACT_BUTTON_STYLE);

bool Begin_ABSTRACT_BUTTON(
ABSTRACT_BUTTON*, struct nk_context*, const char*, nk_flags);
bool Begin_ABSTRACT_BUTTON_With_Buffer(
ABSTRACT_BUTTON*, struct nk_context*, const char*, nk_flags);
void End_ABSTRACT_BUTTON(const ABSTRACT_BUTTON*, struct nk_context*);

// ACCESSORS
bool ABSTRACT_BUTTON_Clicked(const ABSTRACT_BUTTON*);

#endif
