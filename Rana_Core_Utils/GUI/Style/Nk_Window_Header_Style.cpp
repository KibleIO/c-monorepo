#include "Nk_Window_Header_Style.h"

struct nk_style_window_header Nk_Window_Header_Style() {
	return Nk_Window_Header_Style(TRANSPARENT, TRANSPARENT);
}
struct nk_style_window_header Nk_Window_Header_Style(
struct nk_color background, struct nk_color foreground) {
	struct nk_style_window_header style;
	Set_Nk_Window_Header_Style(&style, background, foreground);
	return style;
}

void Set_Nk_Window_Header_Style(
struct nk_style_window_header* style, struct nk_color background,
struct nk_color foreground) {
	Set_Nk_Window_Header_Style(
	style, background,
	Nk_Button_Style(background, foreground),
	Nk_Button_Style(background, foreground),
	foreground);
}

void Set_Nk_Window_Header_Style(
struct nk_style_window_header* style, struct nk_color background,
struct nk_style_button close_button, struct nk_style_button minimize_button,
struct nk_color label_color) {
	Set_Nk_Window_Header_Style_Defaults(style);

	style->normal = nk_style_item_color((background));
	style->hover = nk_style_item_color(lighter((background)));
	style->active = nk_style_item_color(darker((background)));

	style->close_button = close_button;
	style->minimize_button = minimize_button;

	style->label_normal = label_color;
	style->label_hover = label_color;
	style->label_active = label_color;
}

void Set_Nk_Window_Header_Style_Defaults(struct nk_style_window_header* style) {
	style->close_symbol = NK_SYMBOL_X;
	style->minimize_symbol = NK_SYMBOL_UNDERSCORE;
	style->maximize_symbol = NK_SYMBOL_RECT_OUTLINE;

	style->align = NK_HEADER_RIGHT;
	style->padding = NK_WINDOW_HEADER_DEFAULT_PADDING;
	style->label_padding = NK_WINDOW_HEADER_DEFAULT_LABEL_PADDING;
	style->spacing = nk_vec2(0, 0);
}
