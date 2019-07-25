#include "Nk_Text_Style.h"

struct nk_style_text Nk_Text_Style(struct nk_color color) {
	struct nk_style_text style;
	Set_Nk_Text_Style(&style, color);
	return style;
}

void Set_Nk_Text_Style(struct nk_style_text* style, struct nk_color color) {
	Set_Nk_Text_Style_Defaults(style);
	style->color = color;
}
void Set_Nk_Text_Style_Defaults(struct nk_style_text* style) {
	style->color = MED_BLUE;
	style->padding = nk_vec2(0, 0);
}

struct nk_style_text Nk_Style_Text(struct nk_color c, struct nk_vec2 padding) {
	struct nk_style_text style;
	style.color = c;
	style.padding = padding;
	return style;
}
