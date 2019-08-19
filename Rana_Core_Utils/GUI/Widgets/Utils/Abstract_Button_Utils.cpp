#include "Abstract_Button_Utils.h"

void Setup_Style_ABSTRACT_BUTTON(
struct nk_context* ctx, uint8_t button_state,
const ABSTRACT_BUTTON_COLORS* colors, float border, struct nk_vec2 padding) {
	struct nk_color background;

	// Setup the background color based on the button's state
	switch (button_state) {
		case ABSTRACT_BUTTON_HOVER:
			background = colors->hover;
			break;
		case ABSTRACT_BUTTON_ACTIVE:
			background = colors->active;
			break;
		default:
			background = colors->normal;
			break;
	}

	// Setup the window style on the context
	ctx->style.window.fixed_background = nk_style_item_color(background);
	ctx->style.window.background = background;
	ctx->style.window.group_border_color = colors->border_color;
	ctx->style.window.group_border = border;
	ctx->style.window.group_padding = padding;
}

void Setup_State_ABSTRACT_BUTTON(
struct nk_context* ctx, struct nk_rect button_bounds, uint8_t* state,
bool* clicked) {

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
