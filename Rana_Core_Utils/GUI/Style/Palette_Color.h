#ifndef PALETTE_COLOR_H_
#define PALETTE_COLOR_H_

#include "../GUI.h"

struct PALETTE_COLOR {
	struct nk_color color;
	bool assigned;
};

PALETTE_COLOR Palette_Color(struct nk_color);

#endif
