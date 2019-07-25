#ifndef RECT_TRANSFORM_H_
#define RECT_TRANSFORM_H_

#include "../GUI.h"
#include "Alignment.h"

struct RECT_TRANSFORM {
	float widthCompression;
	float heightCompression;
	alignment horizontalAlignment;
	alignment verticalAlignment;
};

// Functions
RECT_TRANSFORM Rect_Transform(
float width_compression, float height_compression, alignment horizontal_align,
alignment vertical_align);
RECT_TRANSFORM Rect_Transform(
float compression, alignment horizontal_align, alignment vertical_align);

RECT_TRANSFORM Horizontal_Transform(
float width_compression, alignment horizontal_align);
RECT_TRANSFORM Vertical_Transform(
float height_compression, alignment vertical_align);

RECT_TRANSFORM Center_Transform(
float width_compression, float height_compression);
RECT_TRANSFORM Center_Transform(float compression);

// Expands/compresses the parent rect, then aligns the result inside the parent
struct nk_rect Transform_Rect(struct nk_rect parent, RECT_TRANSFORM trans);

// Constants
static const RECT_TRANSFORM NO_CHANGE_RECT_TRANSFORM = Rect_Transform(
1, ALIGN_CENTER, ALIGN_CENTER);

#endif
