#include "Style_Utils.h"

COLOR_PAIR Color_Pair_Uniform(struct nk_color color) {
	return Color_Pair(color, color);
}
COLOR_PAIR Color_Pair_Autodiff(struct nk_color color) {
	return Color_Pair(
	Color_Autodiff(color, COLOR_NORMAL), Color_Autodiff(color, COLOR_HOVER));
}
COLOR_PAIR Color_Pair_Diff(
struct nk_color color, COLOR_DIFF rule, uint8_t diff) {
	return Color_Pair(
	Color_Diff(color, rule, diff, COLOR_NORMAL),
	Color_Diff(color, rule, diff, COLOR_HOVER));
}
COLOR_PAIR Color_Pair(struct nk_color normal, struct nk_color hover) {
	COLOR_PAIR pair;
	pair.normal = normal;
	pair.hover = hover;
	return pair;
}

COLOR_TRIO Color_Trio_Uniform(struct nk_color color) {
	return Color_Trio(color, color, color);
}
COLOR_TRIO Color_Trio_Autodiff(struct nk_color color) {
	return Color_Trio(
	Color_Autodiff(color, COLOR_NORMAL), Color_Autodiff(color, COLOR_HOVER),
	Color_Autodiff(color, COLOR_ACTIVE));
}
COLOR_TRIO Color_Trio_Diff(
struct nk_color color, COLOR_DIFF rule, uint8_t diff) {
	return Color_Trio(
	Color_Diff(color, rule, diff, COLOR_NORMAL),
	Color_Diff(color, rule, diff, COLOR_HOVER),
	Color_Diff(color, rule, diff, COLOR_ACTIVE));
}
COLOR_TRIO Color_Trio(
struct nk_color normal, struct nk_color hover, struct nk_color active) {
	COLOR_TRIO trio;
	trio.normal = normal;
	trio.hover = hover;
	trio.active = active;
	return trio;
}

STYLE_ITEM_PAIR Style_Item_Pair_Color(COLOR_PAIR pair) {
	return Style_Item_Pair(
	nk_style_item_color(pair.normal), nk_style_item_color(pair.hover));
}
STYLE_ITEM_PAIR Style_Item_Pair(
struct nk_style_item normal, struct nk_style_item hover) {
	STYLE_ITEM_PAIR pair;
	pair.normal = normal;
	pair.hover = hover;
	return pair;
}

STYLE_ITEM_TRIO Style_Item_Trio_Color(COLOR_TRIO trio) {
	return Style_Item_Trio(
	nk_style_item_color(trio.normal), nk_style_item_color(trio.hover),
	nk_style_item_color(trio.active));
}
STYLE_ITEM_TRIO Style_Item_Trio(
struct nk_style_item normal, struct nk_style_item hover,
struct nk_style_item active) {
	STYLE_ITEM_TRIO trio;
	trio.normal = normal;
	trio.hover = hover;
	trio.active = active;
	return trio;
}

SYMBOL_TYPE_TRIO Symbol_Type_Trio_Uniform(enum nk_symbol_type symbol) {
	return Symbol_Type_Trio(symbol, symbol, symbol);
}
SYMBOL_TYPE_TRIO Symbol_Type_Trio(
enum nk_symbol_type normal, enum nk_symbol_type hover,
enum nk_symbol_type active) {
	SYMBOL_TYPE_TRIO trio;
	trio.normal = normal;
	trio.hover = hover;
	trio.active = active;
	return trio;
}

USER_CALLBACK User_Callback_Empty() {
	return User_Callback(nk_handle_ptr(NULL), NULL, NULL);
}
USER_CALLBACK User_Callback(
nk_handle userdata,
void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata),
void(*draw_end)(struct nk_command_buffer*, nk_handle userdata)) {
	USER_CALLBACK callback;
	callback.userdata = userdata;
	callback.draw_begin = draw_begin;
	callback.draw_end = draw_end;
	return callback;
}

PANEL_DATA Panel_Data(
struct nk_color color, float border, struct nk_vec2 padding) {
	PANEL_DATA data;
	data.border_color = color;
	data.border = border;
	data.padding = padding;
	return data;
}
