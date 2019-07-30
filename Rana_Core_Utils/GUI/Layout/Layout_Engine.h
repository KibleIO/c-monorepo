#ifndef LAYOUT_ENGINE_H_
#define LAYOUT_ENGINE_H_

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Layout_Utilities.h"
#include "Rect_Transform.h"
#include "Breadth_Buffer_Pair.h"
#include "../GUI.h"
#include "../../Utilities/vararg_ext.h"
#include "../../Utilities/utilities.h"
#include "../Widgets/Panel.h"

// Move the window layout down towards the bottom so that the next
// row laid out can be the user-defined footer
void Prepare_Footer_Layout(struct nk_context*, LAYOUT_SIZE footer_height);

// Layout the rect that results from transforming the current widget area
int Layout_Transformed_Area(struct nk_context*, RECT_TRANSFORM);

// Layout a row with a single column with no buffer
// and that fills up all horizontal space
void Layout_Row_Single_Full(
struct nk_context*, BREADTH_BUFFER_PAIR row);

// Layout a row with only one column
void Layout_Row_Single(
struct nk_context*, BREADTH_BUFFER_PAIR row_size,
BREADTH_BUFFER_PAIR column_size);

// Layout evenly spaced items, with or without buffers between
// the sides of the panel
void Layout_Row_Evenly_Spaced_Items(
struct nk_context*, BREADTH_BUFFER_PAIR row, uint8_t items,
float buffer_to_breadth_ratio, bool side_buffers);

void Layout_Row_Evenly_Spaced_Items(
struct nk_context*, BREADTH_BUFFER_PAIR row, LAYOUT_SIZE item_widths,
uint8_t items, bool side_buffers);

// Layout multiple columns with the same buffers and breadths
void Layout_Row_Homogenous(
struct nk_context*, BREADTH_BUFFER_PAIR row_size, uint8_t columns,
BREADTH_BUFFER_PAIR column_size);

void Layout_Row_Symmetric_Even_Pairs(
struct nk_context*, BREADTH_BUFFER_PAIR row_size, LAYOUT_SIZE mid_buffer,
uint8_t total_sizes, /*BREADTH_BUFFER_PAIR*/ ...);

void Layout_Row_Symmetric_Odd_Pairs(
struct nk_context*, BREADTH_BUFFER_PAIR row_size, uint8_t total_sizes,
/*BREADTH_BUFFER_PAIR*/ ...);

void Layout_Row_Custom(
struct nk_context*, BREADTH_BUFFER_PAIR row_size, uint8_t total_sizes,
/*BREADTH_BUFFER_PAIR*/ ...);

void Layout_Row(
struct nk_context*, BREADTH_BUFFER_PAIR row,
uint8_t total_columns, const BREADTH_BUFFER_PAIR columns[]);

void Push_Layout_Size(
const LAYOUT_SIZE*, struct nk_context*, uint32_t* at_width);

int Nk_Group_Begin_With_Buffer(struct nk_context*, const char*, nk_flags);
void Nk_Label_With_Buffer(struct nk_context*, const char*, nk_flags);

#endif // ENDIF
