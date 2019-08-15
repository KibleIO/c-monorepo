#include "Panel_Data.h"

PANEL_DATA Panel_Data(
BORDER border, struct nk_vec2 padding) {
	PANEL_DATA data;
	data.border = border;
	data.padding = padding;
	return data;
}
