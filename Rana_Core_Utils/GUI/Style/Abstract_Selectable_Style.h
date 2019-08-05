#ifndef ABSTRACT_SELECTABLE_STYLE_H_
#define ABSTRACT_SELECTABLE_STYLE_H_

#include "Abstract_Button_Colors.h"

static const struct nk_vec2 ABSTRACT_SELECTABLE_STYLE_DEFAULT_PADDING =
nk_vec2(0, 0);

struct ABSTRACT_SELECTABLE_STYLE {
	ABSTRACT_BUTTON_COLORS defaultColors;
	ABSTRACT_BUTTON_COLORS selectedColors;
	float border;
	struct nk_vec2 padding;
};

ABSTRACT_SELECTABLE_STYLE Abstract_Selectable_Style(
ABSTRACT_BUTTON_COLORS def, ABSTRACT_BUTTON_COLORS sel, float border = 0,
struct nk_vec2 padding = ABSTRACT_SELECTABLE_STYLE_DEFAULT_PADDING);

#endif
