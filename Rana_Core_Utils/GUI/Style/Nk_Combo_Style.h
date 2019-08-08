#ifndef NK_COMBO_STYLE_H_
#define NK_COMBO_STYLE_H_

#include "../NK_BASE.h"
#include "Nk_Button_Style.h"
#include "palette.h"

#define DEFAULT_NK_COMBO_BUTTON_SYMBOL NK_SYMBOL_RECT_SOLID

struct nk_style_combo Nk_Combo_Style(
struct nk_color background, struct nk_color foreground);
struct nk_style_combo Nk_Combo_Style(
struct nk_color background, struct nk_color foreground, struct nk_vec2 padding);

void Set_Nk_Combo_Style(
struct nk_style_combo*, struct nk_color background, struct nk_color foreground);
void Set_Nk_Combo_Style(
struct nk_style_combo*, struct nk_color background, struct nk_color foreground,
struct nk_vec2 padding);

void Set_Nk_Combo_Style_Defaults(struct nk_style_combo*);

#endif
