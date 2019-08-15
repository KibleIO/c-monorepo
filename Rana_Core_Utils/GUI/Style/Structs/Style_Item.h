#ifndef STYLE_ITEM_H_
#define STYLE_ITEM_H_

#include "../../NK_BASE.h"
#include "Color.h"

struct STYLE_ITEM_PAIR {
	struct nk_style_item normal;
	struct nk_style_item hover;
};

STYLE_ITEM_PAIR Style_Item_Pair_Color(COLOR_PAIR);
STYLE_ITEM_PAIR Style_Item_Pair(struct nk_style_item, struct nk_style_item);

struct STYLE_ITEM_TRIO {
	struct nk_style_item normal;
	struct nk_style_item hover;
	struct nk_style_item active;
};

STYLE_ITEM_TRIO Style_Item_Trio_Color(COLOR_TRIO);
STYLE_ITEM_TRIO Style_Item_Trio(
struct nk_style_item, struct nk_style_item, struct nk_style_item);

struct STYLE_ITEM_TRIO_PAIR {
	STYLE_ITEM_TRIO active;
	STYLE_ITEM_TRIO inactive;
};

#endif
