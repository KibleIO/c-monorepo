#include "Layout_Utilities.h"

// PANEL SIZING
float Current_Usable_Panel_Height(const struct nk_context* ctx) {
	const struct nk_panel* panel = ctx->current->layout;

	// Return the usable height of the current panel
	return Usable_Panel_Height(&ctx->style, panel->type, panel->bounds.h);
}

#include "../../Utilities/utilities.h"

float Current_Usable_Panel_Width(const struct nk_context* ctx) {
	const struct nk_panel* panel = ctx->current->layout;

	// Return the usable height of the current panel
	return Usable_Panel_Width(&ctx->style, panel->type, panel->bounds.w);
}

float Remaining_Usable_Panel_Height(const struct nk_context* ctx) {
	const struct nk_panel* panel = ctx->current->layout;

	return Current_Usable_Panel_Height(ctx) -
	abs(panel->bounds.y - (panel->at_y + panel->row.height));
}

float Current_Row_Height(const struct nk_context* ctx) {
	return ctx->current->layout->row.height;
}

float Usable_Panel_Height(const struct nk_style* style,
enum nk_panel_type type, int total_height) {
	// Get the padding on the given panel's type in the style struct
	struct nk_vec2 padding = Get_Panel_Padding(style, type);
	return total_height - (2 * padding.y);
}

float Usable_Panel_Width(const struct nk_style* style,
enum nk_panel_type type, int total_width) {
	// Get the padding on the given panel's type in the style struct
	struct nk_vec2 padding = Get_Panel_Padding(style, type);
	return total_width - (2 * padding.x);
}

struct nk_vec2 Get_Panel_Padding(
const struct nk_style* style, enum nk_panel_type type) {
	switch (type) {
		default:
	    case NK_PANEL_WINDOW: 		return style->window.padding;
	    case NK_PANEL_GROUP: 		return style->window.group_padding;
	    case NK_PANEL_POPUP: 		return style->window.popup_padding;
	    case NK_PANEL_CONTEXTUAL: 	return style->window.contextual_padding;
	    case NK_PANEL_COMBO: 		return style->window.combo_padding;
	    case NK_PANEL_MENU: 		return style->window.menu_padding;
	    case NK_PANEL_TOOLTIP: 		return style->window.menu_padding;
	}
}

float Get_Panel_Border(const struct nk_style* style, enum nk_panel_type type) {
	switch (type) {
		default:
		case NK_PANEL_WINDOW: 		return style->window.border;
		case NK_PANEL_GROUP: 		return style->window.group_border;
		case NK_PANEL_POPUP: 		return style->window.popup_border;
		case NK_PANEL_CONTEXTUAL: 	return style->window.contextual_border;
		case NK_PANEL_COMBO: 		return style->window.combo_border;
		case NK_PANEL_MENU: 		return style->window.menu_border;
		case NK_PANEL_TOOLTIP: 		return style->window.menu_border;
	}
}

// MISC
struct nk_rect Compressed_Rect(struct nk_rect r, float compression) {
	float width = r.w * compression;
	float height = r.h * compression;
	return nk_rect(r.x + (width / 2.0), r.y + (height / 2.0), width, height);
}
