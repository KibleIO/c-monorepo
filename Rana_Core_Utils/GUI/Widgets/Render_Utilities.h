#ifndef RENDER_UTILITIES_H_
#define RENDER_UTILITIES_H_

#include "../NK_BASE.h"
#include "../Style/palette.h"
#include "../Layout/Rect_Transform.h"

void Fill_Current_Rect(
struct nk_context*, float rounding, struct nk_color,
RECT_TRANSFORM = NO_CHANGE_RECT_TRANSFORM);
void Fill_Current_Rect_With_Buffer(
struct nk_context*, float rounding, struct nk_color,
RECT_TRANSFORM = NO_CHANGE_RECT_TRANSFORM);

void Fill_Circle_In_Current_Rect(
struct nk_context*, struct nk_color, RECT_TRANSFORM = NO_CHANGE_RECT_TRANSFORM);
void Fill_Circle_In_Current_Rect_With_Buffer(
struct nk_context*, struct nk_color, RECT_TRANSFORM = NO_CHANGE_RECT_TRANSFORM);

#endif
