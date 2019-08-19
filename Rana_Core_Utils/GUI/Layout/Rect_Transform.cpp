#include "Rect_Transform.h"

RECT_TRANSFORM Rect_Transform(
SIZE_TRANSFORM size_transform, ALIGNMENT_2D alignment) {
	RECT_TRANSFORM rect_trans;
	rect_trans.sizeTransform = size_transform;
	rect_trans.alignment = alignment;
	return rect_trans;
}
RECT_TRANSFORM Rect_Transform(
RESIZE_RATIO resize, ALIGNMENT_2D alignment) {
	return Rect_Transform(Size_Transform(resize, resize), alignment);
}

RECT_TRANSFORM Horizontal_Transform(
RESIZE_RATIO width_resize, ALIGNMENT_1D horizontal_align) {
	return Rect_Transform(
	Size_Transform(width_resize, Ratio_Of_Parent(1)), Alignment_2D(
	horizontal_align, ALIGNMENT_MIDDLE));
}
RECT_TRANSFORM Vertical_Transform(
RESIZE_RATIO height_resize, ALIGNMENT_1D vertical_align) {
	return Rect_Transform(
	Size_Transform(Ratio_Of_Parent(1), height_resize), Alignment_2D(
	ALIGNMENT_MIDDLE, vertical_align));
}

RECT_TRANSFORM Center_Transform(SIZE_TRANSFORM transform) {
	return Rect_Transform(transform, ALIGNMENT_CENTER);
}
RECT_TRANSFORM Center_Transform(RESIZE_RATIO resize) {
	return Center_Transform(Size_Transform(resize, resize));
}

struct nk_rect Transform_Rect(struct nk_rect parent, RECT_TRANSFORM trans) {
	struct nk_vec2 child_size = Transform_Size(
	nk_rect_size(parent), trans.sizeTransform);

	return nk_recta(
	Align_2D(parent, child_size, trans.alignment), child_size);
}
