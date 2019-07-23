#ifndef RENDER_UTILITIES_H_
#define RENDER_UTILITIES_H_

#include "../GUI.h"
#include "../Style/palette.h"
#include "../Layout/Layout_Utilities.h"

void Fill_Current_Rect(
struct nk_context*, float rounding, struct nk_color, float compression = 1.0,
alignment horizontal_align = ALIGN_CENTER,
alignment vertical_align = ALIGN_CENTER);
void Fill_Current_Rect_With_Buffer(
struct nk_context*, float rounding, struct nk_color, float compression = 1.0,
alignment horizontal_align = ALIGN_CENTER,
alignment vertical_align = ALIGN_CENTER);

void Fill_Circle_In_Current_Rect(
struct nk_context*, struct nk_color, float compression = 1.0,
alignment horizontal_align = ALIGN_CENTER,
alignment vertical_align = ALIGN_CENTER);
void Fill_Circle_In_Current_Rect_With_Buffer(
struct nk_context*, struct nk_color, float compression = 1.0,
alignment horizontal_align = ALIGN_CENTER,
alignment vertical_align = ALIGN_CENTER);

#endif
