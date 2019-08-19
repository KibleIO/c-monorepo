#include "Nk_Scrollbar_Style.h"

struct nk_style_scrollbar Nk_Scrollbar_Style() {
	struct nk_style_scrollbar style;
	Set_Nk_Scrollbar_Style(&style, TRANSPARENT, TRANSPARENT);
	return style;
}
struct nk_style_scrollbar Nk_Scrollbar_Style(struct nk_color foreground_color,
struct nk_color background_color) {
	struct nk_style_scrollbar style;
	Set_Nk_Scrollbar_Style(&style, foreground_color, background_color);
	return style;
}

void Set_Nk_Scrollbar_Style(struct nk_style_scrollbar* style,
struct nk_color foreground_color, struct nk_color background_color) {
	Set_Nk_Scrollbar_Style_Defaults(style);

	style->normal = nk_style_item_color((background_color));
	style->hover = nk_style_item_color(lighter((background_color)));
	style->active = nk_style_item_color(darker((background_color)));

	style->cursor_normal = nk_style_item_color((foreground_color));
	style->cursor_hover = nk_style_item_color(
	lighter((foreground_color)));
	style->cursor_active = nk_style_item_color(
	darker((foreground_color)));
}

void Set_Nk_Scrollbar_Style_Defaults(struct nk_style_scrollbar* style) {
	style->border = 0;
	style->rounding = 0;
	style->border_cursor = 0;
	style->rounding_cursor = 0;
	style->padding = nk_vec2(0, 0);

	style->show_buttons = 0;

	style->userdata = nk_handle_ptr(NULL);
	style->draw_begin = NULL;
	style->draw_end = NULL;
}
