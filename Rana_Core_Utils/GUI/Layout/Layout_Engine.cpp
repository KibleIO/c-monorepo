#include "Layout_Engine.h"

void Prepare_Footer_Layout(struct nk_context* ctx, LAYOUT_RATIO footer_height) {
	uint32_t exact_footer_height = Exact_Size_Vertical(&footer_height, ctx);
	uint32_t remaining_height = Remaining_Usable_Panel_Height(ctx);
	int32_t diff = remaining_height - exact_footer_height;

	if (diff > 0) {
		nk_layout_row_dynamic(ctx, diff, 0);
	}
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

// TEMP
#include "../../Utilities/utilities.h"

void Layout_Row_Evenly_Spaced_Items(
struct nk_context* ctx, BREADTH_BUFFER_PAIR row, LAYOUT_RATIO item_widths,
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
			Push_Layout_Ratio(&columns[i].buffer, ctx, &at_width);
			Push_Layout_Ratio(&columns[i].breadth, ctx, &at_width);
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
LAYOUT_RATIO mid_buffer, uint8_t total_pairs, ...) {
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
			Push_Layout_Ratio(&columns[i].buffer, ctx, &at_width);
			Push_Layout_Ratio(&columns[i].breadth, ctx, &at_width);
		}

		// Finish the templated row
		nk_layout_row_template_end(ctx);
	}
}

void Push_Layout_Ratio(
const LAYOUT_RATIO* size, struct nk_context* ctx, uint32_t* at_width) {
	uint32_t column_width = Exact_Size_Horizontal(
	size, ctx, Current_Usable_Panel_Width(ctx) - *at_width);

	nk_layout_row_template_push_static(ctx, column_width);

	*at_width += column_width;
}

/*
LAYOUT SIZE
*/

// LAYOUT_SIZE Layout_Size(float buffer, float breadth) {
// 	LAYOUT_SIZE size;
// 	size.buffer = buffer;
// 	size.breadth = breadth;
// 	return size;
// }
//
// LAYOUT_SIZE Layout_Size(float breadth) {
// 	LAYOUT_SIZE size;
// 	size.buffer = 0;
// 	size.breadth = breadth;
// 	return size;
// }
//
// LAYOUT_SIZE Evenly_Spaced_Item(uint8_t n, float r, bool side_buffers) {
// 	// Assign the number of buffers to n + 1 or n - 1
// 	uint8_t buffers = side_buffers ? n + 1 : n - 1;
// 	float d = (buffers * r) + n;
// 	return Layout_Size(r / d, 1.0 / d);
// }
//
// void Scale_Layout_Size(LAYOUT_SIZE* size, float scalar) {
// 	size->buffer *= scalar;
// 	size->breadth *= scalar;
// }
// /*
// ROW LAYOUT
// */
//
// // DEPRECATED
// LAYOUT_BUFFER_FLAGS Layout_Row_Single_Full(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size) {
// 	if(type != LAYOUT_TYPE_DYNAMIC) {
// 		return Layout_Row_Single(
// 		ctx, type, row_size, Layout_Size(Current_Usable_Panel_Width(ctx)));
// 	}
// 	else {
// 		return Layout_Row_Single(
// 		ctx, type, row_size, Layout_Size(1));
// 	}
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Single(struct nk_context* ctx, LAYOUT_TYPE type,
// LAYOUT_SIZE row_size, LAYOUT_SIZE column_size) {
// 	return Layout_Row(ctx, type, row_size, 1, &column_size);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Evenly_Spaced_Items(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size, uint8_t items,
// float r, bool side_buffers) {
// 	// Assert that the current window is null
// 	assert(ctx->current && "if this triggers you forgot to call `nk_begin`");
//
// 	LAYOUT_SIZE sizes =
// 	Evenly_Spaced_Item(items, r, side_buffers);
//
// 	// If layout is static, scale up the layouts sizes
// 	// by the usable panel width
// 	if(type == LAYOUT_TYPE_STATIC) {
// 		int current_width = Current_Usable_Panel_Width(ctx);
// 		Scale_Layout_Size(&sizes, current_width);
// 	}
//
// 	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
// 	for (
// 	uint8_t i = 0; i < items && i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
// 		if (i == 0) {
// 			column_size_array[i] = Layout_Size(0, sizes.breadth);
// 		}
// 		else {
// 			column_size_array[i] = sizes;
// 		}
// 	}
//
// 	// Get the flags from laying out the row
// 	return Layout_Row(ctx, type, row_size, items, column_size_array);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Homogenous(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
// int total_columns, LAYOUT_SIZE column_size) {
// 	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
//
// 	// Initialize each value in the array to the same value passed in
// 	for (
// 	int i = 0; i < total_columns &&
// 	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
// 		column_size_array[i] = column_size;
// 	}
// 	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Custom(struct nk_context* ctx, LAYOUT_TYPE type,
// LAYOUT_SIZE row_size, int total_columns, ...) {
// 	// Allocate an array of sizes
// 	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
// 	total_columns = min(total_columns, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
//
// 	// Unpack the arguments into the locally allocated array
// 	va_array(column_size_array, (unsigned int)total_columns, LAYOUT_SIZE);
//
// 	// Layout the row using the allocated array
// 	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Even_Pairs(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
// float mid_buffer, int total_pairs, ...) {
// 	// Allocate an array for each pair
// 	LAYOUT_SIZE column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
//
// 	// Allocate an array for each individual size
// 	int total_columns = total_pairs * 2;
// 	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
//
// 	total_pairs = min(total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
//
// 	// Unpack the arguments into the locally allocated array
// 	va_array(column_pairs_array, (unsigned int)total_pairs, LAYOUT_SIZE);
//
// 	// Build up the array of sizes by adding each size
// 	// to a position and it's opposite
// 	for (
// 	int i = 0; i < total_pairs &&
// 	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
// 		column_size_array[i] = column_pairs_array[i];
//
// 		// If this is not the last pair...
// 		if(i < total_pairs - 1) {
// 			//...give the column opposite the current the same breadth
// 			// and the buffer of the one after the current
// 			column_size_array[total_columns - 1 + i] = Layout_Size(
// 				column_pairs_array[i + 1].buffer,
// 				column_pairs_array[i].breadth
// 			);
// 		}
// 		// If this is the last pair...
// 		else {
// 			//...the buffer of the column opposite is equal to any space
// 			// not taken up by all of the other sizes
// 			column_size_array[total_columns - 1 + i] = Layout_Size(
// 				mid_buffer, column_pairs_array[i].breadth
// 			);
// 		}
// 	}
//
// 	// Layout the row using the allocated array
// 	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Odd_Pairs(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
// int total_pairs, ...) {
// 	// Allocate an array for each pair
// 	LAYOUT_SIZE column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
//
// 	// Allocate an array for each individual size
// 	int total_columns = (total_pairs * 2) - 1;
// 	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
//
// 	total_pairs = min(total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
//
// 	// Unpack the arguments into the locally allocated array
// 	va_array(column_pairs_array, (unsigned int)total_pairs, LAYOUT_SIZE);
//
// 	// Build up the array of sizes by adding each size
// 	// to a position and it's opposite
// 	for(
// 	int i = 0; i < total_pairs && i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
// 		column_size_array[i] = column_pairs_array[i];
//
// 		// If this is not the last pair...
// 		if(i < total_pairs - 1) {
// 			//...give the column opposite the current the same breadth
// 			// and the buffer of the one after the current
// 			column_size_array[total_columns - 1 + i] = Layout_Size(
// 			column_pairs_array[i + 1].buffer, column_pairs_array[i].breadth);
// 		}
// 	}
//
// 	// Layout the row
// 	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
// }
//
// LAYOUT_BUFFER_FLAGS Layout_Row(
// struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
// int total_columns, const LAYOUT_SIZE column_sizes[]) {
// 	LAYOUT_BUFFER_FLAGS flags;
// 	flags.totalFlags = total_columns;
//
// 	// Push out the row's buffer space, if it has any
// 	int buffer_size = Exact_Size(
// 	type, row_size.buffer, Current_Usable_Panel_Height(ctx));
// 	if(buffer_size > 0) {
// 		nk_layout_row_dynamic(ctx, buffer_size, 0);
// 	}
//
// 	// Begin a templated row
// 	nk_layout_row_template_begin(
// 	ctx, Exact_Size(type, row_size.breadth, Current_Usable_Panel_Height(ctx)));
//
// 	// Repeatedly push each column buffer and breadth, one after the other
// 	for (
// 	int i = 0; i < total_columns && NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
// 		// If there is buffer space for the column, layout the buffer
// 		buffer_size = Exact_Size(
// 		type, column_sizes[i].buffer, Current_Usable_Panel_Width(ctx));
// 		if(buffer_size > 0) {
// 			nk_layout_row_template_push_static(ctx, buffer_size);
// 		}
//
// 		// Set flag to true if there was buffer space pushed into the row
// 		flags.bufferFlag[i] = buffer_size > 0;
//
// 		// Layout the breadth of the column
// 		nk_layout_row_template_push_static(
// 		ctx, Exact_Size(type, column_sizes[i].breadth,
// 		Current_Usable_Panel_Width(ctx)));
// 	}
//
// 	// Finish the templated row
// 	nk_layout_row_template_end(ctx);
//
// 	return flags;
// }
//
// // RATIO CONVERSION
// int Exact_Size(LAYOUT_TYPE type, float size, int containing_space) {
// 	if(Use_As_Exact(type, size)) {
// 		return (int)round(size);
// 	}
// 	else if(Use_As_Ratio(type, size)) {
// 		return (int)round(containing_space * size);
// 	}
// 	else {
// 		return 0;
// 	}
// }
// bool Use_As_Exact(LAYOUT_TYPE type, float size) {
// 	return type == LAYOUT_TYPE_STATIC ||
// 		(type == LAYOUT_TYPE_MIXED && size > 1.0);
// }
// bool Use_As_Ratio(LAYOUT_TYPE type, float size) {
// 	return type == LAYOUT_TYPE_DYNAMIC ||
// 		(type == LAYOUT_TYPE_MIXED && size <= 1.0);
// }

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
