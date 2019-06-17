#ifndef BREADTH_BUFFER_PAIR_H_
#define BREADTH_BUFFER_PAIR_H_

#include "Layout_Ratio.h"

struct BREADTH_BUFFER_PAIR {
	LAYOUT_RATIO buffer;
	LAYOUT_RATIO breadth;
};

BREADTH_BUFFER_PAIR Buffer_And_Breadth(
LAYOUT_RATIO buffer, LAYOUT_RATIO breadth);
BREADTH_BUFFER_PAIR Breadth(LAYOUT_RATIO breadth);

BREADTH_BUFFER_PAIR Evenly_Spaced_Item(
uint8_t num_items, float buffer_to_breadth_ratio, bool side_buffers);
BREADTH_BUFFER_PAIR Evenly_Spaced_Item(
LAYOUT_RATIO breadth, const struct nk_context*, uint8_t num_items,
bool side_buffers);

void Evenly_Spaced_Items(
BREADTH_BUFFER_PAIR* pairs, LAYOUT_RATIO breadth,
const struct nk_context*, uint8_t num_items, bool side_buffers);


// ACCESSORS

uint32_t Exact_Buffer(
const BREADTH_BUFFER_PAIR*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
uint32_t Exact_Buffer_Vertical(
const BREADTH_BUFFER_PAIR*, const struct nk_context*);
uint32_t Exact_Buffer_Horizontal(
const BREADTH_BUFFER_PAIR*, const struct nk_context*,
uint32_t remaining_horizontal_space);

uint32_t Exact_Breadth(
const BREADTH_BUFFER_PAIR*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
uint32_t Exact_Breadth_Vertical(
const BREADTH_BUFFER_PAIR*, const struct nk_context*);
uint32_t Exact_Breadth_Horizontal(
const BREADTH_BUFFER_PAIR*, const struct nk_context*,
uint32_t remaining_horizontal_space);

#endif
