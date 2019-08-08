#ifndef ABSTRACT_BUTTON_COLORS_H_
#define ABSTRACT_BUTTON_COLORS_H_

#include "../NK_BASE.h"
#include "palette.h"

struct ABSTRACT_BUTTON_COLORS {
	struct nk_color normal;
	struct nk_color hover;
	struct nk_color active;
	struct nk_color border_color;
};

ABSTRACT_BUTTON_COLORS Abstract_Button_Colors(
struct nk_color, struct nk_color = TRANSPARENT);
ABSTRACT_BUTTON_COLORS Abstract_Button_Colors(
struct nk_color normal, struct nk_color hover, struct nk_color active,
struct nk_color = TRANSPARENT);

#endif
