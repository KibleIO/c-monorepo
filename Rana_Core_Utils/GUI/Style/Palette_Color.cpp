#include "Palette_Color.h"

PALETTE_COLOR Palette_Color(struct nk_color color) {
	PALETTE_COLOR pcolor;
	pcolor.color = color;
	pcolor.assigned = true;
	return pcolor;
}
