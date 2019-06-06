#include "Layout_Engine.h"

/*
LAYOUT SIZE
*/

LAYOUT_SIZE Layout_Size(float buffer, float breadth) {
	LAYOUT_SIZE size;
	size.buffer = buffer;
	size.breadth = breadth;
	return size;
}

LAYOUT_SIZE Layout_Size(float breadth) {
	LAYOUT_SIZE size;
	size.buffer = 0;
	size.breadth = breadth;
	return size;
}

LAYOUT_SIZE Evenly_Spaced_Item(uint8_t n, float r, bool side_buffers) {
	// Assign the number of buffers to n + 1 or n - 1
	uint8_t buffers = side_buffers ? n + 1 : n - 1;
	float d = (buffers * r) + n;
	return Layout_Size(r / d, 1.0 / d);
}

void Scale_Layout_Size(LAYOUT_SIZE* size, float scalar) {
	size->buffer *= scalar;
	size->breadth *= scalar;
}

/*
LAYOUT BUFFER FLAGS
*/

bool Get_Flag(const LAYOUT_BUFFER_FLAGS* flags, uint8_t column) {
	if(column < flags->totalFlags) {
		return flags->bufferFlag[column];
	}
	else {
		return false;
	}
}

/*
ROW LAYOUT
*/

LAYOUT_BUFFER_FLAGS Layout_Row_Single_Full(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size) {
	if(type != LAYOUT_TYPE_DYNAMIC) {
		return Layout_Row_Single(
		ctx, type, row_size, Layout_Size(Current_Usable_Panel_Width(ctx)));
	}
	else {
		return Layout_Row_Single(
		ctx, type, row_size, Layout_Size(1));
	}
}

LAYOUT_BUFFER_FLAGS Layout_Row_Single(struct nk_context* ctx, LAYOUT_TYPE type,
LAYOUT_SIZE row_size, LAYOUT_SIZE column_size) {
	return Layout_Row(ctx, type, row_size, 1, &column_size);
}

LAYOUT_BUFFER_FLAGS Layout_Row_Evenly_Spaced_Items(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size, uint8_t items,
float r, bool side_buffers) {
	LAYOUT_SIZE sizes =
	Evenly_Spaced_Item(items, r, side_buffers);

	// If layout is static, scale up the layouts sizes
	// by the usable panel width
	if(type == LAYOUT_TYPE_STATIC) {
		int current_width = Current_Usable_Panel_Width(ctx);
		Scale_Layout_Size(&sizes, current_width);
	}

	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
	for (
	uint8_t i = 0; i < items && i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		if (i == 0) {
			column_size_array[i] = Layout_Size(0, sizes.breadth);
		}
		else {
			column_size_array[i] = sizes;
		}
	}

	// Get the flags from laying out the row
	return Layout_Row(ctx, type, row_size, items, column_size_array);
}

LAYOUT_BUFFER_FLAGS Layout_Row_Homogenous(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
int total_columns, LAYOUT_SIZE column_size) {
	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Initialize each value in the array to the same value passed in
	for (
	int i = 0; i < total_columns &&
	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_size;
	}
	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
}

LAYOUT_BUFFER_FLAGS Layout_Row_Custom(struct nk_context* ctx, LAYOUT_TYPE type,
LAYOUT_SIZE row_size, int total_columns, ...) {
	// Allocate an array of sizes
	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
	total_columns = min(total_columns, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_size_array, (unsigned int)total_columns, LAYOUT_SIZE);

	// Layout the row using the allocated array
	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
}

LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Even_Pairs(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
float mid_buffer, int total_pairs, ...) {
	// Allocate an array for each pair
	LAYOUT_SIZE column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Allocate an array for each individual size
	int total_columns = total_pairs * 2;
	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	total_pairs = min(total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_pairs_array, (unsigned int)total_pairs, LAYOUT_SIZE);

	// Build up the array of sizes by adding each size
	// to a position and it's opposite
	for (
	int i = 0; i < total_pairs &&
	i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_pairs_array[i];

		// If this is not the last pair...
		if(i < total_pairs - 1) {
			//...give the column opposite the current the same breadth
			// and the buffer of the one after the current
			column_size_array[total_columns - 1 + i] = Layout_Size(
				column_pairs_array[i + 1].buffer,
				column_pairs_array[i].breadth
			);
		}
		// If this is the last pair...
		else {
			//...the buffer of the column opposite is equal to any space
			// not taken up by all of the other sizes
			column_size_array[total_columns - 1 + i] = Layout_Size(
				mid_buffer, column_pairs_array[i].breadth
			);
		}
	}

	// Layout the row using the allocated array
	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
}

LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Odd_Pairs(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
int total_pairs, ...) {
	// Allocate an array for each pair
	LAYOUT_SIZE column_pairs_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	// Allocate an array for each individual size
	int total_columns = (total_pairs * 2) - 1;
	LAYOUT_SIZE column_size_array[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];

	total_pairs = min(total_pairs, NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);

	// Unpack the arguments into the locally allocated array
	va_array(column_pairs_array, (unsigned int)total_pairs, LAYOUT_SIZE);

	// Build up the array of sizes by adding each size
	// to a position and it's opposite
	for(
	int i = 0; i < total_pairs && i < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		column_size_array[i] = column_pairs_array[i];

		// If this is not the last pair...
		if(i < total_pairs - 1) {
			//...give the column opposite the current the same breadth
			// and the buffer of the one after the current
			column_size_array[total_columns - 1 + i] = Layout_Size(
			column_pairs_array[i + 1].buffer, column_pairs_array[i].breadth);
		}
	}

	// Layout the row
	return Layout_Row(ctx, type, row_size, total_columns, column_size_array);
}

LAYOUT_BUFFER_FLAGS Layout_Row(
struct nk_context* ctx, LAYOUT_TYPE type, LAYOUT_SIZE row_size,
int total_columns, const LAYOUT_SIZE column_sizes[]) {
	LAYOUT_BUFFER_FLAGS flags;
	flags.totalFlags = total_columns;

	// Push out the row's buffer space, if it has any
	int buffer_size = Exact_Size(
	type, row_size.buffer, Current_Usable_Panel_Height(ctx));
	if(buffer_size > 0) {
		nk_layout_row_dynamic(ctx, buffer_size, 0);
	}

	// Begin a templated row
	nk_layout_row_template_begin(
	ctx, Exact_Size(type, row_size.breadth, Current_Usable_Panel_Height(ctx)));

	// Repeatedly push each column buffer and breadth, one after the other
	for (
	int i = 0; i < total_columns && NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS; i++) {
		// If there is buffer space for the column, layout the buffer
		buffer_size = Exact_Size(
		type, column_sizes[i].buffer, Current_Usable_Panel_Width(ctx));
		if(buffer_size > 0) {
			nk_layout_row_template_push_static(ctx, buffer_size);
		}

		// Set flag to true if there was buffer space pushed into the row
		flags.bufferFlag[i] = buffer_size > 0;

		// Layout the breadth of the column
		nk_layout_row_template_push_static(
		ctx, Exact_Size(type, column_sizes[i].breadth,
		Current_Usable_Panel_Width(ctx)));
	}

	// Finish the templated row
	nk_layout_row_template_end(ctx);

	return flags;
}

// RATIO CONVERSION
int Exact_Size(LAYOUT_TYPE type, float size, int containing_space) {
	if(Use_As_Exact(type, size)) {
		return (int)round(size);
	}
	else if(Use_As_Ratio(type, size)) {
		return (int)round(containing_space * size);
	}
	else {
		return 0;
	}
}
bool Use_As_Exact(LAYOUT_TYPE type, float size) {
	return type == LAYOUT_TYPE_STATIC ||
		(type == LAYOUT_TYPE_MIXED && size > 1.0);
}
bool Use_As_Ratio(LAYOUT_TYPE type, float size) {
	return type == LAYOUT_TYPE_DYNAMIC ||
		(type == LAYOUT_TYPE_MIXED && size <= 1.0);
}

// PANEL SIZING
float Current_Usable_Panel_Height(const struct nk_context* ctx) {
	const struct nk_panel* panel = ctx->current->layout;

	// Return the usable height of the current panel
	return Usable_Panel_Height(&ctx->style, panel->type, panel->bounds.h);
}

float Current_Usable_Panel_Width(const struct nk_context* ctx) {
	const struct nk_panel* panel = ctx->current->layout;

	// Return the usable height of the current panel
	return Usable_Panel_Width(&ctx->style, panel->type, panel->bounds.w);
}

float Usable_Panel_Height(const struct nk_style* style,
enum nk_panel_type type, int total_height) {
	// Get the padding on the given panel's type in the style struct
	struct nk_vec2 padding = Get_Panel_Padding(style, type);
	return total_height - (2 * padding.y);
}

float Usable_Panel_Width(const struct nk_style* style,
enum nk_panel_type type, int total_width) {
	// Get the padding on the given panel's type in the style struct
	struct nk_vec2 padding = Get_Panel_Padding(style, type);
	return total_width - (2 * padding.x);
}

struct nk_vec2 Get_Panel_Padding(const struct nk_style* style,
enum nk_panel_type type) {
	switch (type) {
		default:
	    case NK_PANEL_WINDOW: 		return style->window.padding;
	    case NK_PANEL_GROUP: 		return style->window.group_padding;
	    case NK_PANEL_POPUP: 		return style->window.popup_padding;
	    case NK_PANEL_CONTEXTUAL: 	return style->window.contextual_padding;
	    case NK_PANEL_COMBO: 		return style->window.combo_padding;
	    case NK_PANEL_MENU: 		return style->window.menu_padding;
	    case NK_PANEL_TOOLTIP: 		return style->window.menu_padding;
	}
}

float Remaining_Horizontal_Space(const struct nk_context* ctx,
LAYOUT_TYPE type, int total_sizes, const LAYOUT_SIZE* sizes) {
	if(type == LAYOUT_TYPE_DYNAMIC) {
		return (float)(Exact_Remaining_Horizontal_Space(
		ctx, type, total_sizes, sizes) / Current_Usable_Panel_Width(ctx));
	}
	else {
		return Exact_Remaining_Horizontal_Space(ctx, type, total_sizes, sizes);
	}
}

int Exact_Remaining_Horizontal_Space(const struct nk_context* ctx,
LAYOUT_TYPE type, int total_sizes, const LAYOUT_SIZE* sizes) {
	return Current_Usable_Panel_Width(ctx) -
	Exact_Horizontal_Size(ctx, type, total_sizes, sizes);
}

int Exact_Horizontal_Size(const struct nk_context* ctx, LAYOUT_TYPE type,
int total_sizes, const LAYOUT_SIZE* sizes) {
	int total = 0;

	// Add up the exact sizes of all breadths and buffers
	for(int i = 0; i < total_sizes; i++) {
		total += Exact_Size(
		type, sizes[i].buffer, Current_Usable_Panel_Width(ctx));
		total += Exact_Size(
		type, sizes[i].breadth, Current_Usable_Panel_Width(ctx));
	}

	return total;
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
