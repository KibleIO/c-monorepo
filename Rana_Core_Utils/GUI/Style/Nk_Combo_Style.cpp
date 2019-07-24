#include "Nk_Combo_Style.h"

struct nk_style_combo Nk_Combo_Style(
struct nk_color background, struct nk_color foreground) {
	return Nk_Combo_Style(background, foreground, nk_vec2(0, 0));
}

struct nk_style_combo Nk_Combo_Style(
struct nk_color background, struct nk_color foreground,
struct nk_vec2 padding) {
	struct nk_style_combo style;
	Set_Nk_Combo_Style(&style, background, foreground, padding);
	return style;
}

void Set_Nk_Combo_Style(
struct nk_style_combo* style, struct nk_color background,
struct nk_color foreground) {
	Set_Nk_Combo_Style(style, background, foreground, nk_vec2(0, 0));
}

void Set_Nk_Combo_Style(
struct nk_style_combo* style, struct nk_color background,
struct nk_color foreground, struct nk_vec2 padding) {
	Set_Nk_Combo_Style_Defaults(style);

	style->normal = nk_style_item_color((background));
	style->hover = nk_style_item_color(lighter((background)));
	style->active = nk_style_item_color(darker((background)));
	style->border_color = foreground;

	style->label_normal = foreground;
	style->label_hover = foreground;
	style->label_active = foreground;

	style->symbol_normal = (foreground);
	style->symbol_hover = (foreground);
	style->symbol_active = (foreground);

	style->button = Nk_Button_Style_Padded(
	background, foreground, padding, nk_vec2(0, 0), nk_vec2(0, 0));
}

void Set_Nk_Combo_Style_Defaults(struct nk_style_combo* style) {
	style->sym_normal = DEFAULT_NK_COMBO_BUTTON_SYMBOL;
	style->sym_hover = DEFAULT_NK_COMBO_BUTTON_SYMBOL;
	style->sym_active = DEFAULT_NK_COMBO_BUTTON_SYMBOL;

	style->border = 0;
	style->rounding = 0;
	style->content_padding = nk_vec2(0, 0);
	style->button_padding = nk_vec2(0, 0);
	style->spacing = nk_vec2(0, 0);
}
