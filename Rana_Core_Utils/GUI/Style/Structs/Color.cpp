#include "Color.h"

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
