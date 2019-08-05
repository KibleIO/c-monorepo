#include "Abstract_Button_Colors.h"

ABSTRACT_BUTTON_COLORS Abstract_Button_Colors(
struct nk_color color, struct nk_color border_color) {
	return Abstract_Button_Colors(
	color, lighter(color), darker(color), border_color);
}

ABSTRACT_BUTTON_COLORS Abstract_Button_Colors(
struct nk_color normal, struct nk_color hover, struct nk_color active,
struct nk_color border) {
	ABSTRACT_BUTTON_COLORS colors;
	colors.normal = normal;
	colors.hover = hover;
	colors.active = active;
	colors.border_color = border;
	return colors;
}
