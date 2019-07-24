#include "Nk_Slider_Style.h"

struct nk_style_slider Nk_Slider_Style(
struct nk_color bar_color, struct nk_color cursor) {
	struct nk_style_slider style;
	Set_Nk_Slider_Style(&style, bar_color, cursor);
	return style;
}

struct nk_style_slider Nk_Slider_Style(
struct nk_color bar_color, struct nk_color bar_filled_color,
struct nk_color cursor) {
	struct nk_style_slider style;
	Set_Nk_Slider_Style(&style, bar_color, bar_filled_color, cursor);
	return style;
}

void Set_Nk_Slider_Style(
struct nk_style_slider* style, struct nk_color bar_color,
struct nk_color cursor) {
	Set_Nk_Slider_Style(style, bar_color, cursor, cursor);
}

void Set_Nk_Slider_Style(
struct nk_style_slider* style, struct nk_color bar_color,
struct nk_color bar_filled_color, struct nk_color cursor) {
	Set_Nk_Slider_Style_Defaults(style);

	style->bar_normal = (bar_color);
	style->bar_hover = (lighter(bar_color));
	style->bar_active = (darker(bar_color));
	style->bar_filled = (bar_filled_color);

	style->cursor_normal = nk_style_item_color(cursor);
	style->cursor_hover = nk_style_item_color(lighter(cursor));
	style->cursor_active = nk_style_item_color(darker(cursor));

}

void Set_Nk_Slider_Style_Defaults(struct nk_style_slider* style) {
	style->border_color = LIGHT_GRAY;

	style->normal = nk_style_item_color(TRANSPARENT);
	style->hover = nk_style_item_color(TRANSPARENT);
	style->active = nk_style_item_color(TRANSPARENT);

	style->border = 0;
	style->rounding = 0;
	style->bar_height = SLIDER_BAR_DEFAULT_BAR_HEIGHT;
	style->padding = nk_vec2(0, 0);
	style->spacing = nk_vec2(0, 0);
	style->cursor_size = SLIDER_BAR_DEFAULT_CURSOR_SIZE;

	style->show_buttons = 0;
	style->inc_button = Nk_Button_Style(MED_BLUE, LIGHT_BLUE);
	style->dec_button = Nk_Button_Style(MED_BLUE, LIGHT_BLUE);
	style->inc_symbol = NK_SYMBOL_PLUS;
	style->dec_symbol = NK_SYMBOL_MINUS;

	style->userdata = nk_handle_ptr(NULL);
	style->draw_begin = NULL;
	style->draw_end = NULL;
}
