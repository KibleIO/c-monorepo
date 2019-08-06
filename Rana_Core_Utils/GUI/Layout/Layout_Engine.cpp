#include "Layout_Engine.h"

void Prepare_Footer_Layout(struct nk_context* ctx, LAYOUT_SIZE footer_height) {
	uint32_t exact_footer_height = Exact_Size_Vertical(&footer_height, ctx);
	uint32_t remaining_height = Remaining_Usable_Panel_Height(ctx);
	int32_t diff = remaining_height - exact_footer_height;

	if (diff > 0) {
		nk_layout_row_dynamic(ctx, diff, 0);
	}
}

int Layout_Transformed_Area(struct nk_context* ctx, RECT_TRANSFORM transform) {
 	PANEL group = Panel(Nk_Window_Style());
	struct nk_rect widget_bounds = nk_widget_bounds(ctx);
	struct nk_rect group_rect = Transform_Rect(widget_bounds, transform);
	float upper_buffer_height = abs(widget_bounds.y - group_rect.y);

	if (Start_Group(&group, ctx, "transformed area", NK_WINDOW_NO_SCROLLBAR)) {

		if (upper_buffer_height > 0.0) {
			Layout_Row_Single(
			ctx, Buffer_And_Breadth(Exact_Size(upper_buffer_height), Exact_Size(
			group_rect.h)), Buffer_And_Breadth(Exact_Size(
			abs(widget_bounds.x - group_rect.x)), Exact_Size(group_rect.w)));
		}
		else {
			Layout_Row_Single(
			ctx, Breadth(Exact_Size(group_rect.h)), Buffer_And_Breadth(
			Exact_Size(abs(widget_bounds.x - group_rect.x)), Exact_Size(
			group_rect.w)));
		}

		nk_label(ctx, "", 0);
		return true;
	}
	else {
		return false;
	}
}

void Layout_All(struct nk_context* ctx) {
	Layout_Row_Single_Full(ctx, Breadth(Ratio_Of_Total(1)));
}

void Layout_Row_Single_Full(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size) {
	Layout_Row_Single(ctx, row_size, Breadth(Ratio_Of_Total(1)));
}

void Layout_Row_Single(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size,
BREADTH_BUFFER_PAIR column_size) {
	Layout_Row(ctx, row_size, 1, &column_size);
}

void Layout_Row_Evenly_Spaced_Items(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size, uint8_t items,
float r, bool side_buffers) {
	BREADTH_BUFFER_PAIR sizes = Evenly_Spaced_Item(items, r, side_buffers);
	BREADTH_BUFFER_PAIR column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	for (
	uint8_t i = 0; i < items && i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		if (i == 0 && !side_buffers) {
			column_size_array[i] = Breadth(sizes.breadth);
		}
		else {
			column_size_array[i] = sizes;
		}
	}

	// Layout the row
	Layout_Row(ctx, row_size, items, column_size_array);
}

void Layout_Row_Evenly_Spaced_Items(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row, LAYOUT_SIZE item_widths,
uint8_t items, bool side_buffers) {
	// Assert that the current window is null
	assert(ctx->current && "if this triggers you forgot to call `nk_begin`");

	uint32_t at_width = 0;
	uint32_t row_height = Exact_Buffer_Vertical(&row, ctx);

	// Layout the row buffer if it is bigger than 0
	if (row_height > 0) {
		nk_layout_row_dynamic(ctx, row_height, 0);
	}

	// Layout the row, if it has height
	row_height = Exact_Breadth_Vertical(&row, ctx);
	if (row_height > 0) {
		nk_layout_row_template_begin(ctx, row_height);

		// Initialize the array of columns in this row
		BREADTH_BUFFER_PAIR columns[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
		items = avir::clamp((int)items, 0, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
		Evenly_Spaced_Items(columns, item_widths, ctx, items, side_buffers);

		// Repeatedly push each column buffer and breadth, one after the other
		for (
		uint8_t i = 0; i < items &&
		i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
			Push_Layout_Size(&columns[i].buffer, ctx, &at_width);
			Push_Layout_Size(&columns[i].breadth, ctx, &at_width);
		}

		// Finish the templated row
		nk_layout_row_template_end(ctx);
	}
}

void Layout_Row_Homogenous(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size,
uint8_t total_columns, BREADTH_BUFFER_PAIR column_size) {
	BREADTH_BUFFER_PAIR column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Initialize each value in the array to the same value passed in
	for (
	int i = 0; i < total_columns &&
	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_size;
	}

	Layout_Row(ctx, row_size, total_columns, column_size_array);
}

void Layout_Row_Symmetric_Even_Pairs(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size,
LAYOUT_SIZE mid_buffer, uint8_t total_pairs, ...) {
	// Allocate an array for each pair
	BREADTH_BUFFER_PAIR column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Allocate an array for each individual size
	uint8_t total_columns = total_pairs * 2;
	BREADTH_BUFFER_PAIR column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	total_pairs = min((int)total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_pairs_array, total_pairs, BREADTH_BUFFER_PAIR);

	// Build up the array of sizes by adding each size
	// to a position and it's opposite
	for (
	uint8_t i = 0; i < total_pairs &&
	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_pairs_array[i];

		// If this is not the last pair...
		if(i < total_pairs - 1) {
			//...give the column opposite the current the same breadth
			// and the buffer of the one after the current
			column_size_array[total_columns - i - 1] = Buffer_And_Breadth(
			column_pairs_array[i + 1].buffer, column_pairs_array[i].breadth);
		}
		// If this is the last pair...
		else {
			//...the buffer of the column opposite is the middle buffer
			column_size_array[total_columns - i - 1] = Buffer_And_Breadth(
			mid_buffer, column_pairs_array[i].breadth);
		}
	}

	// Layout the row using the allocated array
 	Layout_Row(ctx, row_size, total_columns, column_size_array);
}

void Layout_Row_Symmetric_Odd_Pairs(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size, uint8_t total_pairs,
/*BREADTH_BUFFER_PAIR*/ ...) {

	// Allocate an array for each pair
	BREADTH_BUFFER_PAIR column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Allocate an array for each individual size
	uint8_t total_columns = (total_pairs * 2) - 1;
	BREADTH_BUFFER_PAIR column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	total_pairs = min((int)total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_pairs_array, total_pairs, BREADTH_BUFFER_PAIR);

	// Build up the array of sizes by adding each size
	// to a position and it's opposite
	for(
	uint8_t i = 0; i < total_pairs &&
	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_pairs_array[i];

		// If this is not the last pair...
		if(i < total_pairs - 1) {
			//...give the column opposite the current the same breadth
			// and the buffer of the one after the current
			column_size_array[total_columns - 1 - i] = Buffer_And_Breadth(
			column_pairs_array[i + 1].buffer, column_pairs_array[i].breadth);
		}
	}

	// Layout the row
	Layout_Row(ctx, row_size, total_columns, column_size_array);
}

void Layout_Row_Custom(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row_size, uint8_t total_columns,
/*BREADTH_BUFFER_PAIR*/ ...) {
	// Allocate an array of sizes
	BREADTH_BUFFER_PAIR column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
	total_columns = min((int)total_columns, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_size_array, total_columns, BREADTH_BUFFER_PAIR);

	// Layout the row using the allocated array
	Layout_Row(ctx, row_size, total_columns, column_size_array);
}

void Layout_Row(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row, uint8_t total_columns,
const BREADTH_BUFFER_PAIR columns[]) {
	// Assert that the current window is null
	assert(ctx->current && "if this triggers you forgot to call `nk_begin`");

	uint32_t at_width = 0;
	uint32_t row_height = Exact_Buffer_Vertical(&row, ctx);

	// Layout the row buffer if it is bigger than 0
	if (row_height > 0) {
		nk_layout_row_dynamic(ctx, row_height, 0);
	}

	// Layout the row, if it has height
	row_height = Exact_Breadth_Vertical(&row, ctx);
	if (row_height > 0) {
		nk_layout_row_template_begin(ctx, row_height);

		// Repeatedly push each column buffer and breadth, one after the other
		for (
		uint8_t i = 0; i < total_columns &&
		i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
			Push_Layout_Size(&columns[i].buffer, ctx, &at_width);
			Push_Layout_Size(&columns[i].breadth, ctx, &at_width);
		}

		// Finish the templated row
		nk_layout_row_template_end(ctx);
	}
}

void Push_Layout_Size(
const LAYOUT_SIZE* size, struct nk_context* ctx, uint32_t* at_width) {
	uint32_t column_width = Exact_Size_Horizontal(
	size, ctx, Current_Usable_Panel_Width(ctx) - *at_width);

	nk_layout_row_template_push_static(ctx, column_width);

	*at_width += column_width;
}

int Nk_Group_Begin_With_Buffer(struct nk_context* ctx, const char* title,
nk_flags flags) {
	nk_label(ctx, "", NK_TEXT_CENTERED);
	return nk_group_begin(ctx, title, flags);
}

void Nk_Label_With_Buffer(struct nk_context* ctx, const char* text,
nk_flags alignment) {
	nk_label(ctx, "", NK_TEXT_CENTERED);
	nk_label(ctx, text, alignment);
}
