#include "Layout_Utilities.h"

struct nk_vec2 Centered(struct nk_rect parent_rect, struct nk_vec2 child_size) {
	return nk_vec2(
	parent_rect.x + ((parent_rect.w - child_size.x) / 2),
	parent_rect.y + ((parent_rect.h - child_size.y) / 2));
}
