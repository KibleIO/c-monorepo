#include "nk_ops.h"

bool Vec2_Cmp(struct nk_vec2 a, struct nk_vec2 b) {
	return a.x == b.x && a.y == b.y;
}

string Rect_Str(struct nk_rect rect) {
	return to_string(rect.x) + ", " + to_string(rect.y) + ", " +
	to_string(rect.w) + ", " + to_string(rect.h);
}

string Vec2_Str(struct nk_vec2 vec) {
	ostringstream sout;
	sout << '(' << vec.x << ", " << vec.y << ')';
	return sout.str();
}

string Color_Str(struct nk_color color) {
	ostringstream sout;
	sout << hex;
	sout << "(" << (int)color.r << ", " << (int)color.g << ", "
	<< (int)color.b << ", " << (int)color.a << ")";
	return sout.str();
}
