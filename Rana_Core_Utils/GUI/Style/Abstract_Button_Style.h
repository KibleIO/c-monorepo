#ifndef ABSTRACT_BUTTON_STYLE_H_
#define ABSTRACT_BUTTON_STYLE_H_

#include "../GUI.h"
#include "palette.h"

static const struct nk_vec2 ABSTRACT_BUTTON_STYLE_DEFAULT_PADDING =
nk_vec2(0, 0);

struct ABSTRACT_BUTTON_STYLE {
	struct nk_color normal;
	struct nk_color hover;
	struct nk_color active;
	struct nk_color border_color;

	float border;
	struct nk_vec2 padding;
};

ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
struct nk_color background, struct nk_color border_color = TRANSPARENT,
float border = 0,
struct nk_vec2 padding = ABSTRACT_BUTTON_STYLE_DEFAULT_PADDING);
ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
struct nk_color normal, struct nk_color hover, struct nk_color active,
struct nk_color border_color = TRANSPARENT, float border = 0,
struct nk_vec2 padding = ABSTRACT_BUTTON_STYLE_DEFAULT_PADDING);

#endif
