#ifndef NK_BUTTON_STYLE_H_
#define NK_BUTTON_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"
#include "Style_Utils.h"

/*
LEGACY
*/

struct nk_style_button Nk_Button_Style(
struct nk_color btn_color, struct nk_color txt_color, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style(
struct nk_color btn_normal, struct nk_color btn_hover,
struct nk_color btn_active, struct nk_color txt_color, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style_Padded(
struct nk_color btn_normal, struct nk_color btn_hover,
struct nk_color btn_active, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 imag_padding, struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style_Padded(
struct nk_color btn_color, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 imag_padding, struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style(
struct nk_image normal, struct nk_image hover, struct nk_image active,
nk_flags align = NK_TEXT_CENTERED);

struct nk_style_button Nk_Button_Style_Dormant(
struct nk_color btn_color, struct nk_color txt_color, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style_Dormant_Padded(
struct nk_color btn_color, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 imag_padding, struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
struct nk_style_button Nk_Button_Style_Dormant(
struct nk_image image, nk_flags align = NK_TEXT_CENTERED);

/*
Color-based buttons
*/
// Defaults the hover color to a slightly lighter color,
// and defaults the clicked color to a slightly darker color
void Set_Nk_Button_Style(
struct nk_style_button*, struct nk_color btn_color, struct nk_color txt_color,
float rounding = 0, nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(
struct nk_style_button*, struct nk_color btn_normal, struct nk_color btn_hover,
struct nk_color btn_active, struct nk_color txt_color, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);

// Overloads w/ padding
void Set_Nk_Button_Style_Padded(
struct nk_style_button*, struct nk_color btn_color, struct nk_color txt_color,
struct nk_vec2 padding, struct nk_vec2 imag_padding,
struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style_Padded(
struct nk_style_button*, struct nk_color btn_normal, struct nk_color btn_hover,
struct nk_color btn_active, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 imag_padding, struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);


// Style for a dormant button. Colors to not change with user input,
// implying that the button does nothing
void Set_Nk_Button_Style_Dormant(
struct nk_style_button*, struct nk_color btn_color, struct nk_color txt_color,
float rounding = 0, nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style_Dormant_Padded(
struct nk_style_button*, struct nk_color btn_color, struct nk_color txt_color,
struct nk_vec2 padding, struct nk_vec2 image_padding,
struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);

/*
Image-based buttons
*/
void Set_Nk_Button_Style(
struct nk_style_button*, struct nk_image normal, struct nk_image hover,
struct nk_image active, nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style(
struct nk_style_button*, struct nk_image normal, struct nk_image hover,
struct nk_image active,
struct nk_color txt_color, nk_flags align = NK_TEXT_CENTERED);

// Overloads with padding
void Set_Nk_Button_Style_Padded(
struct nk_style_button*, struct nk_image* normal, struct nk_image hover,
struct nk_image active,
struct nk_vec2 padding, struct nk_vec2 imag_padding,
struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);
void Set_Nk_Button_Style_Padded(
struct nk_style_button*, struct nk_image normal, struct nk_image hover,
struct nk_image active, struct nk_color txt_color, struct nk_vec2 padding,
struct nk_vec2 imag_padding, struct nk_vec2 touch_padding, float rounding = 0,
nk_flags align = NK_TEXT_CENTERED);

// Set default values on the button style
void Set_Nk_Button_Style_Defaults(struct nk_style_button*);

/*
EXPERIMENTAL
*/

static const STYLE_ITEM_TRIO DEFAULT_BUTTON_BACKGROUND = Style_Item_Trio_Color(
Color_Trio_Autodiff(MED_BLUE));
static const struct nk_color DEFAULT_BUTTON_BORDER_COLOR = LIGHT_BLUE;
static const struct nk_color DEFAULT_BUTTON_TEXT_BACKGROUND_COLOR = TRANSPARENT;
static const COLOR_TRIO DEFAULT_BUTTON_TEXT_COLOR = Color_Trio_Uniform(
LIGHT_BLUE);
static const nk_flags DEFAULT_BUTTON_TEXT_ALIGNMENT = NK_TEXT_CENTERED;
static const float DEFAULT_BUTTON_BORDER = 0;
static const float DEFAULT_BUTTON_ROUNDING = 10;
static const struct nk_vec2 DEFAULT_BUTTON_PADDING = nk_vec2(0, 0);
static const struct nk_vec2 DEFAULT_BUTTON_IMAGE_PADDING = nk_vec2(0, 0);
static const struct nk_vec2 DEFAULT_BUTTON_TOUCH_PADDING = nk_vec2(0, 0);
static const USER_CALLBACK DEFAULT_BUTTON_CALLBACK = User_Callback_Empty();

// struct nk_style_button Nk_Style_Button(
// STYLE_ITEM_TRIO = DEFAULT_BUTTON_BACKGROUND, )

#endif
