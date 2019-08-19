#include "Style_Item.h"

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
