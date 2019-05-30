#include "nuklear_plus.h"

// BASIC INITIALIZERS
// ------------------
void Initialize_Window_Layout(struct window_layout* window, struct nk_rect rect, struct size_hint bottomBuff,
  struct nk_style_window style, const char* title, bool subwindow, struct row_layout* rows, int totalRows) {
    // Allocate an array with a little extra space
    const int rowCapacity = totalRows * 2;
    struct row_layout* newRows = new row_layout[rowCapacity];
    char* copy = new char[strlen(title) + 1]; // Copy of the title string
    strcpy(copy, title);

    // Do a deep copy of the rows given
    for(int i = 0; i < totalRows; i++) {
      newRows[i] = rows[i];
    }

    // Assign each of the variables into the pointer
    *window = { rect, style, copy, bottomBuff, subwindow, newRows, totalRows, rowCapacity, 0, false };
  }
void Initialize_Row_Layout(struct row_layout* row, struct size_hint height, struct size_hint upperBuff,
  struct size_hint rightBuff, struct column_layout* columns, int totalColumns) {
    // Allocate an array with a little extra space
    const int columnCapacity = totalColumns * 2;
    struct column_layout* newColumns = new column_layout[columnCapacity];

    // Do a deep copy of the columns given
    for(int i = 0; i < totalColumns; i++) {
      newColumns[i] = columns[i];
    }

    // Assign each of the variables to the pointer
    *row = { height, upperBuff, rightBuff, columns, totalColumns, columnCapacity, 0, false };
  }
void Initialize_Column_Layout(struct column_layout* col, struct size_hint width, struct size_hint leftBuff, struct nk_style style) {
  *col = { width, leftBuff, style };
}

// BASIC DELETERS

void Delete_Window_Layout(struct window_layout* window) {
  for(int i = 0; i < window->totalRows; i++) {
    Delete_Row_Layout(&window->rows[i]);
  }
  delete [] window->rows;
  delete [] window->title;
}

void Delete_Row_Layout(struct row_layout* row) {
  delete [] row->columns;
}

// TEMPLATED INITIALIZERS
// ----------------------

void Initialize_Row_Layout_Homogenous(struct row_layout* row, struct size_hint height, struct size_hint upperBuff,
  struct size_hint columnSize, struct size_hint columnLeftBuffer, struct nk_style columnStyle, int totalItems) {
    *row = Row_Layout(height, upperBuff);
    Apply_Row_Template_Homogenous(row, columnSize, columnLeftBuffer, columnStyle, totalItems);
  }

void Initialize_Row_Layout_Symmetric_Even_Pairs(struct row_layout* row, struct size_hint height, struct size_hint upperBuff,
  struct column_layout* pairs, int totalPairs, struct size_hint midBuff) {
    *row = Row_Layout(height, upperBuff);
    Apply_Row_Template_Symmetric_Even_Pairs(row, pairs, totalPairs, midBuff);
  }

void Initialize_Row_Layout_Symmetric_Odd_Pairs(struct row_layout* row, struct size_hint height, struct size_hint upperBuff,
  struct column_layout* pairs, int totalColumns) {
    *row = Row_Layout(height, upperBuff);
    Apply_Row_Template_Symmetric_Odd_Pairs(row, pairs, totalColumns);
  }

// INITIALIZATION HELPERS
// ----------------------

void Initialize_Window_Layout_Begin(struct window_layout* window, struct nk_rect rect, struct nk_style_window style, const char* title) {
  *window = Window_Layout(rect, style, title);
  window->currentRow = -1;
  window->layoutFinished = false;
}

void Initialize_Subwindow_Layout_Begin(struct window_layout* window, const struct window_layout* parent, int rowIndex,
  int columnIndex, struct nk_style_window style, const char* title) {
    *window = Subwindow_Layout(parent, rowIndex, columnIndex, style, title);
    window->currentRow = -1;
    window->layoutFinished = false;
  }

void Initialize_Window_Layout_End(struct window_layout* window) {
  if(!window->layoutFinished) {
    cout << "WARNING: called \"Initialize_Window_Layout_End\" on window \"" << window->title << "\"\n\tbefore the window was finished initializing.\n\tDid you apply a template to all of the rows?\n";
  }

  window->currentRow = 0;
  window->layoutFinished = false;
  Finalize_Window_Sizes(window);
}

// WINDOW TEMPLATES

void Apply_Window_Template_Homogenous(struct window_layout* window, struct size_hint height, struct size_hint upperBuff, int totalRows) {
  const int rowCapacity = totalRows * 2;
  struct row_layout* rows = new row_layout[rowCapacity];

  // Assign each row layout the same upper buffer and height
  for(int i = 0; i < totalRows; i++) {
    rows[i] = Row_Layout(height, upperBuff);
  }

  // Set bottom buffer to match the top buffer
  window->bottomBuffer = upperBuff;

  // Set rows to rows set up in the function
  window->rows = rows;
  window->totalRows = totalRows;
  window->rowCapacity = rowCapacity;

  // Update layout finished
  window->layoutFinished = window->currentRow >= (window->totalRows - 1);
}

void Apply_Window_Template_Custom(struct window_layout* window, struct size_hint bottomBuff, struct row_layout* otherRows, int totalRows) {
  const int rowCapacity = totalRows * 2;
  struct row_layout* rows = new row_layout[rowCapacity];

  // Assign each row layout the same upper buffer and height
  for(int i = 0; i < totalRows; i++) {
    rows[i] = otherRows[i];
  }

  // Set bottom buffer
  window->bottomBuffer = bottomBuff;

  // Set rows to the rows given to the funtion
  window->rows = rows;
  window->totalRows = totalRows;
  window->rowCapacity = rowCapacity;

  // Update layout finished
  window->layoutFinished = window->currentRow >= (window->totalRows - 1);
}

// WINDOW TEMPLATE VARIADIC

void Apply_Window_Template_Custom(struct window_layout* window,
struct size_hint bottomBuff, int totalRows, /*row_layout*/ ...) {
	struct row_layout* row_array = new row_layout[totalRows];

	// Unpack the arguments and store them in the local array
	va_list vrows;
	va_start(vrows, totalRows);
	for(int i = 0; i < totalRows; i++) {
		row_array[i] = va_arg(vrows, row_layout);
	}
	va_end(vrows);

	Apply_Window_Template_Custom(window, bottomBuff, row_array, totalRows);
	delete [] row_array;
}

// CURRENT ROW TEMPLATES

void Apply_Row_Template_Homogenous(struct window_layout* window,
struct size_hint columnSize, struct size_hint columnLeftBuffer,
 struct nk_style columnStyle, int totalItems) {
    if(!window->layoutFinished) {
      // Update current row and layout finished
      window->currentRow++;
      window->layoutFinished = window->currentRow >= (window->totalRows - 1);
      // Apply template to current row
      Apply_Row_Template_Homogenous(&window->rows[window->currentRow], columnSize, columnLeftBuffer, columnStyle, totalItems);
    }
    else {
      cout << "WARNING: homogenous row template cannot be applied on window named \"" << window->title << "\"\n\tbecause you have already applied templates to every row" << endl;
    }
  }

void Apply_Row_Template_Homogenous_Custom_Sides(struct window_layout* window, struct size_hint sideBuff, struct size_hint columnSize,
  struct size_hint columnLeftBuffer, struct nk_style columnStyle, int totalItems) {
    if(!window->layoutFinished) {
      // Update current row and layout finished
      window->currentRow++;
      window->layoutFinished = window->currentRow >= (window->totalRows - 1);
      // Apply template to current row
      Apply_Row_Template_Homogenous_Custom_Sides(&window->rows[window->currentRow], sideBuff, columnSize, columnLeftBuffer, columnStyle, totalItems);
    }
    else {
      cout << "WARNING: homogenous custom sides row template cannot be applied on window named \"" << window->title << "\"\n\tbecause you have already applied templates to every row" << endl;
    }
  }

void Apply_Row_Template_Symmetric_Even_Pairs(struct window_layout* window, struct column_layout* pairs, int totalPairs, struct size_hint midBuff) {
  if(!window->layoutFinished) {
    // Update current row and layout finished
    window->currentRow++;
    window->layoutFinished = window->currentRow >= (window->totalRows - 1);
    // Apply template to current row
    Apply_Row_Template_Symmetric_Even_Pairs(&window->rows[window->currentRow], pairs, totalPairs, midBuff);
  }
  else {
    cout << "WARNING: symmetric even pairs row template cannot be applied on window named \"" << window->title << "\"\n\tbecause you have already applied templates to every row" << endl;
  }
}

void Apply_Row_Template_Symmetric_Odd_Pairs(struct window_layout* window, struct column_layout* pairs, int totalColumns) {
  if(!window->layoutFinished) {
    // Update current row and layout finished
    window->currentRow++;
    window->layoutFinished = window->currentRow >= (window->totalRows - 1);
    // Apply template to current row
    Apply_Row_Template_Symmetric_Odd_Pairs(&window->rows[window->currentRow], pairs, totalColumns);
  }
  else {
    cout << "WARNING: symmetric odd pairs row template cannot be applied on window named \"" << window->title << "\"\n\tbecause you have already applied templates to every row" << endl;
  }
}

void Apply_Row_Template_Custom(struct window_layout* window, struct size_hint rightBuff, struct column_layout* cols, int totalColumns) {
  if(!window->layoutFinished) {
    // Update current row and layout finished
    window->currentRow++;
    window->layoutFinished = window->currentRow >= (window->totalRows - 1);
    // Apply template to current row
    Apply_Row_Template_Custom(&window->rows[window->currentRow], rightBuff, cols, totalColumns);
  }
  else {
    cout << "WARNING: custom row template cannot be applied on window named \"" << window->title << "\"\n\tbecause you have already applied templates to every row" << endl;
  }
}

void Apply_Row_Template_Ditto_Previous(struct window_layout* window) {
  Apply_Row_Template_Ditto(window, window->currentRow);
}

void Apply_Row_Template_Ditto(struct window_layout* window, int rowIndex) {
  if(!window->layoutFinished && rowIndex <= window->currentRow) {
    // Store total columns
    int totalColumns = window->rows[rowIndex].totalColumns;
    // Update current row and layout finished
    window->currentRow++;
    window->layoutFinished = window->currentRow >= (window->totalRows - 1);
    // Copy specifically templated info from the row at rowIndex into the row at currentRow
    // Copy right buffer
    window->rows[window->currentRow].rightBuffer = window->rows[rowIndex].rightBuffer;
    window->rows[window->currentRow].totalColumns = totalColumns;
    // Deep-copy source columns into destination columns
    window->rows[window->currentRow].columns = new column_layout[totalColumns];
    for(int i = 0; i < totalColumns; i++) {
      window->rows[window->currentRow].columns[i] = window->rows[rowIndex].columns[i];
    }
  }
  else {
    cout << "WARNING: ditto row template cannot be applied on window named \""
		 << window->title << "\"\n\tbecause you have already applied "
		 << "templates to every row" << endl;
  }
}

// CURRENT ROW TEMPLATE VARIADIC

void Apply_Row_Template_Symmetric_Even_Pairs(struct window_layout* window,
struct size_hint midBuff, int totalPairs, /*column_layout*/...) {
	if(!window->layoutFinished) {
		// Create the variadic list and a basic array of columns
		va_list vcols;
		struct column_layout* col_array = new column_layout[totalPairs];

		// Put each item in the va_list into the array
		va_start(vcols, totalPairs);
		for(int i = 0; i < totalPairs; i++) {
			col_array[i] = va_arg(vcols, column_layout);
		}
		va_end(vcols);

		// Apply template to current row
		Apply_Row_Template_Symmetric_Even_Pairs(window,
			col_array, totalPairs, midBuff);

		// Delete array (function called above does a deep copy)
		delete [] col_array;
    }
    else {
		cout << "WARNING: custom row template cannot be applied on window "
			 << "named \"" << window->title << "\"\n\tbecause you have "
			 << "already applied templates to every row" << endl;
    }
}

void Apply_Row_Template_Symmetric_Odd_Pairs(struct window_layout* window,
int totalPairs, /*column_layout*/...) {
	if(!window->layoutFinished) {
		// Create the variadic list and a basic array of columns
		va_list vcols;
		struct column_layout* col_array = new column_layout[totalPairs];

		// Put each item in the va_list into the array
		va_start(vcols, totalPairs);
		for(int i = 0; i < totalPairs; i++) {
			col_array[i] = va_arg(vcols, column_layout);
		}
		va_end(vcols);

		// Apply template to current row
		Apply_Row_Template_Symmetric_Odd_Pairs(window, col_array, totalPairs);

		// Delete array (function called above does a deep copy)
		delete [] col_array;
	}
	else {
		cout << "WARNING: symmetric odd pairs row template cannot be applied "
			 << "on window named \"" << window->title << "\"\n\tbecause you "
			 << "have already applied templates to every row" << endl;
	}
}

void Apply_Row_Template_Custom(struct window_layout* window,
struct size_hint rightBuff, int totalColumns, /*column_layout*/...) {
	if(!window->layoutFinished) {
		// Create the variadic list and a basic array of columns
		va_list vcols;
		struct column_layout* col_array = new column_layout[totalColumns];

		// Put each item in the va_list into the array
		va_start(vcols, totalColumns);
		for(int i = 0; i < totalColumns; i++) {
			col_array[i] = va_arg(vcols, column_layout);
		}
		va_end(vcols);

		// Apply template to current row
		Apply_Row_Template_Custom(window, rightBuff,
			col_array, totalColumns);

		// Delete array (function called above does a deep copy)
		delete [] col_array;
	}
	else {
		cout << "WARNING: custom row template cannot be applied "
			 << "on window named \"" << window->title << "\"\n\tbecause "
			 << "you have already applied templates to every row" << endl;
	}
}


// ROW TEMPLATES

void Apply_Row_Template_Homogenous(struct row_layout* row,
struct size_hint columnSize, struct size_hint columnLeftBuffer,
struct nk_style columnStyle, int totalItems) {
    // Create an array of columns to initialize the row with
    const int columnCapacity = totalItems * 2;
    struct column_layout* cols = new column_layout[columnCapacity];

    // Intialize all columns with the same data
    for(int i = 0; i < totalItems; i++) {
      cols[i] = { columnSize, columnLeftBuffer, columnStyle };
    }

    // Set columns and total columns with columns created
    row->columns = cols;
    row->totalColumns = totalItems;
    row->columnCapacity = columnCapacity;
    row->rightBuffer = columnLeftBuffer;
  }

void Apply_Row_Template_Homogenous_Custom_Sides(struct row_layout* row, struct size_hint sideBuff, struct size_hint columnSize,
  struct size_hint columnLeftBuffer, struct nk_style columnStyle, int totalItems) {
    // Create an array of columns to initialize the row with
    const int columnCapacity = totalItems * 2;
    struct column_layout* cols = new column_layout[columnCapacity];

    // Intialize all columns
    for(int i = 0; i < totalItems; i++) {
      // The first column has the same left buffer as the buffer on the sides of the window
      if(i == 0) {
        cols[i] = { columnSize, sideBuff, columnStyle };
      }
      // All other columns have the same left buffer given
      else {
        cols[i] = { columnSize, columnLeftBuffer, columnStyle };
      }
    }

    // Set columns and total columns with columns created
    row->columns = cols;
    row->totalColumns = totalItems;
    row->columnCapacity = columnCapacity;
    row->rightBuffer = sideBuff;
  }

void Apply_Row_Template_Symmetric_Even_Pairs(struct row_layout* row,
struct column_layout* pairs, int totalPairs, struct size_hint midBuff) {
  // Allocate an array with double the columns as pairs given
  const int totalColumns = totalPairs * 2;
  const int columnCapacity = totalColumns * 2;
  struct column_layout* cols = new column_layout[columnCapacity];

  for(int i = 0; i < totalPairs; i++) {
    cols[i] = pairs[i];

    // If this is not the last item pair...
    if(i < totalPairs - 1) {
      //...the opposite item should have the left buffer of the NEXT item
      cols[(totalColumns - 1) - i] = column_layout { pairs[i].width, pairs[i + 1].leftBuffer, pairs[i].style };
    }
    // If this is the last pair...
    else {
      //...the left buffer should be the buffer between the middle pair
      cols[(totalColumns - 1) - i] = column_layout { pairs[i].width, midBuff, pairs[i].style };
    }
  }

  // Set columns and total columns with columns created
  row->columns = cols;
  row->totalColumns = totalColumns;
  row->columnCapacity = columnCapacity;
  row->rightBuffer = pairs[0].leftBuffer;
}

void Apply_Row_Template_Symmetric_Odd_Pairs(struct row_layout* row, struct column_layout* pairs, int totalPairs) {
  // Allocate an array with double the columns as pairs minus one
  const int totalColumns = (totalPairs * 2) - 1;
  const int columnCapacity = totalColumns * 2;
  struct column_layout* cols = new column_layout[columnCapacity];

  for(int i = 0; i < totalPairs; i++) {
    cols[i] = pairs[i];

    // If this is not the last item pair...
    if(i < totalPairs - 1) {
      //...the opposite item should have the left buffer of the NEXT item
      cols[(totalColumns - 1) - i] = column_layout { pairs[i].width, pairs[i + 1].leftBuffer, pairs[i].style };
    }
  }

  // Set columns and total columns with columns created
  row->columns = cols;
  row->totalColumns = totalColumns;
  row->columnCapacity = columnCapacity;
  row->rightBuffer = pairs[0].leftBuffer;
}

void Apply_Row_Template_Custom(struct row_layout* row, struct size_hint rightBuff, struct column_layout* cols, int totalColumns) {
  // Create an array of columns to initialize the row with
  const int columnCapacity = totalColumns * 2;
  struct column_layout* newCols = new column_layout[columnCapacity];

  // Intialize all columns with the same data
  for(int i = 0; i < totalColumns; i++) {
    newCols[i] = cols[i];
  }

  // Set columns and total columns with columns created
  row->columns = newCols;
  row->totalColumns = totalColumns;
  row->columnCapacity = columnCapacity;
  row->rightBuffer = rightBuff;
}

// ROW TEMPLATE VARIADIC

// SIZE FITTING ALGORITHMS
// -----------------------
void Finalize_Window_Sizes(struct window_layout* window) {
  Finalize_Vertical_Window_Sizes(window);
  Finalize_Horizontal_Window_Sizes(window);
}

void Finalize_Vertical_Window_Sizes(struct window_layout* window) {
  // Total vertical sizes is all row heights, all row buffers, and window's bottom buffer
  const int totalVerticalSizes = (2 * window->totalRows) + 1;
  struct size_hint** verticalSizeRefs = new size_hint*[totalVerticalSizes];

  // Put all row heights and buffers into the array of references
  for(int i = 0; i < window->totalRows; i++) {
    verticalSizeRefs[i * 2] = &(window->rows[i].upperBuffer);
    verticalSizeRefs[(i * 2) + 1] = &(window->rows[i].height);
  }
  // Put the bottom buffer of the window into the last place in the reference array
  verticalSizeRefs[totalVerticalSizes - 1] = &(window->bottomBuffer);

  // Make correct rows square within the window
  Enforce_Inheritance(verticalSizeRefs, totalVerticalSizes, window->geometry.w);

  // Fit all vertical sizes into the window's height
  Fit(verticalSizeRefs, totalVerticalSizes, window->geometry.h);

  // De-allocate memory block used to hold the addresses of the sizes
  // But do NOT deallocate the sizes, themselves!
  delete [] verticalSizeRefs;
}

void Finalize_Horizontal_Window_Sizes(struct window_layout* window) {
  // Loop through each row in the window and fit their columns into the window's width
  for(int row = 0; row < window->totalRows; row++) {
    Finalize_Row_Sizes(&window->rows[row], window->geometry.w);
  }
}

void Finalize_Row_Sizes(struct row_layout* row, int windowWidth) {
  // Allocate a block of memory addresses for the column size references
  const int totalHorizontalSizes = (2 * row->totalColumns) + 1;
  struct size_hint** horizontalSizeRefs = new size_hint*[totalHorizontalSizes];

  // Loop through and add references to the current row's column widths and buffers
  for(int col = 0; col < row->totalColumns; col++) {
    horizontalSizeRefs[col * 2] = &row->columns[col].leftBuffer;
    horizontalSizeRefs[(col * 2) + 1] = &row->columns[col].width;
  }
  // Put a reference to the right buffer of the current row in the last place in the array
  horizontalSizeRefs[totalHorizontalSizes - 1] = &row->rightBuffer;

  // Make correct columns or column buffers square within the row
  Enforce_Inheritance(horizontalSizeRefs, totalHorizontalSizes, row->height.size);

  // Fit the references to the column widths/buffers into the window's width
  Fit(horizontalSizeRefs, totalHorizontalSizes, windowWidth);

  // De-allocate memory block used to hold the addresses of the sizes
  // But do NOT deallocate the sizes, themselves!
  delete [] horizontalSizeRefs;
}

void Fit(struct size_hint** sizeRefs, int totalRefs, int containingSpace) {
	// Total space of all the hints
	int totalSpace = Get_Total_Hints(sizeRefs, totalRefs);
	// Space that cannot be adjusted because it has a fixed policy
	int fixedSpace = Get_Fixed_Hints(sizeRefs, totalRefs);
	// Total space that can still be adjusted
	int adjustableSpace = totalSpace - fixedSpace;
	// Space within the containing space that is not taken up by the fixed space
	int availableSpace = containingSpace - fixedSpace;

	// If there is some space that can be adjusted, adjust it
	// If all sizes are fixed, we don't need to bother adjusting anything
	if(adjustableSpace > 0) {
		Adjust_Space(sizeRefs, totalRefs, adjustableSpace, availableSpace);
	}
}

void Adjust_Space(struct size_hint** sizeRefs, int totalRefs,
int adjustableSpace, int availableSpace) {
	float ratio;  // Ratio of current space to adjustable space

	for(int i = 0; i < totalRefs; i++) {
		if(sizeRefs[i]->policy != SIZING_FIXED) {
			// Find the ratio of the hint to adjustable size
			ratio = (float)sizeRefs[i]->hint / (float)adjustableSpace;

			// Reduce adjustable space by the current size hint
			adjustableSpace -= sizeRefs[i]->hint;

			// Adjust size so that it has the same
			// ratio within the available space
			sizeRefs[i]->size = availableSpace * ratio;

			// Make sure the size calculated is acceptable
			Clamp_Size_Hint(sizeRefs[i]);

			// Calculate space that still needs to be filled up
			availableSpace -= sizeRefs[i]->size;
		}
	}
}

int Get_Fixed_Hints(struct size_hint** sizeRefs, int totalRefs) {
	int fixedSpace = 0;

	// Add sizes that have a fixed policy
	for(int i = 0; i < totalRefs; i++) {
		if(sizeRefs[i]->policy == SIZING_FIXED) {
			fixedSpace += sizeRefs[i]->hint;
		}
	}

	return fixedSpace;
}

int Get_Total_Hints(struct size_hint** sizeRefs, int totalRefs) {
	int totalSpace = 0;

	// Accumulate all sizes
	for(int i = 0; i < totalRefs; i++) {
		totalSpace += sizeRefs[i]->hint;
	}

	return totalSpace;
}

// Use the sizing policy of the hint to keep the size constrained, if necessary
void Clamp_Size_Hint(struct size_hint* sizeRef) {
  // If the size is less than the minimum but the policy forbids it...
  if(sizeRef->size < sizeRef->min &&
    (sizeRef->policy == SIZING_MIN || sizeRef->policy == SIZING_CONSTRAINED)) {
    //...clamp size to minimum
    sizeRef->size = sizeRef->min;
  }

  // If the size is greater than the maximum but the policy forbids it...
  if(sizeRef->size > sizeRef->max &&
    (sizeRef->policy == SIZING_MAX || sizeRef->policy == SIZING_CONSTRAINED)) {
    //...clamp size to maximum
    sizeRef->size = sizeRef->max;
  }

  // Make sure the size is not smaller than 0
  if(sizeRef->size < 0) {
    sizeRef->size = 0;
  }
}

void Enforce_Inheritance(struct size_hint** children, int totalChildren, int parentSize) {
  for(int i = 0; i < totalChildren; i++) {
    Inherit_Size(children[i], parentSize);
  }
}

// If the size hint inherits, fix it to the given ratio of the parent size
void Inherit_Size(struct size_hint* child, int parentSize) {
  if(child->policy == SIZING_INHERITED) {
    *child = Fixed_Size((int)(parentSize * child->inheritanceRatio));
  }
}

// LAYOUTING EXECUTION
// -------------------

// Start the layouting rows from the beginning and start an nk (sub)window
int Window_Layout_Begin(struct window_layout* layout, struct nk_context* ctx, nk_flags flags) {
  layout->currentRow = 0;
  layout->layoutFinished = false;
  ctx->style.window = layout->style;
  // Refresh the data on each of the rows
  for(int i = 0; i < layout->totalRows; i++) {
    Row_Layout_Begin(&layout->rows[i]);
  }
  // If this window is a subwindow, start a group
  if(layout->isSubwindow) {
    return nk_group_begin(ctx, layout->title, flags);
  }
  // Otherwise, start a standalone window
  else {
    return nk_begin(ctx, layout->title, layout->geometry, flags);
  }
}

// Set the row to start from the beginning for layouting
void Row_Layout_Begin(struct row_layout* row) {
  row->currentColumn = -1;
  row->layoutFinished = false;
}

// Layout the next part of the window
void Window_Layout_Next(struct window_layout* layout, struct nk_context* ctx) {
  // If the current row is finished layouting...
  if(layout->rows[layout->currentRow].layoutFinished) {
    //...end the current row
    Row_Layout_End(&layout->rows[layout->currentRow], ctx);
    // Go to the next row. Update finished flag
    layout->currentRow++;
    layout->layoutFinished = layout->currentRow >= layout->totalRows;
  }
  // If layouting is not finished...
  if(!layout->layoutFinished) {
    //...layout the next part of the current row
    Row_Layout_Next(&layout->rows[layout->currentRow], ctx);
  }
}

// Layout the next column in the row
void Row_Layout_Next(struct row_layout* row, struct nk_context* ctx) {
  // Enter only if layouting the row is not finished
  if(!row->layoutFinished) {
    // If we are on the first column, allocate the rows
    if(row->currentColumn == -1) {
      // Allocate buffer space only if it is nonzero
      if(row->upperBuffer.size > 0) {
        nk_layout_row_dynamic(ctx, row->upperBuffer.size, 1);
      }

      // Begin the row with double the columns (one column for the column's buffer)
      nk_layout_row_begin(ctx, NK_STATIC, row->height.size, row->totalColumns * 2);
    }

    // Move to the next column and lay it out
    row->currentColumn++;
    Layout_Column(row->columns[row->currentColumn], ctx);

    // Layout is finished if we just laid out the last column
    row->layoutFinished = row->currentColumn >= (row->totalColumns - 1);
  }
}

// Simple function pushes out the column buffer and the column width
// with the specified style set
void Layout_Column(struct column_layout col, struct nk_context* ctx) {
  // Set the style on the context
  ctx->style = col.style;

  // Layout buffer only if it is nonzero
  if(col.leftBuffer.size > 0) {
    nk_layout_row_push_buffer(ctx, col.leftBuffer.size);
  }

  // Push the width of the column item into the row
  nk_layout_row_push(ctx, col.width.size);
}

// This is NOT called by the window and MUST be called by client code
void Window_Layout_End(struct window_layout* layout, struct nk_context* ctx) {
  // End the correct window type
  if(layout->isSubwindow) {
    nk_group_end(ctx);
  }
  else {
    nk_end(ctx);
  }
}

// This is called by the window - client code need not call
void Row_Layout_End(struct row_layout* row, struct nk_context* ctx) {
	(void)row;	// Bypass compiler warning
  nk_layout_row_end(ctx);
}

/*
LAYOUT MODIFICATION
-------------------
*/

void Insert_Row(struct window_layout* window,
struct row_layout row, int index) {
  if(index >= 0 && index <= window->totalRows) {
    // If total rows meets or exceeds the capacity,
	// increase the size of the row array
    if(window->totalRows >= window->rowCapacity) {
      Reserve_Rows(window, window->totalRows * 2);
    }

    // From the last element down to the element of insertion,
    // move each element down in the array once
    for(int i = window->totalRows - 1; i >= index; i--) {
      window->rows[i + 1] = window->rows[i];
    }

    // Assign the given row and increment total rows
    window->rows[index] = row;
    (window->totalRows)++;

	// Re-adjust the sizes to accomodate the newly inserted row
	Finalize_Window_Sizes(window);
  }
}

void Remove_Row(struct window_layout* window, int index) {
  if(index >= 0 && index < window->totalRows) {
    // Overwrite the row to remove by moving all of the rows above it down
    for(int i = index + 1; i < window->totalRows; i++) {
      window->rows[i - 1] = window->rows[i];
    }

    // Decrease total rows
    (window->totalRows)--;

	// Re-adjust the sizes to accomodate
	// the missing space that must be filled up
	Finalize_Window_Sizes(window);
  }
}

void Reserve_Rows(struct window_layout* window, int new_cap) {
  if(new_cap > window->totalRows) {
    struct row_layout* newRows =
		new row_layout[new_cap];

    // Copy the current rows into the new rows
    for(int i = 0; i < window->totalRows; i++) {
      newRows[i] = window->rows[i];
    }

    // Assign the new rows to the window's rows and update capacity
    delete [] window->rows;
    window->rows = newRows;
    window->rowCapacity = new_cap;
  }
}

/*
LAYOUTING HELPERS
-----------------
*/

struct row_layout& Current_Row(const struct window_layout* window) {
  return window->rows[window->currentRow];
}
struct column_layout& Current_Column(const struct window_layout* window) {
  return Current_Column(&Current_Row(window));
}
struct column_layout& Current_Column(const struct row_layout* row) {
  return row->columns[row->currentColumn];
}
struct nk_rect Current_Rect(const struct window_layout* window) {
	return Item_Rect(window, window->currentRow,
		Current_Row(window).currentColumn);
}

struct row_layout& Row(const struct window_layout* window, int row) {
  return window->rows[row];
}
struct column_layout& Column(const struct window_layout* window, int row, int column) {
  return window->rows[row].columns[column];
}
struct column_layout& Column(const struct row_layout* row, int column) {
  return row->columns[column];
}

struct size_hint Free_Size(int hint) {
	size_hint size;
	size.hint = hint;
	size.min = 1;
	size.max = 1;
	size.policy = SIZING_FREE;
	size.size = hint;
	size.inheritanceRatio = 0.0;
	return size;
}
struct size_hint Fixed_Size(int size) {
	size_hint shint;
    shint.hint = size;
    shint.min = 1;
    shint.max = 1;
    shint.policy = SIZING_FIXED;
    shint.size = size;
    shint.inheritanceRatio = 0.0;
    return shint;
}
struct size_hint Lower_Bounded_Size(int hint, int min) {
	size_hint shint;
	shint.hint = hint;
	shint.min = min;
	shint.max = min;
	shint.policy = SIZING_MIN;
	shint.size = hint;
	shint.inheritanceRatio = 0.0;
	return shint;
}
struct size_hint Upper_Bounded_Size(int hint, int max) {
	size_hint shint;
	shint.hint = hint;
	shint.min = max;
	shint.max = max;
	shint.policy = SIZING_MAX;
	shint.size = hint;
	shint.inheritanceRatio = 0.0;
	return shint;
}
struct size_hint Constrained_Size(int hint, int min, int max) {
	size_hint size;
	size.hint = hint;
	size.min = min;
	size.max = max;
	size.policy = SIZING_CONSTRAINED;
	size.size = hint;
	size.inheritanceRatio = 0.0;
	return size;
}
struct size_hint Inherited_Size(float ratio) {
	size_hint size;
	size.hint = 1;
	size.min = 1;
	size.max = 1;
	size.policy = SIZING_INHERITED;
	size.size = 1;
	size.inheritanceRatio = ratio;
	return size;
}
struct column_layout Column_Layout(struct size_hint width,
struct size_hint left_buffer, struct nk_style style) {
	column_layout column;
	column.width = width;
	column.leftBuffer = left_buffer;
	column.style = style;
	return column;
}
struct row_layout Row_Layout(struct size_hint height, struct size_hint upperBuffer) {
	row_layout row;
	row.height = height;
	row.upperBuffer = upperBuffer;
	row.rightBuffer = Free_Size(0);
	row.columns = NULL;
	row.totalColumns = 0;
	row.columnCapacity = 0;
	row.layoutFinished = false;
	return row;
}
struct window_layout Window_Layout(struct nk_rect rect,
struct nk_style_window style, const char* title) {
	window_layout window;

	char* copy = new char[strlen(title) + 1];
	strcpy(copy, title);

	window.geometry = rect;
	window.style = style;
	window.title = copy;
	window.bottomBuffer = Free_Size(0);
	window.isSubwindow = false;
	window.rows = NULL;
	window.totalRows = 0;
	window.rowCapacity = 0;
	window.currentRow = 0;
	window.layoutFinished = false;

	return window;
}
struct window_layout Subwindow_Layout(const struct window_layout* parent, int rowIndex, int columnIndex,
struct nk_style_window style, const char* title) {
	window_layout window = Window_Layout(
		Item_Rect(parent, rowIndex, columnIndex), style, title);
	window.isSubwindow = true;
	return window;
}

struct nk_rect Item_Rect(const struct window_layout* parent, int rowIndex, int columnIndex) {
  struct nk_rect rect = nk_rect(0, 0, 0, 0);  // Rect of this subwindow
  rect.x = parent->geometry.x;
  rect.y = parent->geometry.y;

  // Add buffers and widths for each column in the selected row
  // to get the subwindow's x-coordinate
  for(int col = 0; col < columnIndex; col++) {
    rect.x += parent->rows[rowIndex].columns[col].leftBuffer.size;
    rect.x += parent->rows[rowIndex].columns[col].width.size;
  }
  rect.x += parent->rows[rowIndex].columns[columnIndex].leftBuffer.size;

  // Add buffers and heights of the rows before the selected row
  // to get the subwindow's y-coordinate
  for(int row = 0; row < rowIndex; row++) {
    rect.y += parent->rows[row].upperBuffer.size;
    rect.y += parent->rows[row].height.size;
  }
  rect.y += parent->rows[rowIndex].upperBuffer.size;

  // Width/height of the window is just the width/height of the area it's nested in
  rect.w = parent->rows[rowIndex].columns[columnIndex].width.size;
  rect.h = parent->rows[rowIndex].height.size;

  return rect;
}

nk_flags nk_edit_string_concealed(struct nk_context* ctx, nk_flags flags, char* buffer, int *len, int max, char concealChar, nk_plugin_filter filter) {
  int oldLen = *len;	// Length of the password from the previous render
  char* bufferEdited = new char[max]; // The char buffer actually edited by the text edit box
  nk_flags flagReturn;

  // Make each character in the buffer leading up to the current length equal the concealing character
  for(int index = 0; index < oldLen; ++index) {
    bufferEdited[index] = concealChar;
  }

  // Edit the buffer to be edited
  flagReturn = nk_edit_string(ctx, flags, bufferEdited, len, max, filter);

	// Loop goes through each character added to the buffer edited and adds them to the buffer passed in
	for(int index = oldLen; index < *len; ++index) {
		buffer[index] = bufferEdited[index];
	}

  // De-allocate buffer edited
  delete [] bufferEdited;

  return flagReturn;
}

void nk_layout_row_push_buffer(struct nk_context* ctx, float buffer) {
  nk_layout_row_push(ctx, buffer);
  nk_label(ctx, "", 0);
}

namespace nk_experimental {
	/*
	GENERIC ITERATORS (WARNING: EXPERIMENTAL)
	-----------------------------------------
	*/

	void Window_Iterate_Begin(struct window_layout* window, int startingRowIndex, int startingColumnIndex) {
	  cout << "Beginning iteration on window named \"" << window->title << "\" at row #" << startingRowIndex << " column #" << startingColumnIndex << endl;
	  window->currentRow = startingRowIndex;
	  Row_Iterate_Begin(&Current_Row(window), startingColumnIndex);
	  window->layoutFinished = startingRowIndex >= window->totalRows;
	}

	void Row_Iterate_Begin(struct row_layout* row, int startingColumnIndex) {
	  row->currentColumn = startingColumnIndex - 1;
	  row->layoutFinished = startingColumnIndex >= (row->totalColumns - 1);
	}

	void Window_Iterate_Next_Column(struct window_layout* window) {
	  cout << "Iterating next column on window named \"" << window->title << "\"" << endl;
	  // If the current row is finished layouting...
	  if(Current_Row(window).layoutFinished) {
	    //...end the current row
	    Row_Iterate_End(&Current_Row(window));
	    // Move the window to the next row
	    Window_Iterate_Next_Row(window);
	    // If the next row exists, begin iterating through it
	    if(!window->layoutFinished) {
	      Row_Iterate_Begin(&Current_Row(window));
	    }
	  }

	  // If layouting is not finished...
	  if(!window->layoutFinished) {
	    //...iterate the column of the current row
	    Row_Iterate_Next(&Current_Row(window));
	  }
	}

	void Window_Iterate_Next_Row(struct window_layout* window) {
	  cout << "Iterating next row on window named \"" << window->title << "\"" << endl;
	  // If layouting is finished, go to the next row
	  if(!window->layoutFinished) {
	    window->currentRow++;
	    window->layoutFinished = window->currentRow >= window->totalRows;
	  }
	  else {
	    cout << "WARNING: you cannot iterate to the next row on window named \"" << window->title << "\"\n\tbecause you have already iterated through every row" << endl;
	  }
	}

	void Row_Iterate_Next(struct row_layout* row) {
	  cout << "Iterating next column on row" << endl;
	  if(!row->layoutFinished) {
	    row->currentColumn++;
	    row->layoutFinished = row->currentColumn >= (row->totalColumns - 1);
	  }
	  else {
	    cout << "WARNING: you cannot iterate to the next column on the row\n\tbecause you have already iterated through every column" << endl;
	  }
	}

	void Window_Iterate_End(struct window_layout* window) {
	  window->currentRow = -1;
	  window->layoutFinished = false;
	}

	void Row_Iterate_End(struct row_layout* row) {
	  row->currentColumn = -1;
	  row->layoutFinished = false;
	}
}
