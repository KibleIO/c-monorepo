#ifndef SIZE_TRANSFORM_H_
#define SIZE_TRANSFORM_H_

#include "../NK_BASE.h"
#include "../../Utilities/LOGGING.h"
#include "../../Utilities/utilities.h"

// RESIZE RATIO

enum RESIZE_RATIO_TYPE {
	RATIO_OF_PARENT_PARALLEL,
	RATIO_OF_PARENT_OPPOSITE,
	RATIO_OF_SELF_OPPOSITE
};

struct RESIZE_RATIO {
	float ratio;
	RESIZE_RATIO_TYPE type;
};

RESIZE_RATIO Ratio_Of_Parent(float);
RESIZE_RATIO Ratio_Of_Parent_Opposite(float);
RESIZE_RATIO Ratio_Of_Self_Opposite(float);

// SIZE TRANSFORM

struct SIZE_TRANSFORM {
	RESIZE_RATIO widthResize;
	RESIZE_RATIO heightResize;
};

SIZE_TRANSFORM Size_Transform(
RESIZE_RATIO width_resize, RESIZE_RATIO height_resize);
bool Size_Transform_Valid(
RESIZE_RATIO width_resize, RESIZE_RATIO height_resize);

// Apply the given transformation to the parent size, and return the new size
struct nk_vec2 Transform_Size(struct nk_vec2 parent_size, SIZE_TRANSFORM);

// Return the exact size, given the parent parellel and opposite dimensions
// If the resize is based on self opposite, return -1
float Resize_By_Parent(
RESIZE_RATIO, float parent_parellel, float parent_opposite);

#endif
