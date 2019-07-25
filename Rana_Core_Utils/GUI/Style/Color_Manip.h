#ifndef COLOR_MANIP_H_
#define COLOR_MANIP_H_

#include "../GUI.h"

enum COLOR_TYPE {
	COLOR_NORMAL,
	COLOR_HOVER,
	COLOR_ACTIVE
};

struct COLOR_DIFF {
	int16_t normalApplications;
	int16_t hoverApplications;
	int16_t activeApplications;
	bool includeAlpha;
};

COLOR_DIFF Color_Diff_Pair(int16_t, int16_t, bool);
COLOR_DIFF Color_Diff_Trio(int16_t, int16_t, int16_t, bool);

struct COLOR_AUTODIFF {
	static COLOR_DIFF rule;
	static uint8_t diff;
};

void Set_COLOR_AUTODIFF(COLOR_DIFF, uint8_t);

struct nk_color Color_Autodiff(struct nk_color, COLOR_TYPE);
struct nk_color Color_Diff(
struct nk_color, COLOR_DIFF, int16_t diff, COLOR_TYPE);
struct nk_color Color_Diff(
struct nk_color, int16_t increment, int16_t applications, bool includeAlpha);
struct nk_color Color_Diff(struct nk_color, int16_t diff, bool includeAlpha);

int16_t Color_Autodiff_Applications(COLOR_TYPE);
int16_t Color_Diff_Applications(COLOR_DIFF, COLOR_TYPE);

#endif
