#include "Render_Utilities.h"

void Fill_Current_Rect(
struct nk_context* ctx, float rounding, struct nk_color color,
RECT_TRANSFORM trans) {
	nk_fill_rect(
	nk_window_get_canvas(ctx), Transform_Rect(nk_widget_bounds(ctx), trans),
	rounding, color);
}

void Fill_Current_Rect_With_Buffer(
struct nk_context* ctx, float rounding, struct nk_color color,
RECT_TRANSFORM trans) {
	nk_label(ctx, "", 0);
	Fill_Current_Rect(ctx, rounding, color, trans);
}

void Fill_Circle_In_Current_Rect(
struct nk_context* ctx, struct nk_color color, RECT_TRANSFORM trans) {
	nk_fill_circle(
	nk_window_get_canvas(ctx), Transform_Rect(nk_widget_bounds(ctx), trans),
	color);
}

void Fill_Circle_In_Current_Rect_With_Buffer(
struct nk_context* ctx, struct nk_color color, RECT_TRANSFORM trans) {
	nk_label(ctx, "", 0);
	Fill_Circle_In_Current_Rect(ctx, color, trans);
}
