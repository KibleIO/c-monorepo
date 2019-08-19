#include "Border.h"

BORDER Border(struct nk_color color, float width) {
	BORDER border;
	border.color = color;
	border.width = width;
	return border;
}
