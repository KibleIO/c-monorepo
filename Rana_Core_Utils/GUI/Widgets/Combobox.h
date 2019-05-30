#ifndef COMBOBOX_H_
#define COMBOBOX_H_

#include "../GUI.h"
#include "../Style.h"
#include "../../Utilities/vararg_ext.h"

// Will the size be set by the callee or
// inherited from the size related to it?
enum COMBOBOX_ITEM_TYPE {
	COMBOBOX_ITEM_TYPE_SET,
	COMBOBOX_ITEM_TYPE_INHERIT
};

/*
COMBOBOX_ITEM
*/

struct COMBOBOX_ITEM {
	uint32_t size;
	COMBOBOX_ITEM_TYPE type;
};

COMBOBOX_ITEM Combobox_Item(uint32_t size, COMBOBOX_ITEM_TYPE);
COMBOBOX_ITEM Combobox_Item_Set(uint32_t size);
COMBOBOX_ITEM Combobox_Item_Inherit();

/*
COMBOBOX
*/

#define MAX_COMBOBOX_ITEMS 25

struct COMBOBOX {
	struct nk_style_combo style;
	struct nk_style_window dropdownWindowStyle;
	struct nk_style_button dropdownButtonStyle;

	char* items[MAX_COMBOBOX_ITEMS];
	uint8_t totalItems;
	int selectedItem;
};

void Initialize_COMBOBOX(
COMBOBOX*, struct nk_style_combo, struct nk_style_window,
struct nk_style_button, uint8_t total_items, /* const char* */...);
void Initialize_COMBOBOX(
COMBOBOX*, struct nk_style_combo, struct nk_style_window,
struct nk_style_button, uint8_t total_items, const char* items[]);

// Render the combobox. Return true if the value changed
bool Render_COMBOBOX(
COMBOBOX*, struct nk_context*, COMBOBOX_ITEM item_height,
COMBOBOX_ITEM dropdown_width, COMBOBOX_ITEM dropdown_height);

void Delete_COMBOBOX(COMBOBOX*);

/*
GETTERS
*/

int Selected_Index_COMBOBOX(const COMBOBOX*);
string Selected_Item_COMBOBOX(const COMBOBOX*);

/*
HELPERS
*/

void Setup_Combobox_Style(COMBOBOX*, struct nk_context*);

void Setup_Combobox_Items(
struct nk_context*, uint8_t total_items, COMBOBOX_ITEM* item_height,
COMBOBOX_ITEM* dropdown_width, COMBOBOX_ITEM* dropdown_height);
void Setup_Combobox_Item(COMBOBOX_ITEM* item, uint32_t inheritance_size);

#endif
