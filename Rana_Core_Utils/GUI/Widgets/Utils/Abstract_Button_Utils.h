#ifndef ABSTRACT_BUTTON_UTILS_H_
#define ABSTRACT_BUTTON_UTILS_H_

#include "../../GUI.h"
#include "../../Style/Abstract_Button_Style.h"

#define ABSTRACT_BUTTON_NORMAL	0
#define ABSTRACT_BUTTON_HOVER	1
#define ABSTRACT_BUTTON_ACTIVE	2

void Setup_Style_ABSTRACT_BUTTON(
struct nk_context*, uint8_t button_state, const ABSTRACT_BUTTON_COLORS*,
float border, struct nk_vec2 padding);
void Setup_State_ABSTRACT_BUTTON(
struct nk_context*, struct nk_rect button_bounds, uint8_t* state,
bool* clicked);

#endif
