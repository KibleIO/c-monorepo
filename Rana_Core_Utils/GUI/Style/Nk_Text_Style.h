#ifndef NK_TEXT_STYLE_H_
#define NK_TEXT_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"

/*
LEGACY
*/

struct nk_style_text Nk_Text_Style(struct nk_color);

void Set_Nk_Text_Style(struct nk_style_text*, struct nk_color);
void Set_Nk_Text_Style_Defaults(struct nk_style_text*);

/*
EXPERIMENTAL
*/

static const struct nk_color DEFAULT_TEXT_COLOR = MED_BLUE;
static const struct nk_vec2 DEFAULT_TEXT_PADDING = nk_vec2(0,0);

struct nk_style_text Nk_Style_Text(
struct nk_color = DEFAULT_TEXT_COLOR, struct nk_vec2 = DEFAULT_TEXT_PADDING);

#endif
