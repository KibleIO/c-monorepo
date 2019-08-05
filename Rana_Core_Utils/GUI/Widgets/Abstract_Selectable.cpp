#include "Abstract_Selectable.h"

void Initialize_ABSTRACT_SELECTABLE(
ABSTRACT_SELECTABLE* selectable, ABSTRACT_SELECTABLE_STYLE style) {
	selectable->state = ABSTRACT_BUTTON_NORMAL;
	selectable->clicked = false;
	selectable->selected = false;
	selectable->style = style;
}

bool Begin_ABSTRACT_SELECTABLE(
ABSTRACT_SELECTABLE* selectable, struct nk_context* ctx, const char* title,
nk_flags window_flags) {
	// Setup style of the abstract button based on whether or not is selected
	if (selectable->selected) {
		Setup_Style_ABSTRACT_BUTTON(
		ctx, selectable->state, &selectable->style.defaultColors,
		selectable->style.border, selectable->style.padding);
	}
	else {
		Setup_Style_ABSTRACT_BUTTON(
		ctx, selectable->state, &selectable->style.selectedColors,
		selectable->style.border, selectable->style.padding);
	}

	// Setup the state and clicked bool for the selectable
	Setup_State_ABSTRACT_BUTTON(
	ctx, nk_widget_bounds(ctx), &selectable->state, &selectable->clicked);

	if (ABSTRACT_SELECTABLE_Clicked(selectable)) {
		selectable->selected = !selectable->selected;
	}

	return nk_group_begin(ctx, title, window_flags);
}
