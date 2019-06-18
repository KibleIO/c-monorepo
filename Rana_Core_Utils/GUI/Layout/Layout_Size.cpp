#include "Layout_Size.h"

LAYOUT_SIZE Layout_Size(float size, SIZE_TYPE type) {
	LAYOUT_SIZE layout_size;
	layout_size.size = size;
	layout_size.type = type;
	return layout_size;
}

LAYOUT_SIZE Ratio_Of_Total(float ratio) {
	return Layout_Size(ratio, SIZE_TYPE_RATIO_OF_PANEL_TOTAL);
}
LAYOUT_SIZE Ratio_Of_Remainder(float ratio) {
	return Layout_Size(ratio, SIZE_TYPE_RATIO_OF_PANEL_REMAINDER);
}
LAYOUT_SIZE Ratio_Of_Opposite(float ratio) {
	return Layout_Size(ratio, SIZE_TYPE_RATIO_OF_OPPOSITE);
}
LAYOUT_SIZE Exact_Size(float size) {
	return Layout_Size(size, SIZE_TYPE_EXACT_SIZE);
}

uint32_t Exact_Size(
const LAYOUT_SIZE* size, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	// Determine the size
	switch (size->type) {
		case SIZE_TYPE_RATIO_OF_PANEL_TOTAL:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->size * (float)Current_Usable_Panel_Width(ctx));
				case VERTICAL:
					return round(
					size->size * (float)Current_Usable_Panel_Height(ctx));
				default:
					return 0;
			}
			break;
		case SIZE_TYPE_RATIO_OF_PANEL_REMAINDER:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->size * (float)remaining_horizontal_space);
				case VERTICAL:
					return round(
					size->size * (float)Remaining_Usable_Panel_Height(ctx));
				default:
					return 0;
			}
			break;
		case SIZE_TYPE_RATIO_OF_OPPOSITE:
			switch (o) {
				case HORIZONTAL:
					return round(
					size->size * (float)ctx->current->layout->row.height);
				case VERTICAL:
					return round(
					size->size * (float)Current_Usable_Panel_Width(ctx));
				default:
					return 0;
			}
			break;
		case SIZE_TYPE_EXACT_SIZE:
			return round(size->size);
		default:
			return 0;
	}
}

uint32_t Exact_Size_Horizontal(
const LAYOUT_SIZE* size, const struct nk_context* ctx,
uint32_t remaining_horizontal_space) {
	return Exact_Size(size, ctx, HORIZONTAL, remaining_horizontal_space);
}

uint32_t Exact_Size_Vertical(
const LAYOUT_SIZE* size, const struct nk_context* ctx) {
	return Exact_Size(size, ctx, VERTICAL, 0);
}

LAYOUT_SIZE Convert_To_Ratio_Of_Total(
const LAYOUT_SIZE* size, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	uint32_t exact_size;
	uint32_t total_panel_size;

	switch (size->type) {
		case SIZE_TYPE_RATIO_OF_PANEL_TOTAL:
			return *size;
		case SIZE_TYPE_RATIO_OF_PANEL_REMAINDER:
		case SIZE_TYPE_RATIO_OF_OPPOSITE:
		case SIZE_TYPE_EXACT_SIZE:
			exact_size = Exact_Size(size, ctx, o, remaining_horizontal_space);
			total_panel_size = Current_Usable_Panel_Space(ctx, o);
			return Ratio_Of_Total((float)exact_size / total_panel_size);
		default:
			return Ratio_Of_Total(0);
	}
}

LAYOUT_SIZE Convert_To_Ratio_Of_Total_Vertical(
const LAYOUT_SIZE* size, const struct nk_context* ctx) {
	return Convert_To_Ratio_Of_Total(size, ctx, VERTICAL, 0);
}

LAYOUT_SIZE Convert_To_Ratio_Of_Total_Horizontal(
const LAYOUT_SIZE* size, const struct nk_context* ctx,
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
