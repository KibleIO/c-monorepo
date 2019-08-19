#ifndef COLOR_H_
#define COLOR_H_

#include "../../NK_BASE.h"
#include "Color_Diff.h"

struct COLOR_PAIR {
	struct nk_color normal;
	struct nk_color hover;
};

COLOR_PAIR Color_Pair_Uniform(struct nk_color);
COLOR_PAIR Color_Pair_Autodiff(struct nk_color);
COLOR_PAIR Color_Pair_Diff(struct nk_color, COLOR_DIFF, uint8_t diff);
COLOR_PAIR Color_Pair(struct nk_color, struct nk_color);

struct COLOR_TRIO {
	struct nk_color normal;
	struct nk_color hover;
	struct nk_color active;
};

COLOR_TRIO Color_Trio_Uniform(struct nk_color);
COLOR_TRIO Color_Trio_Autodiff(struct nk_color);
COLOR_TRIO Color_Trio_Diff(struct nk_color, COLOR_DIFF, uint8_t diff);
COLOR_TRIO Color_Trio(struct nk_color, struct nk_color, struct nk_color);

#endif
