#ifndef NK_SCROLLBAR_STYLE_H_
#define NK_SCROLLBAR_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"

struct nk_style_scrollbar Nk_Scrollbar_Style();
struct nk_style_scrollbar Nk_Scrollbar_Style(
struct nk_color foreground, struct nk_color background);
void Set_Nk_Scrollbar_Style(struct nk_style_scrollbar*, struct nk_color,
	struct nk_color);
void Set_Nk_Scrollbar_Style_Defaults(struct nk_style_scrollbar*);

#endif
