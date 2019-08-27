#include "Abstract_Button.h"

void Initialize_ABSTRACT_BUTTON(
ABSTRACT_BUTTON* button, ABSTRACT_BUTTON_STYLE style) {
	button->state = ABSTRACT_BUTTON_NORMAL;
	button->clicked = false;
	button->style = style;
}

bool Begin_ABSTRACT_BUTTON(
ABSTRACT_BUTTON* button, struct nk_context* ctx, const char* title,
nk_flags window_flags) {
	Setup_Style_ABSTRACT_BUTTON(
	ctx, button->state, &button->style.colors, button->style.border,
	button->style.padding);
	Setup_State_ABSTRACT_BUTTON(
	ctx, nk_widget_bounds(ctx), &button->state, &button->clicked);
	return nk_group_begin(ctx, title, window_flags);
}

bool Begin_ABSTRACT_BUTTON_With_Buffer(
ABSTRACT_BUTTON* button, struct nk_context* ctx, const char* title,
nk_flags window_flags) {
	nk_label(ctx, "", 0);
	return Begin_ABSTRACT_BUTTON(button, ctx, title, window_flags);
}

void End_ABSTRACT_BUTTON(
const ABSTRACT_BUTTON* button, struct nk_context* ctx) {
	Setup_Style_ABSTRACT_BUTTON(
	ctx, button->state, &button->style.colors, button->style.border,
	button->style.padding);
	nk_group_end(ctx);
}

// ACCESSORS
bool ABSTRACT_BUTTON_Clicked(const ABSTRACT_BUTTON* button) {
	return button->clicked;
}
