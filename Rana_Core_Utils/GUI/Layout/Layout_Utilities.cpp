#include "Layout_Utilities.h"

struct nk_vec2 Align_2D(
int parent_x, int parent_y, int parent_w, int parent_h, int child_w,
int child_h, alignment horizontal_align, alignment vertical_align) {
	return nk_vec2(
	Align_1D_P(parent_x, parent_w, child_w, horizontal_align),
	Align_1D_P(parent_y, parent_h, child_h, vertical_align));
}

struct nk_vec2 Align_2D(
struct nk_rect parent_rect, struct nk_vec2 child_size,
alignment horizontal_align, alignment vertical_align) {
	return Align_2D(
	parent_rect.x, parent_rect.y, parent_rect.w, parent_rect.h, child_size.x,
	child_size.y, horizontal_align, vertical_align);
}

int Align_1D_C(
int parent_start, int parent_end, int child_magnitude, alignment align) {
	return Align_1D_P(
	parent_start, abs(parent_end - parent_start), child_magnitude, align);
}
int Align_1D_P(
int parent_start, int parent_magnitude, int child_magnitude, alignment align) {
	switch(align) {
		case ALIGN_BEGIN_OUTER:
			return parent_start - child_magnitude;
		case ALIGN_BEGIN_INNER:
			return parent_start;
		case ALIGN_CENTER:
			return parent_start + ((parent_magnitude - child_magnitude) / 2);
		case ALIGN_END_INNER:
			return parent_start + (parent_magnitude - child_magnitude);
		case ALIGN_END_OUTER:
			return parent_start + parent_magnitude;
		default:
			return 0;
	}
}

int Align_1D_C(struct nk_vec2 coords, int child_magnitude, alignment align) {
	return Align_1D_C(coords.x, coords.y, child_magnitude, align);
}
int Align_1D_P(struct nk_vec2 start_dir, int child_magnitude, alignment align) {
	return Align_1D_P(start_dir.x, start_dir.y, child_magnitude, align);
}
