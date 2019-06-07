#include "Render_Utilities.h"

void Fill_Current_Rect(
struct nk_context* ctx, float rounding, struct nk_color color) {
	nk_fill_rect(
	nk_window_get_canvas(ctx), nk_widget_bounds(ctx), rounding, rb_flip(color));
}

void Fill_Current_Rect_With_Buffer(
struct nk_context* ctx, float rounding, struct nk_color color) {
	nk_label(ctx, "", 0);
	Fill_Current_Rect(ctx, rounding, color);
}
