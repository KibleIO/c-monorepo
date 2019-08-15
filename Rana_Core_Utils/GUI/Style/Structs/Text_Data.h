#ifndef TEXT_DATA_H_
#define TEXT_DATA_H_

#include "../../NK_BASE.h"
#include "Color.h"
#include "../palette.h"

struct TEXT_DATA {
	COLOR_TRIO color;
	struct nk_color background;
	nk_flags alignment;
};

TEXT_DATA Text_Color(COLOR_TRIO, struct nk_color, nk_flags);

#endif
