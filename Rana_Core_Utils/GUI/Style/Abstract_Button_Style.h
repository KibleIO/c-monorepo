#ifndef ABSTRACT_BUTTON_STYLE_H_
#define ABSTRACT_BUTTON_STYLE_H_

#include "../GUI.h"
#include "palette.h"
#include "Abstract_Button_Colors.h"

static const struct nk_vec2 ABSTRACT_BUTTON_STYLE_DEFAULT_PADDING =
nk_vec2(0, 0);

struct ABSTRACT_BUTTON_STYLE {
	ABSTRACT_BUTTON_COLORS colors;
	float border;
	struct nk_vec2 padding;
};

ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
ABSTRACT_BUTTON_COLORS, float border = 0,
struct nk_vec2 padding = ABSTRACT_BUTTON_STYLE_DEFAULT_PADDING);

#endif
