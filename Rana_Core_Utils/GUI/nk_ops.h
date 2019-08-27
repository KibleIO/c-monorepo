#ifndef NK_OPS_H_
#define NK_OPS_H_

#include "NK_BASE.h"
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

bool Vec2_Cmp(struct nk_vec2, struct nk_vec2);

string Rect_Str(struct nk_rect);
string Vec2_Str(struct nk_vec2);
string Color_Str(struct nk_color);

#endif
