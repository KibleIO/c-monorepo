#include "Nk_Toggle_Style.h"

struct nk_style_toggle Nk_Toggle_Style(struct nk_color background,
struct nk_color check, struct nk_color text) {
	struct nk_style_toggle style;
	Set_Nk_Toggle_Style(&style, background, check, text);
	return style;
}

void Set_Nk_Toggle_Style(struct nk_style_toggle* style,
struct nk_color background, struct nk_color check, struct nk_color text) {
	// Set default styling
	Set_Nk_Toggle_Style_Defaults(style);

	style->normal = nk_style_item_color((background));
	style->hover = nk_style_item_color(lighter((background)));
	style->active = nk_style_item_color(darker((background)));

	style->cursor_normal = nk_style_item_color((check));
	style->cursor_hover = nk_style_item_color(lighter((check)));

	style->text_normal = text;
	style->text_hover = text;
	style->text_active = text;
}

void Set_Nk_Toggle_Style_Defaults(struct nk_style_toggle* style) {
	style->border_color = TRANSPARENT;

	style->text_background = TRANSPARENT;
	style->text_alignment = NK_TEXT_CENTERED;

	style->padding = nk_vec2(DEFAULT_TOGGLE_PADDING, DEFAULT_TOGGLE_PADDING);
	style->touch_padding = nk_vec2(0, 0);
	style->spacing = DEFAULT_TOGGLE_SPACING;
	style->border = 0;

	style->userdata = nk_handle_ptr(NULL);
	style->draw_begin = NULL;
	style->draw_end = NULL;
}
