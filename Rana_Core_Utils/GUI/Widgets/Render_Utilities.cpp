#include "Render_Utilities.h"

void Fill_Current_Rect(
struct nk_context* ctx, float rounding, struct nk_color color,
float compression, alignment horizontal_align, alignment vertical_align) {
	nk_fill_rect(
	nk_window_get_canvas(ctx), Compressed_Rect(nk_widget_bounds(ctx),
	compression, horizontal_align, vertical_align), rounding, rb_flip(color));
}

void Fill_Current_Rect_With_Buffer(
struct nk_context* ctx, float rounding, struct nk_color color,
float compression, alignment horizontal_align, alignment vertical_align) {
	nk_label(ctx, "", 0);
	Fill_Current_Rect(ctx, rounding, color, compression, horizontal_align,
	vertical_align);
}

void Fill_Circle_In_Current_Rect(
struct nk_context* ctx, struct nk_color color, float compression,
alignment horizontal_align, alignment vertical_align) {
	nk_fill_circle(
	nk_window_get_canvas(ctx), Compressed_Rect(nk_widget_bounds(ctx),
	compression, horizontal_align, vertical_align), color);
}

void Fill_Circle_In_Current_Rect_With_Buffer(
struct nk_context* ctx, struct nk_color color, float compression,
alignment horizontal_align, alignment vertical_align) {
	nk_label(ctx, "", 0);
	Fill_Circle_In_Current_Rect(ctx, color, compression, horizontal_align,
		vertical_align);
}
