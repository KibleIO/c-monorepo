#include "Text_Data.h"

TEXT_DATA Text_Color(
COLOR_TRIO color, struct nk_color background, nk_flags alignment) {
	TEXT_DATA text;
	text.color = color;
	text.background = background;
	text.alignment = alignment;
	return text;
}
