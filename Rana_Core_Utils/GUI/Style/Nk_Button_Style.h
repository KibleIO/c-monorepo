#ifndef NK_BUTTON_STYLE_H_
#define NK_BUTTON_STYLE_H_

#include "../GUI.h"
#include "palette.h"
#include "Button_Images.h"

struct nk_style_button Nk_Button_Style(struct nk_color btn_color,
	struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style(struct nk_color btn_normal,
	struct nk_color btn_hover, struct nk_color btn_active,
	struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style(struct nk_color btn_normal,
	struct nk_color btn_hover, struct nk_color btn_active,
	struct nk_color txt_color, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style(struct nk_color btn_color,
	struct nk_color txt_color, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style(BUTTON_IMAGES*,
	nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style(BMP* normal, BMP* hover, BMP* active,
	nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style_Dormant(struct nk_color btn_color,
	struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style_Dormant(struct nk_color btn_color,
	struct nk_color txt_color, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style_Dormant(BMP* image,
	nk_flags align = NK_TEXT_CENTERED);

/*
Color-based buttons
*/
// Defaults the hover color to a slightly lighter color,
// and defaults the clicked color to a slightly darker color
void Set_Nk_Button_Style(struct nk_style_button*, struct nk_color btn_color,
	struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(struct nk_style_button*, struct nk_color btn_normal,
	struct nk_color btn_hover, struct nk_color btn_active,
	struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);

// Overloads w/ padding
void Set_Nk_Button_Style(struct nk_style_button*, struct nk_color btn_color,
	struct nk_color txt_color, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(struct nk_style_button*, struct nk_color btn_normal,
	struct nk_color btn_hover, struct nk_color btn_active,
	struct nk_color txt_color, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);

// Style for a dormant button. Colors to not change with user input,
// implying that the button does nothing
void Set_Nk_Button_Style_Dormant(struct nk_style_button*,
	struct nk_color btn_color, struct nk_color txt_color, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style_Dormant(struct nk_style_button*,
	struct nk_color btn_color, struct nk_color txt_color,
	struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);

/*
Image-based buttons
*/
void Set_Nk_Button_Style(struct nk_style_button*, BUTTON_IMAGES*,
	nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(struct nk_style_button*, BMP* normal, BMP* hover,
	BMP* active, nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(struct nk_style_button*, BMP* normal, BMP* hover,
	BMP* active, struct nk_color txt_color, nk_flags align = NK_TEXT_CENTERED);

// Overloads with padding
void Set_Nk_Button_Style(struct nk_style_button*, BMP* normal, BMP* hover,
	BMP* active, struct nk_vec2 padding, float rounding = 0,
	nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(struct nk_style_button*, BMP* normal, BMP* hover,
	BMP* active, struct nk_color txt_color, struct nk_vec2 padding,
	float rounding = 0, nk_flags align = NK_TEXT_CENTERED);

// Set default values on the button style
void Set_Nk_Button_Style_Defaults(struct nk_style_button*);

#endif
