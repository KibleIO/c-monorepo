#include "Nk_Window_Style.h"

struct nk_style_window Nk_Window_Style() {
	struct nk_style_window style;
	Set_Nk_Window_Style(&style);
	return style;
}
struct nk_style_window Nk_Window_Style(struct nk_color color) {
	struct nk_style_window style;
	Set_Nk_Window_Style(&style, color);
	return style;
}
struct nk_style_window Nk_Window_Style(struct nk_color color, float rounding) {
	struct nk_style_window style;
	Set_Nk_Window_Style(&style, color, rounding);
	return style;
}
struct nk_style_window Nk_Window_Style(struct nk_color background,
struct nk_vec2 scrollbar_size) {
	struct nk_style_window style;
	Set_Nk_Window_Style(&style, background, scrollbar_size);
	return style;
}
struct nk_style_window Nk_Window_Style(
struct nk_color background, struct nk_style_window_header header) {
	return Nk_Window_Style(background, nk_vec2(0, 0), header);
}
struct nk_style_window Nk_Window_Style(
struct nk_color background, struct nk_vec2 scrollbar_size,
struct nk_style_window_header header) {
	struct nk_style_window style;
	Set_Nk_Window_Style(&style, background, scrollbar_size, header);
	return style;
}

void Set_Nk_Window_Style(struct nk_style_window* style) {
	Set_Nk_Window_Style(style, TRANSPARENT);
}
void Set_Nk_Window_Style(struct nk_style_window* style, struct nk_color color) {
	Set_Nk_Window_Style_Defaults(style);
	style->fixed_background = nk_style_item_color((color));
	style->background = color;
}
void Set_Nk_Window_Style(struct nk_style_window* style, struct nk_color color,
float rounding) {
	Set_Nk_Window_Style(style, color);
	style->rounding = rounding;
}
void Set_Nk_Window_Style(struct nk_style_window* style, struct nk_color color,
struct nk_vec2 scrollbar_size) {
	Set_Nk_Window_Style(style, color);
	style->scrollbar_size = scrollbar_size;
}
void Set_Nk_Window_Style(
struct nk_style_window* style, struct nk_color color,
struct nk_vec2 scrollbar_size, struct nk_style_window_header header) {
	Set_Nk_Window_Style(style, color, scrollbar_size);
	style->header = header;
}
void Set_Nk_Window_Style_Defaults(struct nk_style_window* style) {
	style->header = Nk_Window_Header_Style();

	style->background = TRANSPARENT;

	style->border = 0;
	style->group_border = 0;
	style->popup_border = 0;
	style->combo_border = 0;
	style->contextual_border = 0;
	style->menu_border = 0;
	style->tooltip_border = 0;

	style->rounding = 0;
	style->spacing = nk_vec2(0, 0);
	style->scrollbar_size = nk_vec2(0, 0);
	style->min_size = nk_vec2(0, 0);

	style->padding = nk_vec2(0, 0);
	style->group_padding = nk_vec2(0, 0);
	style->popup_padding = nk_vec2(0, 0);
	style->combo_padding = nk_vec2(0, 0);
	style->contextual_padding = nk_vec2(0, 0);
	style->menu_padding = nk_vec2(0, 0);
	style->tooltip_padding = nk_vec2(0, 0);
}
