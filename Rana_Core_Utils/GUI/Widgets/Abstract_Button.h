#ifndef ABSTRACT_BUTTON_H_
#define ABSTRACT_BUTTON_H_

#include "../Style/Abstract_Button_Style.h"

#define ABSTRACT_BUTTON_NORMAL	0
#define ABSTRACT_BUTTON_HOVER	1
#define ABSTRACT_BUTTON_ACTIVE	2

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
void End_ABSTRACT_BUTTON(struct nk_context*);

// ACCESSORS
bool ABSTRACT_BUTTON_Clicked(const ABSTRACT_BUTTON*);

// HELPERS
void Setup_Style_ABSTRACT_BUTTON(
struct nk_context*, uint8_t button_state, const ABSTRACT_BUTTON_STYLE*);
void Setup_State_ABSTRACT_BUTTON(
struct nk_context*, uint8_t* state, bool* clicked);
void Setup_Hover_Normal_State_ABSTRACT_BUTTON(
struct nk_context*, uint8_t* state, struct nk_rect);

#endif
