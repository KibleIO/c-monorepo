#include "palette.h"

nk_color lighter(nk_color original) {
  return nk_rgba(original.r + DIFF.r, original.g + DIFF.g, original.b + DIFF.b, original.a);
}
nk_color darker(nk_color original) {
  return nk_rgba(original.r - DIFF.r, original.g - DIFF.g, original.b - DIFF.b, original.a);
}

// Given rgba components, gives a color digestible by the Graphics class
Color rgba(char r, char g, char b, char a) {
	Color returnme;

	((char*)&returnme)[0] = b;
	((char*)&returnme)[1] = g;
	((char*)&returnme)[2] = r;
	((char*)&returnme)[3] = a;

	return returnme;
}

Color fromNkColor(const struct nk_color& color) {
	return rgba(color.r, color.g, color.b, color.a);
}

// Return true if the color is "bright" (more white than black)
bool bright(const struct nk_color& color) {
  int sum = color.r + color.g + color.b;
  return sum >= 400;
}

struct nk_color rb_flip (const struct nk_color& original) {
  return nk_rgba(original.b, original.g, original.r, original.a);
}
