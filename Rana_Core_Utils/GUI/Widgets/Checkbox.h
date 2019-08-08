#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "../Style/Nk_Toggle_Style.h"

struct CHECKBOX {
	struct nk_style_toggle style;
	int value;	// Value current in the checkbox
};

void Initialize_Checkbox(CHECKBOX*, struct nk_color background_color,
struct nk_color check_color, struct nk_color text_color);
void Initialize_Checkbox(CHECKBOX*, struct nk_style_toggle);
// Render the checkbox. Return true in the frame that it is clicked
bool Render_Checkbox(CHECKBOX*, struct nk_context*, const char*);
bool Render_Checkbox_With_Buffer(CHECKBOX*, struct nk_context*, const char*);
void Delete_Checkbox(CHECKBOX*);

// ACCESSORS
bool Checkbox_State(const CHECKBOX*);

// MUTATORS
void Set_Checkbox_State(CHECKBOX*, bool state);

#endif
