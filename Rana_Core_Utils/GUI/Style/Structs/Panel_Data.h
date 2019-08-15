#ifndef PANEL_DATA_H_
#define PANEL_DATA_H_

#include "../../NK_BASE.h"
#include "Border.h"

struct PANEL_DATA {
	BORDER border;
	struct nk_vec2 padding;
};

PANEL_DATA Panel_Data(BORDER, struct nk_vec2);

#endif
