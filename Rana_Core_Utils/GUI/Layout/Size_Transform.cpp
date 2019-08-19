#include "Size_Transform.h"

// RESIZE_RATIO

RESIZE_RATIO Ratio_Of_Parent(float ratio) {
	RESIZE_RATIO resize_ratio;
	resize_ratio.ratio = ratio;
	resize_ratio.type = RATIO_OF_PARENT_PARALLEL;
	return resize_ratio;
}
RESIZE_RATIO Ratio_Of_Parent_Opposite(float ratio) {
	RESIZE_RATIO resize_ratio;
	resize_ratio.ratio = ratio;
	resize_ratio.type = RATIO_OF_PARENT_OPPOSITE;
	return resize_ratio;
}
RESIZE_RATIO Ratio_Of_Self_Opposite(float ratio) {
	RESIZE_RATIO resize_ratio;
	resize_ratio.ratio = ratio;
	resize_ratio.type = RATIO_OF_SELF_OPPOSITE;
	return resize_ratio;
}

// SIZE TRANSFORM

SIZE_TRANSFORM Size_Transform(
RESIZE_RATIO width_resize, RESIZE_RATIO height_resize) {
	if (!Size_Transform_Valid(width_resize, height_resize)) {
		log_err(string("Cannot construct a size transform where both ") +
		" dimensions are ratios of each other");
		return {};
	}

	SIZE_TRANSFORM transform;
	transform.widthResize = width_resize;
	transform.heightResize = height_resize;
	return transform;
}

bool Size_Transform_Valid(
RESIZE_RATIO width_resize, RESIZE_RATIO height_resize) {
	return width_resize.type != RATIO_OF_SELF_OPPOSITE ||
	height_resize.type != RATIO_OF_SELF_OPPOSITE;
}

struct nk_vec2 Transform_Size(
struct nk_vec2 parent_size, SIZE_TRANSFORM transform) {
	struct nk_vec2 new_size = nk_vec2(
	Resize_By_Parent(transform.widthResize, parent_size.x, parent_size.y),
	Resize_By_Parent(transform.heightResize, parent_size.y, parent_size.x));

	// Check the new size for ratios of opposites
	if (new_size.x < 0.0f && new_size.y >= 0.0f) {
		new_size.x = new_size.y * transform.widthResize.ratio;
	}
	else if (new_size.x >= 0.0f && new_size.y < 0.0f) {
		new_size.y = new_size.x * transform.heightResize.ratio;
	}
	else if (new_size.x < 0.0f && new_size.y < 0.0f) {
		log_err(string("Cannot transform a size where both ") +
		"dimensions are ratios of each other");
		new_size = nk_vec2(0, 0);
	}

	return new_size;
}

float Resize_By_Parent(
RESIZE_RATIO resize, float parent_parellel, float parent_opposite) {
	switch (resize.type) {
		case RATIO_OF_PARENT_PARALLEL:	return resize.ratio * parent_parellel;
		case RATIO_OF_PARENT_OPPOSITE:	return resize.ratio * parent_opposite;
		default:						return -1.0;
	}
}
