#ifndef BORDER_H_
#define BORDER_H_

#include "../../NK_BASE.h"

struct BORDER {
	struct nk_color color;
	float width;
};

BORDER Border(struct nk_color, float);

#endif
