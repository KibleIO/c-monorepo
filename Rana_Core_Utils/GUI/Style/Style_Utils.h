#ifndef STYLE_UTILS_H_
#define STYLE_UTILS_H_

#include "../NK_BASE.h"
#include "Color_Manip.h"

struct COLOR_PAIR {
	struct nk_color normal;
	struct nk_color hover;
};

COLOR_PAIR Color_Pair_Uniform(struct nk_color);
COLOR_PAIR Color_Pair_Autodiff(struct nk_color);
COLOR_PAIR Color_Pair_Diff(struct nk_color, COLOR_DIFF, uint8_t diff);
COLOR_PAIR Color_Pair(struct nk_color, struct nk_color);

struct COLOR_TRIO {
	struct nk_color normal;
	struct nk_color hover;
	struct nk_color active;
};

COLOR_TRIO Color_Trio_Uniform(struct nk_color);
COLOR_TRIO Color_Trio_Autodiff(struct nk_color);
COLOR_TRIO Color_Trio_Diff(struct nk_color, COLOR_DIFF, uint8_t diff);
COLOR_TRIO Color_Trio(struct nk_color, struct nk_color, struct nk_color);

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

struct SYMBOL_TYPE_TRIO {
	enum nk_symbol_type normal;
	enum nk_symbol_type hover;
	enum nk_symbol_type active;
};

SYMBOL_TYPE_TRIO Symbol_Type_Trio_Uniform(enum nk_symbol_type);
SYMBOL_TYPE_TRIO Symbol_Type_Trio(
enum nk_symbol_type, enum nk_symbol_type, enum nk_symbol_type);

struct USER_CALLBACK {
	nk_handle userdata;
	void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata);
	void(*draw_end)(struct nk_command_buffer*, nk_handle userdata);
};

USER_CALLBACK User_Callback_Empty();
USER_CALLBACK User_Callback(
nk_handle, void(*)(struct nk_command_buffer*, nk_handle userdata),
void(*)(struct nk_command_buffer*, nk_handle userdata));

struct PANEL_DATA {
	struct nk_color border_color;
	float border;
	struct nk_vec2 padding;
};

PANEL_DATA Panel_Data(struct nk_color, float, struct nk_vec2);

#endif
