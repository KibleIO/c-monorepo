#ifndef NK_BUTTON_STYLE_H_
#define NK_BUTTON_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"
#include "Structs/Style_Structs.h"

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

static const char NK_STYLE_BUTTON_BACKGROUND_CODE = 'B';
static const char NK_STYLE_BUTTON_TEXT_CODE = 'T';
static const char NK_STYLE_BUTTON_BORDER_CODE = 'b';
static const char NK_STYLE_BUTTON_ROUNDING_CODE = 'r';
static const char NK_STYLE_BUTTON_PADDING_CODE = 'p';
static const char NK_STYLE_BUTTON_IMAGE_PADDING_CODE = 'i';
static const char NK_STYLE_BUTTON_TOUCH_PADDING_CODE = 't';
static const char NK_STYLE_BUTTON_USER_DATA_CODE = 'u';

struct NK_STYLE_BUTTON_DEFAULTS {
	static STYLE_ITEM_TRIO background;
	static TEXT_DATA text;
	static BORDER border;
	static float rounding;
	static struct nk_vec2 padding;
	static struct nk_vec2 image_padding;
	static struct nk_vec2 touch_padding;
	static USER_CALLBACK callback;
};

void Set_Nk_Style_Button_Defaults_Code(
const char* code, uint8_t num_args, /* encoded args */...);
void Set_Nk_Style_Button_Defaults(
STYLE_ITEM_TRIO, TEXT_DATA, BORDER, float rounding, struct nk_vec2 padding,
struct nk_vec2 image_padding, struct nk_vec2 touch_padding,
USER_CALLBACK callback);

struct nk_style_button Nk_Style_Button_Code(
const char* code, uint8_t num_args, /* encoded args */...);
struct nk_style_button Nk_Style_Button_Default();
struct nk_style_button Nk_Style_Button(
STYLE_ITEM_TRIO, TEXT_DATA, BORDER, float rounding, struct nk_vec2 padding,
struct nk_vec2 image_padding, struct nk_vec2 touch_padding,
USER_CALLBACK callback);

#endif
