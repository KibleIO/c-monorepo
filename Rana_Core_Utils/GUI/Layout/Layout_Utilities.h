#ifndef LAYOUT_UTILITIES_H_
#define LAYOUT_UTILITIES_H_

#include <cmath>
#include "../GUI.h"

enum alignment {
	// Align the child so that it ends where the parent begins
	ALIGN_BEGIN_OUTER,
	// Align the child so that it starts where the parent starts
	ALIGN_BEGIN_INNER,
	// Align the child so it starts midway inside the parent
	ALIGN_CENTER,
	// Align the child so it ends where the parent ends
	ALIGN_END_INNER,
	// Align the child so it begins where the parent ends
	ALIGN_END_OUTER
};

// Get the x-y coordinates of the top left corner of a rectangle
// with the given alignment inside of a parent rect
struct nk_vec2 Align_2D(
int parent_x, int parent_y, int parent_w, int parent_h, int child_w,
int child_h, alignment horizontal_align, alignment vertical_align);

// Overload takes rect and vec2 as arguments
struct nk_vec2 Align_2D(
struct nk_rect parent_rect, struct nk_vec2 child_size,
alignment horizontal_align, alignment vertical_align);

// Get the starting point of a line aligned within a parent line
// _C = "C"oordinates of the parent line are given
// _R = start and magnitude of the parent line are given
//		(analogous to "P"olar coordinates)
int Align_1D_C(
int parent_start, int parent_end, int child_magnitude, alignment align);
int Align_1D_P(
int parent_start, int parent_magnitude, int child_magnitude, alignment align);

// Overloads take nk_vec2 as arguments
int Align_1D_C(
struct nk_vec2 coords, int child_magnitude, alignment align);
int Align_1D_P(
struct nk_vec2 start_dir, int child_magnitude, alignment align);

#endif
