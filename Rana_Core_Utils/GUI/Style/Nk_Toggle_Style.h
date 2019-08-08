#ifndef NK_TOGGLE_STYLE_H_
#define NK_TOGGLE_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"

#define DEFAULT_TOGGLE_PADDING 3
#define DEFAULT_TOGGLE_SPACING 10

struct nk_style_toggle Nk_Toggle_Style(struct nk_color background,
	struct nk_color check, struct nk_color text);
void Set_Nk_Toggle_Style(struct nk_style_toggle*, struct nk_color background,
	struct nk_color check, struct nk_color text);
void Set_Nk_Toggle_Style_Defaults(struct nk_style_toggle*);

#endif
