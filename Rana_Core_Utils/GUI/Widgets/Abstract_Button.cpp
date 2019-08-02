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
	Setup_Style_ABSTRACT_BUTTON(ctx, button->state, &button->style);
	Setup_State_ABSTRACT_BUTTON(ctx, &button->state, &button->clicked);
	return nk_group_begin(ctx, title, window_flags);
}

bool Begin_ABSTRACT_BUTTON_With_Buffer(
ABSTRACT_BUTTON* button, struct nk_context* ctx, const char* title,
nk_flags window_flags) {
	nk_label(ctx, "", 0);
	return Begin_ABSTRACT_BUTTON(button, ctx, title, window_flags);
}

void End_ABSTRACT_BUTTON(struct nk_context* ctx) {
	nk_group_end(ctx);
}

// ACCESSORS
bool ABSTRACT_BUTTON_Clicked(const ABSTRACT_BUTTON* button) {
	return button->clicked;
}

// HELPERS
void Setup_Style_ABSTRACT_BUTTON(
struct nk_context* ctx, uint8_t button_state,
const ABSTRACT_BUTTON_STYLE* style) {
	struct nk_color background;

	// Setup the background color based on the button's state
	switch (button_state) {
		case ABSTRACT_BUTTON_HOVER:
			background = style->hover;
			break;
		case ABSTRACT_BUTTON_ACTIVE:
			background = style->active;
			break;
		default:
			background = style->normal;
			break;
	}

	// Setup the window style on the context
	ctx->style.window.fixed_background = nk_style_item_color(background);
	ctx->style.window.background = background;
	ctx->style.window.group_border_color = style->border_color;
	ctx->style.window.group_border = style->border;
	ctx->style.window.group_padding = style->padding;
}

void Setup_State_ABSTRACT_BUTTON(
struct nk_context* ctx, uint8_t* state, bool* clicked) {
	struct nk_rect button_bounds = nk_widget_bounds(ctx);

	*state = ABSTRACT_BUTTON_NORMAL;

	if (nk_input_is_mouse_hovering_rect(&ctx->input, button_bounds)) {
		*state = ABSTRACT_BUTTON_HOVER;
		
		if (nk_input_is_mouse_down(&ctx->input, NK_BUTTON_LEFT)) {
			*state = ABSTRACT_BUTTON_ACTIVE;
		}
	}

	// Set clicked to true once on mouse left button down
	*clicked = nk_input_is_mouse_click_down_in_rect(
	&ctx->input, NK_BUTTON_LEFT, button_bounds, true);
}
