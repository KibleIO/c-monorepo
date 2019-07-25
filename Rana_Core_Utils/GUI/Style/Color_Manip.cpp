#include "Color_Manip.h"

COLOR_DIFF COLOR_AUTODIFF::rule = Color_Diff_Trio(0, 1, -1, false);
uint8_t COLOR_AUTODIFF::diff = 8;

COLOR_DIFF Color_Diff_Pair(
int16_t normal, int16_t hover, bool includeAlpha) {
	return Color_Diff_Trio(normal, hover, hover, includeAlpha);
}

COLOR_DIFF Color_Diff_Trio(
int16_t normal, int16_t hover, int16_t active, bool includeAlpha) {
	COLOR_DIFF diff;
	diff.normalApplications = normal;
	diff.hoverApplications = hover;
	diff.activeApplications = active;
	diff.includeAlpha = includeAlpha;
	return diff;
}

struct nk_color Color_Autodiff(struct nk_color c, COLOR_TYPE type) {
	return Color_Diff(c, COLOR_AUTODIFF::rule, COLOR_AUTODIFF::diff, type);
}

struct nk_color Color_Diff(
struct nk_color c, COLOR_DIFF rule, int16_t diff, COLOR_TYPE type) {
	return Color_Diff(
	c, Color_Diff_Applications(rule, type), diff, rule.includeAlpha);
}

struct nk_color Color_Diff(
struct nk_color c, int16_t increment, int16_t applications, bool includeAlpha) {
	return Color_Diff(c, increment * applications, includeAlpha);
}

struct nk_color Color_Diff(struct nk_color c, int16_t diff, bool includeAlpha) {
	if (includeAlpha) {
		return nk_rgba(c.r + diff, c.g + diff, c.b + diff, c.a + diff);
	}
	else {
		return nk_rgb(c.r + diff, c.g + diff, c.b + diff);
	}
}

int16_t Color_Autodiff_Applications(COLOR_TYPE type) {
	return Color_Diff_Applications(COLOR_AUTODIFF::rule, type);
}
int16_t Color_Diff_Applications(COLOR_DIFF rule, COLOR_TYPE type) {
	switch (type) {
		case COLOR_NORMAL:	return rule.normalApplications;
		case COLOR_HOVER:	return rule.hoverApplications;
		case COLOR_ACTIVE:	return rule.activeApplications;
		default:			return 0;
	}
}
