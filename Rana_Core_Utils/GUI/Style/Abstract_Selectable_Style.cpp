#include "Abstract_Selectable_Style.h"

ABSTRACT_SELECTABLE_STYLE Abstract_Selectable_Style(
ABSTRACT_BUTTON_COLORS default_colors, ABSTRACT_BUTTON_COLORS selected_colors,
float border, struct nk_vec2 padding) {
	ABSTRACT_SELECTABLE_STYLE style;
	style.defaultColors = default_colors;
	style.selectedColors = selected_colors;
	style.border = border;
	style.padding = padding;
	return style;
}
