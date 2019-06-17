#include "Layout_Ratio.h"

LAYOUT_RATIO Layout_Ratio(
float ratio, RATIO_TYPE type) {
	LAYOUT_RATIO size;
	size.ratio = ratio;
	size.type = type;
	return size;
}

LAYOUT_RATIO Ratio_Of_Total(float ratio) {
	return Layout_Ratio(ratio, RATIO_OF_PANEL_TOTAL);
}
LAYOUT_RATIO Ratio_Of_Remainder(float ratio) {
	return Layout_Ratio(ratio, RATIO_OF_PANEL_REMAINDER);
}
LAYOUT_RATIO Ratio_Of_Opposite(float ratio) {
	return Layout_Ratio(ratio, RATIO_OF_OPPOSITE);
}

uint32_t Exact_Size(
const LAYOUT_RATIO* size, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	// Determine the size
	switch (size->type) {
		case RATIO_OF_PANEL_TOTAL:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->ratio * (float)Current_Usable_Panel_Width(ctx));
				case VERTICAL:
					return round(
					size->ratio * (float)Current_Usable_Panel_Height(ctx));
				default:
					return 0;
			}
			break;
		case RATIO_OF_PANEL_REMAINDER:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->ratio * (float)remaining_horizontal_space);
				case VERTICAL:
					return round(
					size->ratio * (float)Remaining_Usable_Panel_Height(ctx));
				default:
					return 0;
			}
			break;
		case RATIO_OF_OPPOSITE:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->ratio * (float)ctx->current->layout->row.height);
				case VERTICAL:
					return round(
					size->ratio * (float)Current_Usable_Panel_Width(ctx));
				default:
					return 0;
			}
			break;
		default:
			return 0;
	}
}

uint32_t Exact_Size_Horizontal(
const LAYOUT_RATIO* size, const struct nk_context* ctx,
uint32_t remaining_horizontal_space) {
	return Exact_Size(size, ctx, HORIZONTAL, remaining_horizontal_space);
}

uint32_t Exact_Size_Vertical(
const LAYOUT_RATIO* size, const struct nk_context* ctx) {
	return Exact_Size(size, ctx, VERTICAL, 0);
}

LAYOUT_RATIO Convert_To_Ratio_Of_Total(
const LAYOUT_RATIO* size, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	uint32_t exact_size;
	uint32_t total_panel_size;

	switch (size->type) {
		case RATIO_OF_PANEL_TOTAL:
			return *size;
		case RATIO_OF_PANEL_REMAINDER:
		case RATIO_OF_OPPOSITE:
			exact_size = Exact_Size(size, ctx, o, remaining_horizontal_space);
			total_panel_size = Current_Usable_Panel_Space(ctx, o);
			return Ratio_Of_Total((float)exact_size / total_panel_size);
		default:
			return Ratio_Of_Total(0);
	}
}

LAYOUT_RATIO Convert_To_Ratio_Of_Total_Vertical(
const LAYOUT_RATIO* size, const struct nk_context* ctx) {
	return Convert_To_Ratio_Of_Total(size, ctx, VERTICAL, 0);
}

LAYOUT_RATIO Convert_To_Ratio_Of_Total_Horizontal(
const LAYOUT_RATIO* size, const struct nk_context* ctx,
uint32_t remaining_horizontal_space) {
	return Convert_To_Ratio_Of_Total(
	size, ctx, HORIZONTAL, remaining_horizontal_space);
}

uint32_t Current_Usable_Panel_Space(
const struct nk_context* ctx, ORIENTATION o) {
	switch (o) {
		case VERTICAL:
			return Current_Usable_Panel_Height(ctx);
		case HORIZONTAL:
			return Current_Usable_Panel_Width(ctx);
		default:
			return 0;
	}
}
