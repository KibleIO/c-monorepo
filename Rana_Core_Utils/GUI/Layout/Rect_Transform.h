#ifndef RECT_TRANSFORM_H_
#define RECT_TRANSFORM_H_

#include "../NK_BASE.h"
#include "Alignment.h"
#include "Size_Transform.h"

struct RECT_TRANSFORM {
	SIZE_TRANSFORM sizeTransform;
	ALIGNMENT_2D alignment;
};

// Functions
RECT_TRANSFORM Rect_Transform(
SIZE_TRANSFORM size_transform, ALIGNMENT_2D align);
RECT_TRANSFORM Rect_Transform(RESIZE_RATIO resize, ALIGNMENT_2D align);

RECT_TRANSFORM Horizontal_Transform(
RESIZE_RATIO width_resize, ALIGNMENT_1D horizontal_align);
RECT_TRANSFORM Vertical_Transform(
RESIZE_RATIO height_resize, ALIGNMENT_1D vertical_align);

RECT_TRANSFORM Center_Transform(
SIZE_TRANSFORM sizeTransform);
RECT_TRANSFORM Center_Transform(RESIZE_RATIO compression);

// Resizes the parent rect, then aligns the result inside the parent
struct nk_rect Transform_Rect(struct nk_rect parent, RECT_TRANSFORM trans);

// Constants
static const RECT_TRANSFORM NO_CHANGE_RECT_TRANSFORM = Rect_Transform(
Size_Transform(Ratio_Of_Parent(1), Ratio_Of_Parent(1)), ALIGNMENT_CENTER);

#endif
