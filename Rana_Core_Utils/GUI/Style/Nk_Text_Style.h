#ifndef NK_TEXT_STYLE_H_
#define NK_TEXT_STYLE_H_

#include "../GUI.h"
#include "palette.h"

struct nk_style_text Nk_Text_Style(struct nk_color);

void Set_Nk_Text_Style(struct nk_style_text*, struct nk_color);
void Set_Nk_Text_Style_Defaults(struct nk_style_text*);

#endif
