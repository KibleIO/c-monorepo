#ifndef NK_SLIDER_STYLE_H_
#define NK_SLIDER_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"
#include "Nk_Button_Style.h"

#define SLIDER_BAR_DEFAULT_BAR_HEIGHT 10
#define SLIDER_BAR_DEFAULT_CURSOR_SIZE nk_vec2(20, 20)

struct nk_style_slider Nk_Slider_Style(
struct nk_color bar_color, struct nk_color cursor);
struct nk_style_slider Nk_Slider_Style(
struct nk_color bar_color, struct nk_color bar_filled_color,
struct nk_color cursor);

void Set_Nk_Slider_Style(
struct nk_style_slider*, struct nk_color bar_color,
struct nk_color cursor);
void Set_Nk_Slider_Style(
struct nk_style_slider*, struct nk_color bar_color,
struct nk_color bar_filled_color, struct nk_color cursor);
void Set_Nk_Slider_Style_Defaults(struct nk_style_slider*);

#endif
