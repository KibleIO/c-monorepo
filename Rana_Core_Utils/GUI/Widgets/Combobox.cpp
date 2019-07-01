#include "Combobox.h"

/*
COMBOBOX_ITEM
*/

COMBOBOX_ITEM Combobox_Item_Set(uint32_t size) {
	return Combobox_Item(size, COMBOBOX_ITEM_TYPE_SET);
}
COMBOBOX_ITEM Combobox_Item_Inherit() {
	return Combobox_Item(0, COMBOBOX_ITEM_TYPE_INHERIT);
}
COMBOBOX_ITEM Combobox_Item(uint32_t size, COMBOBOX_ITEM_TYPE type) {
	COMBOBOX_ITEM item;
	item.size = size;
	item.type = type;
	return item;
}

/*
COMBOBOX
*/

void Initialize_COMBOBOX(
COMBOBOX* combobox, struct nk_style_combo style,
struct nk_style_window dropdown_window_style,
struct nk_style_button dropdown_button_style, uint8_t total_items, ...) {
	const char* items[MAX_COMBOBOX_ITEMS];
	va_array(items, total_items, const char*);
	Initialize_COMBOBOX(
	combobox, style, dropdown_window_style, dropdown_button_style, total_items,
	items);
}

void Initialize_COMBOBOX(
COMBOBOX* combobox, struct nk_style_combo style,
struct nk_style_window dropdown_window_style,
struct nk_style_button dropdown_button_style, uint8_t total_items,
const char* items[]) {
	combobox->style = style;
	combobox->dropdownWindowStyle = dropdown_window_style;
	combobox->dropdownButtonStyle = dropdown_button_style;

	combobox->totalItems = total_items;
	combobox->selectedItem = 0;

	for(uint8_t i = 0; i < total_items && i < MAX_COMBOBOX_ITEMS; i++) {
		combobox->items[i] = new char[strlen(items[i]) + 1];
		strcpy(combobox->items[i], items[i]);
	}
}

bool Render_COMBOBOX(
COMBOBOX* combobox, struct nk_context* ctx, COMBOBOX_ITEM item_height,
COMBOBOX_ITEM dropdown_width, COMBOBOX_ITEM dropdown_height) {
	uint8_t prev_selected = combobox->selectedItem;

	Setup_Combobox_Items(
	ctx, combobox->totalItems, &item_height, &dropdown_width, &dropdown_height);

	Setup_Combobox_Style(combobox, ctx);

	// How is the alignment/padding of the combobox button set?

	// BUT this one also screws other things up,
	// like making the main text disappear if the padding
	// is any value above 1, so maybe it is affecting the main button
	// but just not correctly?

	// Actually nope, it seems to be a different issue...

	//ctx->style.button.padding = nk_vec2(1, 0); //NOPE
	//ctx->style.combo.button.padding = nk_vec2(0, 10); NUH-UH
	//ctx->style.menu_button.padding = nk_vec2(10, 0); NO
	//ctx->style.window.combo_padding = nk_vec2(10, 0); NNNOOOOOOOO
	//ctx->style.contextual_button.padding = nk_vec2(0, 0);

	nk_combobox(
	ctx, (const char**)combobox->items, combobox->totalItems,
	&combobox->selectedItem, item_height.size,
	nk_vec2(dropdown_width.size, dropdown_height.size));

	return prev_selected != combobox->selectedItem;
}

bool Render_COMBOBOX_With_Buffer(
COMBOBOX* combobox, struct nk_context* ctx, COMBOBOX_ITEM item_height,
COMBOBOX_ITEM dropdown_width, COMBOBOX_ITEM dropdown_height) {
	nk_label(ctx, "", 0);
	return Render_COMBOBOX(
	combobox, ctx, item_height, dropdown_width, dropdown_height);
}

void Delete_COMBOBOX(COMBOBOX* combobox) {
	for (uint8_t i = 0; i < combobox->totalItems; i++) {
		delete [] combobox->items[i];
	}
}

/*
GETTERS
*/

int Selected_Index_COMBOBOX(const COMBOBOX* combobox) {
	return combobox->selectedItem;
}
string Selected_Item_COMBOBOX(const COMBOBOX* combobox) {
	return string(combobox->items[combobox->selectedItem]);
}

/*
HELPERS
*/

void Setup_Combobox_Style(COMBOBOX* combobox, struct nk_context* ctx) {
	ctx->style.combo = combobox->style;
	ctx->style.window = combobox->dropdownWindowStyle;
	ctx->style.contextual_button = combobox->dropdownButtonStyle;
}

void Setup_Combobox_Items(
struct nk_context* ctx, uint8_t total_items, COMBOBOX_ITEM* item_height,
COMBOBOX_ITEM* dropdown_width, COMBOBOX_ITEM* dropdown_height) {
	struct nk_rect current_rect = nk_widget_bounds(ctx);
	Setup_Combobox_Item(item_height, current_rect.h);
	Setup_Combobox_Item(dropdown_width, current_rect.w);
	Setup_Combobox_Item(dropdown_height, item_height->size * total_items);
}

void Setup_Combobox_Item(COMBOBOX_ITEM* item, uint32_t inheritance_size) {
	if (item->type == COMBOBOX_ITEM_TYPE_INHERIT) {
		item->size = inheritance_size;
	}
}
