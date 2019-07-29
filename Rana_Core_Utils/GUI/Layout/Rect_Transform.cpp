#include "Rect_Transform.h"

RECT_TRANSFORM Rect_Transform(
float width_compression, float height_compression, alignment horizontal_align,
alignment vertical_align) {
	RECT_TRANSFORM rect_trans;
	rect_trans.widthCompression = width_compression;
	rect_trans.heightCompression = height_compression;
	rect_trans.horizontalAlignment = horizontal_align;
	rect_trans.verticalAlignment = vertical_align;
	return rect_trans;
}
RECT_TRANSFORM Rect_Transform(
float compression, alignment horizontal_align, alignment vertical_align) {
	return Rect_Transform(
	compression, compression, horizontal_align, vertical_align);
}

RECT_TRANSFORM Horizontal_Transform(
float width_compression, alignment horizontal_align) {
	return Rect_Transform(
	width_compression, 1.0, horizontal_align, ALIGN_CENTER);
}
RECT_TRANSFORM Vertical_Transform(
float height_compression, alignment vertical_align) {
	return Rect_Transform(
	1.0, height_compression, ALIGN_CENTER, vertical_align);
}

RECT_TRANSFORM Center_Transform(
float width_compression, float height_compression) {
	return Rect_Transform(
	width_compression, height_compression, ALIGN_CENTER, ALIGN_CENTER);
}
RECT_TRANSFORM Center_Transform(float compression) {
	return Center_Transform(compression, compression);
}

struct nk_rect Transform_Rect(struct nk_rect parent, RECT_TRANSFORM trans) {
	struct nk_vec2 child_size = nk_vec2(
	parent.w * trans.widthCompression, parent.h * trans.heightCompression);

	return nk_recta(
	Align_2D(parent, child_size, trans.horizontalAlignment,
	trans.verticalAlignment), child_size);
}
