#include "Nk_Button_Style.h"

struct nk_style_button Nk_Button_Style(struct nk_color btn_color,
struct nk_color txt_color, float rounding, nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style(&style, btn_color, txt_color, rounding, align);
	return style;
}
struct nk_style_button Nk_Button_Style(struct nk_color btn_normal,
struct nk_color btn_hover, struct nk_color btn_active,
struct nk_color txt_color, float rounding, nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style(&style, btn_normal, btn_hover, btn_active, txt_color,
		rounding, align);
	return style;
}
struct nk_style_button Nk_Button_Style_Padded(
struct nk_color btn_normal, struct nk_color btn_hover,
struct nk_color btn_active, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 image_padding,  struct nk_vec2 touch_padding, float rounding,
nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style_Padded(
	&style, btn_normal, btn_hover, btn_active, txt_color, padding,
	image_padding, touch_padding, rounding, align);
	return style;
}
struct nk_style_button Nk_Button_Style_Padded(
struct nk_color btn_color, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 image_padding, struct nk_vec2 touch_padding, float rounding,
nk_flags align) {
	return Nk_Button_Style_Padded(
	btn_color, lighter(btn_color), darker(btn_color), txt_color, padding,
	image_padding, touch_padding, rounding, align);
}
struct nk_style_button Nk_Button_Style(BUTTON_IMAGES* images, nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style(&style, images, align);
	return style;
}
struct nk_style_button Nk_Button_Style(BMP* normal, BMP* hover, BMP* active,
nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style(&style, normal, hover, active, align);
	return style;
}
struct nk_style_button Nk_Button_Style_Dormant(struct nk_color btn_color,
struct nk_color txt_color, float rounding, nk_flags align) {
	struct nk_style_button style;
	Set_Nk_Button_Style_Dormant(&style, btn_color, txt_color, rounding, align);
	return style;
}
struct nk_style_button Nk_Button_Style_Dormant_Padded(
struct nk_color btn_color, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 image_padding, struct nk_vec2 touch_padding, float rounding,
nk_flags align) {
	return Nk_Button_Style_Padded(
	btn_color, btn_color, btn_color, txt_color, padding, image_padding,
	touch_padding, rounding, align);
}
struct nk_style_button Nk_Button_Style_Dormant(BMP* image, nk_flags align) {
	return Nk_Button_Style(image, image, image, align);
}

/*
Color-based buttons
*/
void Set_Nk_Button_Style(struct nk_style_button* style, struct nk_color btn_color,
struct nk_color txt_color, float rounding, nk_flags align) {
    Set_Nk_Button_Style(
	style, btn_color, lighter(btn_color), darker(btn_color), txt_color,
	rounding, align);
 }
void Set_Nk_Button_Style(struct nk_style_button* style, struct nk_color btn_normal,
struct nk_color btn_hover, struct nk_color btn_active, struct nk_color txt_color,
float rounding, nk_flags align) {
    Set_Nk_Button_Style_Defaults(style);
    style->normal = nk_style_item_color(rb_flip(btn_normal));
    style->hover = nk_style_item_color(rb_flip(btn_hover));
    style->active = nk_style_item_color(rb_flip(btn_active));
    style->text_normal = txt_color;
    style->text_hover = txt_color;
    style->text_active = txt_color;
    style->text_alignment = align;
    style->rounding = rounding;
}
// Overloads with padding
void Set_Nk_Button_Style_Padded(
struct nk_style_button* style, struct nk_color btn_color,
struct nk_color txt_color, struct nk_vec2 padding, struct nk_vec2 image_padding,
struct nk_vec2 touch_padding, float rounding, nk_flags align) {
	Set_Nk_Button_Style_Padded(
	style, btn_color, lighter(btn_color), darker(btn_color), txt_color, padding,
	image_padding, touch_padding, rounding, align);
}
void Set_Nk_Button_Style_Padded(
struct nk_style_button* style, struct nk_color btn_normal,
struct nk_color btn_hover, struct nk_color btn_active,
struct nk_color txt_color, struct nk_vec2 padding, struct nk_vec2 image_padding,
struct nk_vec2 touch_padding, float rounding, nk_flags align) {
	Set_Nk_Button_Style(
	style, btn_normal, btn_hover, btn_active, txt_color, rounding, align);
	style->padding = padding;
	style->image_padding = image_padding;
	style->touch_padding = touch_padding;
}

/*
Dormant buttons
*/
void Set_Nk_Button_Style_Dormant(
struct nk_style_button* style, struct nk_color btn_color,
struct nk_color txt_color, float rounding, nk_flags align) {
	Set_Nk_Button_Style(
	style, btn_color, btn_color, btn_color, txt_color, rounding, align);
}
void Set_Nk_Button_Style_Dormant_Padded(
struct nk_style_button* style, struct nk_color btn_color,
struct nk_color txt_color, struct nk_vec2 padding, struct nk_vec2 image_padding,
struct nk_vec2 touch_padding, float rounding, nk_flags align) {
	Set_Nk_Button_Style_Padded(
	style, btn_color, btn_color, btn_color, txt_color, padding, image_padding,
	touch_padding, rounding, align);
}

/*
Image based buttons
*/
void Set_Nk_Button_Style(
struct nk_style_button* style, BUTTON_IMAGES* images, nk_flags align) {
	Set_Nk_Button_Style(
	style, &images->normal, &images->hover, &images->active, align);
}
void Set_Nk_Button_Style(
struct nk_style_button* style, BMP* normal, BMP* hover, BMP* active,
nk_flags align) {
	Set_Nk_Button_Style(style, normal, hover, active, TRANSPARENT, align);
}
void Set_Nk_Button_Style(
struct nk_style_button* style, BMP* normal, BMP* hover, BMP* active,
struct nk_color txt_color, nk_flags align) {
    Set_Nk_Button_Style_Defaults(style);
    style->normal = nk_style_item_image(nk_image_ptr(normal));
    style->hover = nk_style_item_image(nk_image_ptr(hover));
    style->active = nk_style_item_image(nk_image_ptr(active));
    style->text_normal = txt_color;
    style->text_hover = txt_color;
    style->text_active = txt_color;
    style->text_alignment = align;
}

void Set_Nk_Button_Style_Defaults(struct nk_style_button* style) {
	style->normal = nk_style_item_color(rb_flip(MED_BLUE));
	style->hover = nk_style_item_color(lighter(rb_flip(MED_BLUE)));
	style->active = nk_style_item_color(darker(rb_flip(MED_BLUE)));
	style->border_color = TRANSPARENT;

	style->text_background = TRANSPARENT;
	style->text_normal = LIGHT_GRAY;
	style->text_hover = LIGHT_GRAY;
	style->text_active = LIGHT_GRAY;
	style->text_alignment = NK_TEXT_CENTERED;

	style->border = 0;
	style->rounding = 0;
	style->padding = nk_vec2(0, 0);
	style->image_padding = nk_vec2(0, 0);
	style->touch_padding = nk_vec2(0, 0);

	style->userdata = nk_handle_ptr(NULL);
	style->draw_begin = NULL;
	style->draw_end = NULL;
}
