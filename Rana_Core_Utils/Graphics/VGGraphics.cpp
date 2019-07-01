#ifdef VGSUPPORTED
#include "VGGraphics.h"
VGGRAPHICS vg;
int VGGRAPHICS::w;
int VGGRAPHICS::h;

ColorArray Int_To_Color_Array(int icolor) {
	ColorArray acolor;
	uint8_t* color = (uint8_t*)&icolor;
	acolor.color[0] = color[0];
	acolor.color[1] = color[1];
	acolor.color[2] = color[2];
	acolor.color[3] = color[3];
	return acolor;
}

void Initialize_VGGRAPHICS() {
	log_dbg("initializing openvg");
	init(&vg.w, &vg.h);
	log_dbg("loading notosans font");
	vg.font = loadfont(
		NotoSans_glyphPoints,
		NotoSans_glyphPointIndices,
		NotoSans_glyphInstructions,
		NotoSans_glyphInstructionIndices,
		NotoSans_glyphInstructionCounts,
		NotoSans_glyphAdvances,
		NotoSans_characterMap,
		NotoSans_glyphCount);
}

void Finish_VGGRAPHICS() {
	log_dbg("unloading notosans font");
	unloadfont(vg.font.Glyphs, vg.font.Count);
	log_dbg("closing openvg");
	finish();
}

void Start_Picture_VGGRAPHICS() {
	Start(vg.w, vg.h);
}

void End_Picture_VGGRAPHICS() {
	End();
}

void Clear_Screen_VGGRAPHICS(uint8_t color[4]) {
	Background(color[2], color[1], color[0]);
}

void Line_VGGRAPHICS(
int x1, int y1, int x2, int y2, VGfloat thickness, uint8_t color[4]) {
	StrokeWidth(thickness);
	Stroke(color[2], color[1], color[0], color[3] / 255.0);
	Line(x1, vg.h - y1, x2, vg.h - y2);
}

void Rect_VGGRAPHICS(
int x, int y, int w, int h, int rounding, VGfloat thickness, uint8_t color[4]) {
	StrokeWidth(thickness);
	Stroke(color[2], color[1], color[0], color[3] / 255.0);
	if (rounding == 0) {
		RectOutline(x, vg.h - h - y, w, h);
	} else {
		RoundrectOutline(x, vg.h - h - y, w, h, rounding, rounding);
	}
}

void Rect_Filled_VGGRAPHICS(
int x, int y, int w, int h, int rounding, uint8_t color[4]) {
	Fill(color[2], color[1], color[0], color[3] / 255.0);
	if (rounding == 0) {
		Rect(x, vg.h - h - y, w, h);
	} else {
		Roundrect(x, vg.h - h - y, w, h, rounding, rounding);
	}
}

void Ellipse_VGGRAPHICS(
int x, int y, int w, int h, VGfloat thickness, uint8_t color[4]) {
	StrokeWidth(thickness);
	Stroke(color[2], color[1], color[0], color[3] / 255.0);
	EllipseOutline(x + w / 2, vg.h - y - h / 2, w, h);
}

void Ellipse_Filled_VGGRAPHICS(
int x, int y, int w, int h, uint8_t color[4]) {
	Fill(color[2], color[1], color[0], color[3] / 255.0);
	Ellipse(x + w / 2, vg.h - y - h / 2, w, h);
}

void Polygon_VGGRAPHICS(
VGfloat* xp, VGfloat* yp, int np, VGfloat thickness, uint8_t color[4]) {
	StrokeWidth(thickness);
	Stroke(color[2], color[1], color[0], color[3] / 255.0);
	Polyline(xp, yp, np);
}

void Polygon_Filled_VGGRAPHICS(
VGfloat* xp, VGfloat* yp, int np, uint8_t color[4]) {
	Fill(color[2], color[1], color[0], color[3] / 255.0);
	Polygon(xp, yp, np);
}

void Text_VGGRAPHICS(
int x, int y, const char* text, int h, uint8_t color[4]) {
	Fill(color[0], color[1], color[2], color[3] / 255.0);
	//the h / 4 term in the y compontent is an approximation
	Text(x, vg.h - h - h / 4 - y, text, vg.font, h);
}

void Image_VGGRAPHICS(int x, int y, int w, int h, uint8_t* data) {
	VGbitfield quality = VG_IMAGE_QUALITY_NONANTIALIASED;
	VGImageFormat format = VG_sARGB_8888;
	VGImage img = vgCreateImage(format, w, h, quality);
	vgImageSubData(img, (void*)data, w * 4, format, 0, 0, w, h);
	vgSeti(VG_IMAGE_QUALITY, quality);
	vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgScale(1.0, -1.0);
	vgTranslate(x, -(vg.h - h - y) - h);
	vgDrawImage(img);
	vgDestroyImage(img);
	vgLoadIdentity();
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
}

void Quarter_Arc_Filled_VGGRAPHICS(
int x, int y, int w, int h, int q, uint8_t color[4]) {
	int sa = 0;
	if (q == FIRST_QUADRANT) {
		sa = 0;
	} else if (q == SECOND_QUADRANT) {
		sa = 90;
	} else if (q == THIRD_QUADRANT) {
		sa = 180;
	} else if (q == FOURTH_QUADRANT) {
		sa = 270;
	}
	
	Fill(color[2], color[1], color[0], color[3] / 255.0);
	Arc(x + w, vg.h - y, w * 2, h * 2, sa, 90);

	VGfloat xp[3];
	VGfloat yp[3];

	//TODO: this triangle is only correct for Quadrant 3
	xp[0] = x;
	xp[1] = x + w;
	xp[2] = x + w;
	yp[0] = vg.h - y;
	yp[1] = vg.h - y;
	yp[2] = vg.h - h - y;

	Fill(color[2], color[1], color[0], color[3] / 255.0);
	Polygon(xp, yp, 3);
}

#endif
