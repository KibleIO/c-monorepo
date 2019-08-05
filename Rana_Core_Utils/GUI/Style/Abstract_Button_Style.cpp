#include "Abstract_Button_Style.h"

ABSTRACT_BUTTON_STYLE Abstract_Button_Style(
ABSTRACT_BUTTON_COLORS colors, float border, struct nk_vec2 padding) {
	ABSTRACT_BUTTON_STYLE style;
	style.colors = colors;
	style.border = border;
	style.padding = padding;
	return style;
}
