#include "Breadth_Buffer_Pair.h"

BREADTH_BUFFER_PAIR Buffer_And_Breadth(
LAYOUT_SIZE buffer, LAYOUT_SIZE breadth) {
	BREADTH_BUFFER_PAIR pair;
	pair.buffer = buffer;
	pair.breadth = breadth;
	return pair;
}
BREADTH_BUFFER_PAIR Breadth(LAYOUT_SIZE breadth) {
	return Buffer_And_Breadth(Ratio_Of_Total(0), breadth);
}

BREADTH_BUFFER_PAIR Evenly_Spaced_Item(
uint8_t n, float r, bool side_buffers) {
	uint8_t buffers = side_buffers ? n + 1 : n - 1;
	float d = (buffers * r) + n;
	return Buffer_And_Breadth(Ratio_Of_Total(r / d), Ratio_Of_Total(1.0 / d));
}
BREADTH_BUFFER_PAIR Evenly_Spaced_Item(
LAYOUT_SIZE breadth, const struct nk_context* ctx, uint8_t items,
bool side_buffers) {
	uint32_t current_width = Current_Usable_Panel_Width(ctx);
	uint8_t total_buffers = side_buffers ? items + 1 : items - 1;
	uint32_t breadth_size = Exact_Size_Horizontal(
	&breadth, ctx, current_width);
	uint32_t buffer_size =
	(current_width - items * breadth_size) / total_buffers;

	return Evenly_Spaced_Item(
	items, (float)buffer_size / breadth_size, side_buffers);
}

void Evenly_Spaced_Items(
BREADTH_BUFFER_PAIR* pairs, LAYOUT_SIZE breadth,
const struct nk_context* ctx, uint8_t num_items, bool side_buffers) {
	BREADTH_BUFFER_PAIR item = Evenly_Spaced_Item(
	breadth, ctx, num_items, side_buffers);

	for (uint8_t i = 0; i < num_items; i++) {
		if (i == 0 && !side_buffers) {
			pairs[i] = Breadth(item.breadth);
		}
		else {
			pairs[i] = item;
		}
	}
}

uint32_t Exact_Buffer(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	return Exact_Size(&pair->buffer, ctx, o, remaining_horizontal_space);
}
uint32_t Exact_Buffer_Vertical(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx) {
	return Exact_Buffer(pair, ctx, VERTICAL, 0);
}
uint32_t Exact_Size_Horizontal(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx,
uint32_t remaining_horizontal_space) {
	return Exact_Buffer(pair, ctx, HORIZONTAL, remaining_horizontal_space);
}

uint32_t Exact_Breadth(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx, ORIENTATION o,
uint32_t remaining_horizontal_space) {
	return Exact_Size(&pair->breadth, ctx, o, remaining_horizontal_space);
}
uint32_t Exact_Breadth_Vertical(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx) {
	return Exact_Breadth(pair, ctx, VERTICAL, 0);
}
uint32_t Exact_Breadth_Horizontal(
const BREADTH_BUFFER_PAIR* pair, const struct nk_context* ctx,
uint32_t remaining_horizontal_space) {
	return Exact_Breadth(pair, ctx, HORIZONTAL, remaining_horizontal_space);
}
