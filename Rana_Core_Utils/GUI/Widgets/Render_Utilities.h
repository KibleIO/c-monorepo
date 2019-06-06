#ifndef RENDER_UTILITIES_H_
#define RENDER_UTILITIES_H_

#include "../GUI.h"
#include "../Style/palette.h"

void Fill_Current_Rect(struct nk_context*, float rounding, struct nk_color);
void Fill_Current_Rect_With_Buffer(
struct nk_context*, float rounding, struct nk_color);

#endif
