#include "Alignment.h"

ALIGNMENT_1D Horizontal_Alignment(ALIGNMENT_2D align) {
	return (ALIGNMENT_1D)(align >> 2);
}
ALIGNMENT_1D Vertical_Alignment(ALIGNMENT_2D align) {
	return (ALIGNMENT_1D)(align & 3);
}
ALIGNMENT_2D Alignment_2D(ALIGNMENT_1D horizontal, ALIGNMENT_1D vertical) {
	return (ALIGNMENT_2D)(horizontal << 2 | vertical);
}

int Align_1D(
int parent_begin, int parent_magnitude, int child_magnitude,
ALIGNMENT_1D align) {
	switch(align)
	{
		case ALIGNMENT_BEGIN:
			return parent_begin;
		case ALIGNMENT_MIDDLE:
			return parent_begin + ((parent_magnitude - child_magnitude) / 2);
		case ALIGNMENT_END:
			return parent_begin + (parent_magnitude - child_magnitude);
		default:
			return 0;
	}
}
struct nk_vec2 Align_2D(struct nk_rect parent, struct nk_vec2 child_dimensions,
ALIGNMENT_2D align) {
	return nk_vec2(
	Align_1D(parent.x, parent.w, child_dimensions.x,
	Horizontal_Alignment(align)), Align_1D(parent.y, parent.h,
	child_dimensions.y, Vertical_Alignment(align)));
}
