#include "Nk_Edit_Style.h"

struct nk_style_edit Nk_Edit_Style_Plain(struct nk_color background_color,
struct nk_color foreground_color) {
	struct nk_style_edit style;
	Set_Nk_Edit_Style_Plain(&style, background_color, foreground_color);
	return style;
}
struct nk_style_edit Nk_Edit_Style_Vibrant(struct nk_color background_color,
struct nk_color foreground_color) {
	struct nk_style_edit style;
	Set_Nk_Edit_Style_Vibrant(&style, background_color, foreground_color);
	return style;
}
struct nk_style_edit Nk_Edit_Style_Dead(struct nk_color background_color,
struct nk_color foreground_color) {
	struct nk_style_edit style;
	Set_Nk_Edit_Style_Dead(&style, background_color, foreground_color);
	return style;
}
struct nk_style_edit Nk_Edit_Style_Plain(struct nk_color background_color,
struct nk_color cursor_color, struct nk_color text_color,
struct nk_color text_color_selected) {
	struct nk_style_edit style;
	Set_Nk_Edit_Style_Plain(&style, background_color, cursor_color, text_color,
		text_color_selected);
	return style;
}
struct nk_style_edit Nk_Edit_Style_Vibrant(struct nk_color background_color,
struct nk_color cursor_color, struct nk_color text_color,
struct nk_color text_color_selected) {
	struct nk_style_edit style;
	Set_Nk_Edit_Style_Vibrant(&style, background_color, cursor_color, text_color,
		text_color_selected);
	return style;
}
void Set_Nk_Edit_Style_Plain(struct nk_style_edit* style,
struct nk_color background_color, struct nk_color foreground_color) {
	Set_Nk_Edit_Style_Plain(style, background_color, foreground_color,
		foreground_color, background_color);
}
void Set_Nk_Edit_Style_Vibrant(struct nk_style_edit* style,
struct nk_color background_color, struct nk_color foreground_color) {
	Set_Nk_Edit_Style_Vibrant(style, background_color, foreground_color,
		foreground_color, background_color);
}
void Set_Nk_Edit_Style_Plain(struct nk_style_edit* style,
struct nk_color background_color, struct nk_color cursor_color,
struct nk_color text_color, struct nk_color text_color_selected) {
	Set_Nk_Edit_Style_Defaults(style);

	// Background color
	style->normal = nk_style_item_color((background_color));
	style->hover = nk_style_item_color((background_color));
	style->active = nk_style_item_color((background_color));

	// Cursor color
	style->cursor_normal = (cursor_color);
	style->cursor_hover = (cursor_color);
	// Text color while under the cursor - same as when selected
	style->cursor_text_normal = text_color_selected;
	style->cursor_text_hover = text_color_selected;

	// Text color while not selected
	style->text_normal = text_color;
	style->text_hover = text_color;
	style->text_active = text_color;

	// Rect color while selecting - same as cursor color
	style->selected_normal = (cursor_color);
	style->selected_hover = (cursor_color);
	// Text color while selected
	style->selected_text_normal = text_color_selected;
	style->selected_text_hover = text_color_selected;
}
void Set_Nk_Edit_Style_Vibrant(struct nk_style_edit* style,
struct nk_color background_color, struct nk_color cursor_color,
struct nk_color text_color, struct nk_color text_color_selected) {
	Set_Nk_Edit_Style_Defaults(style);

	// Background color
	style->normal = nk_style_item_color((background_color));
	style->hover = nk_style_item_color(lighter((background_color)));
	style->active = nk_style_item_color(darker((background_color)));

	// Cursor color
	style->cursor_normal = (cursor_color);
	style->cursor_hover = lighter((cursor_color));
	// Text color while under the cursor - same as when selected
	style->cursor_text_normal = text_color_selected;
	style->cursor_text_hover = lighter(text_color_selected);

	// Text color while not selected
	style->text_normal = text_color;
	style->text_hover = lighter(text_color);
	style->text_active = darker(text_color);

	// Rect color while selecting - same as cursor color
	style->selected_normal = (cursor_color);
	style->selected_hover = (cursor_color);
	// Text color while selected
	style->selected_text_normal = text_color_selected;
	style->selected_text_hover = text_color_selected;
}
void Set_Nk_Edit_Style_Dead(struct nk_style_edit* style,
struct nk_color background_color, struct nk_color foreground_color) {
	Set_Nk_Edit_Style_Plain(style, background_color, background_color,
		foreground_color, foreground_color);
}
void Set_Nk_Edit_Style_Defaults(struct nk_style_edit* style) {
	style->border = 0;
	style->rounding = 0;
	style->cursor_size = 5;
	style->scrollbar_size = nk_vec2(0, 0);
	style->padding = nk_vec2(10, 0);
	style->row_padding = 2;
}
