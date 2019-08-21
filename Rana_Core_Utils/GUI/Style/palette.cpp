#include "palette.h"

nk_color lighter(nk_color original) {
  return nk_rgba(original.r + DIFF.r, original.g + DIFF.g, original.b + DIFF.b, original.a);
}
nk_color darker(nk_color original) {
  return nk_rgba(original.r - DIFF.r, original.g - DIFF.g, original.b - DIFF.b, original.a);
}

// Return true if the color is "bright" (more white than black)
bool bright(const struct nk_color& color) {
  int sum = color.r + color.g + color.b;
  return sum >= 400;
}

struct nk_color rb_flip (const struct nk_color& original) {
  return nk_rgba(original.b, original.g, original.r, original.a);
}

/*
EXPERIMENTAL
*/

PALETTE_COLOR PALETTE::colors[MAX_COLORS] = {};

void Set_Color(struct nk_color c, uint8_t i) {
	if (i < MAX_COLORS) {
		PALETTE::colors[i] = Palette_Color(c);
	}
	else {
		log_err("Palette cannot hold a color at index " + to_string((int)i));
	}
}

struct nk_color Get_Color(uint8_t i) {
	// Check if index is in range
	if (i < MAX_COLORS) {

		// Check if requested color exists
		PALETTE_COLOR c = PALETTE::colors[i];
		if (c.assigned) {
			return c.color;
		}
		else {
			log_err("Palette slot " + to_string((int)i) +
			" has not been assigned a color");
			return nk_rgba(255, 255, 255, 255);
		}
	}
	else {
		log_err("Palette cannot hold a color at index " + to_string((int)i));
		return nk_rgba(255, 255, 255, 255);
	}
}
