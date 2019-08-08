#ifndef NK_EDIT_STYLE_H_
#define NK_EDIT_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"

struct nk_style_edit Nk_Edit_Style_Plain(struct nk_color background_color,
	struct nk_color foreground_color);
struct nk_style_edit Nk_Edit_Style_Vibrant(struct nk_color background_color,
	struct nk_color foreground_color);
struct nk_style_edit Nk_Edit_Style_Dead(struct nk_color background_color,
	struct nk_color foreground_color);
struct nk_style_edit Nk_Edit_Style_Plain(struct nk_color background_color,
	struct nk_color cursor_color, struct nk_color text_color,
	struct nk_color text_color_selected);
struct nk_style_edit Nk_Edit_Style_Vibrant(struct nk_color background_color,
	struct nk_color cursor_color, struct nk_color text_color,
	struct nk_color text_color_selected);

// Shortcut style setting gets all colors from a pair of colors
// background_color = box background color and text while selected color
// foreground_color = cursor color, selection rectangle color, and text not
//		selected color
void Set_Nk_Edit_Style_Plain(struct nk_style_edit*,
	struct nk_color background_color, struct nk_color foreground_color);
void Set_Nk_Edit_Style_Vibrant(struct nk_style_edit*,
	struct nk_color background_color, struct nk_color foreground_color);
// Set style so that the edit box colors don't change
// when you hover and click on it
void Set_Nk_Edit_Style_Plain(struct nk_style_edit*,
	struct nk_color background_color, struct nk_color cursor_color,
	struct nk_color text_color, struct nk_color text_color_selected);
// Set style so that the edit box colors do change
// when you hover and click on it
void Set_Nk_Edit_Style_Vibrant(struct nk_style_edit*,
	struct nk_color background_color, struct nk_color cursor_color,
	struct nk_color text_color, struct nk_color text_color_selected);
// Set style so that the cursor and select rectangle are invisible,
// implying no input can be made
void Set_Nk_Edit_Style_Dead(struct nk_style_edit*,
	struct nk_color background_color, struct nk_color foreground_color);
void Set_Nk_Edit_Style_Defaults(struct nk_style_edit*);

#endif
