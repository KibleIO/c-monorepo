#include "Abstract_Button_Style.h"

ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
struct nk_color background, struct nk_color border_color, float border,
struct nk_vec2 padding) {
	return Abstract_Button_Style(
	background, lighter(background), darker(background), border_color, border,
	padding);
}

ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
struct nk_color normal, struct nk_color hover, struct nk_color active,
struct nk_color border_color, float border, struct nk_vec2 padding) {
	ABSTRACT_BUTTON_STYLE style;
	style.normal = normal;
	style.hover = hover;
	style.active = active;
	style.border_color = border_color;
	style.border = border;
	style.padding = padding;
	return style;
}
