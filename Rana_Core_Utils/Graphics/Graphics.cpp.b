//PLATFORMS: Linux, Windows, OSX

#include "Graphics.h"

void Initialize_GRAPHICS(GRAPHICS* graphics, char* buffer, int width, int height) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)buffer;
	(void)width;
	(void)height;
	graphics->Width = width;
	graphics->Height = height;
#else
	graphics->Buffer      = buffer;
	graphics->Width       = width;
	graphics->Height      = height;
	graphics->Width_Clip  = width;
	graphics->Height_Clip = height;
	graphics->X_clip      = 0;
	graphics->Y_clip      = 0;
	graphics->ContourX    = new long[graphics->Height * 2];
	graphics->Transparent = false;
#endif
}

#ifdef GRAPHICS_USING_HARDWARE
void Start_Picture_GRAPHICS(GRAPHICS* graphics, int width, int height) {
	if (width == 0) {
		width = graphics->Width;
	}
	if (height == 0) {
		height = graphics->Height;
	}
	Start(width, height);
}

void End_Picture_GRAPHICS() {
	End();
}

void fasterimage(VGfloat x, VGfloat y, int w, int h, VGubyte* data) {
	unsigned int dstride = w * 4;
	VGImageFormat rgbaFormat = VG_sARGB_8888;
	VGImage img = vgCreateImage(rgbaFormat, w, h, VG_IMAGE_QUALITY_NONANTIALIASED);
	vgImageSubData(img, (void*)data, dstride, rgbaFormat, 0, 0, w, h);
	vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_NONANTIALIASED);
	vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgTranslate(x, y);
	vgDrawImage(img);
	vgLoadIdentity();
	vgDestroyImage(img);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
}
#endif

void Delete_GRAPHICS(GRAPHICS* graphics) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	finish();
#else
	delete [] graphics->ContourX;
#endif
}

void SwapBuffers_GRAPHICS(GRAPHICS* graphics, char* buff) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	(void)buff;
#else
	graphics->Buffer = buff;
#endif
}

void ClearScreen_GRAPHICS(GRAPHICS* graphics, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	
	Background(color_bytes[2], color_bytes[1], color_bytes[0]);
#else
	int *buff = (int*)graphics->Buffer;
	for (int y = graphics->Height - 1; y >= 0; y--) {
		for (int x = graphics->Width - 1; x >= 0; x--) {
			buff[y * graphics->Width + x] = color;
		}
	}
#endif
}

void Set_Clip_GRAPHICS(GRAPHICS* graphics, int x, int y, int width, int height) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	(void)x;
	(void)y;
	(void)width;
	(void)height;
#else
	if (x < 0 || x >= graphics->Width || y < 0 || y >= graphics->Height) {
		graphics->X_clip      = 0;
		graphics->Y_clip      = 0;
		graphics->Width_Clip  = graphics->Width;
		graphics->Height_Clip = graphics->Height;
	} else {
	  if ((width + x) > graphics->Width) {
			width = (graphics->Width - x);
	  }
	  if ((height + y) > graphics->Height) {
			height = (graphics->Height - y);
	  }
	  graphics->X_clip      = x;
	  graphics->Y_clip      = y;
	  graphics->Width_Clip  = width;
	  graphics->Height_Clip = height;
	}
#endif
}

void Clip_GRAPHICS(GRAPHICS* graphics, int &x, int &y, int &w, int &h) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	(void)x;
	(void)y;
	(void)w;
	(void)h;
#else
	uint8_t invalid = false;
	if (x < 0 || y < 0) {
		log_err("negative coordinates not implemented");
		invalid = true;
	}

	if (x >= graphics->Width_Clip + graphics->X_clip ||
	y >= graphics->Height_Clip + graphics->Y_clip) {
		log_err("offscreen coordinates not allowed");
		invalid = true;
	}

	if (invalid) {
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		return;
	}

	if (x < graphics->X_clip) {
		w -= graphics->X_clip - x;
		x = graphics->X_clip;
	}

	if (w < 0) {
		w = 0;
	}

	if (y < graphics->Y_clip) {
		h -= graphics->Y_clip - y;
		y = graphics->Y_clip;
	}

	if (h < 0) {
		h = 0;
	}

	if (x + w > graphics->Width_Clip + graphics->X_clip) {
		w = graphics->Width_Clip + graphics->X_clip - x;
	}

	if (y + h > graphics->Height_Clip + graphics->X_clip) {
		h = graphics->Height_Clip + graphics->Y_clip - y;
	}
#endif
}

void DrawPoint_GRAPHICS(GRAPHICS* graphics, int x, int y, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;

	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Rect(x, graphics->Height - y, 1, 1);
#else
	if (graphics->Transparent) {
		DrawPoint_Transparent_GRAPHICS(graphics, x, y, color);
	} else {
		DrawPoint_Opaque_GRAPHICS(graphics, x, y, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void DrawPoint_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int color) {
	if (graphics->Transparent) {
		DrawPoint_Transparent_GRAPHICS_UNSAFE(graphics, x, y, color);
	} else {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x, y, color);
	}
}

void DrawPoint_Transparent_GRAPHICS(GRAPHICS* graphics, int x, int y, int col1) {
	if (x < graphics->Width_Clip + graphics->X_clip && x >= graphics->X_clip && y < graphics->Height_Clip + graphics->Y_clip && y >= graphics->Y_clip) {
		unsigned char* fg = (unsigned char*) &col1;
		unsigned char* bg = (unsigned char*) &((int*)graphics->Buffer)[x + (y * graphics->Width)];
		unsigned int result;
		unsigned int alpha = fg[3] + 1;
    unsigned int inv_alpha = 256 - fg[3];

    ((unsigned char*)&result)[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
    ((unsigned char*)&result)[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
    ((unsigned char*)&result)[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
    ((unsigned char*)&result)[3] = 0xFF;
		((int*)graphics->Buffer)[x + (y * graphics->Width)] = result;
	}
}

void DrawPoint_Transparent_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int col1) {
	unsigned char* fg = (unsigned char*) &col1;
	unsigned int alpha = fg[3] + 1;
	if (alpha == 256) {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x, y, col1);
		return;
	}
	unsigned char* bg = (unsigned char*) &((int*)graphics->Buffer)[x + (y * graphics->Width)];
	unsigned int result;
  unsigned int inv_alpha = 256 - fg[3];

  ((unsigned char*)&result)[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
  ((unsigned char*)&result)[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
  ((unsigned char*)&result)[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
  ((unsigned char*)&result)[3] = 0xff;
	((int*)graphics->Buffer)[x + (y * graphics->Width)] = result;
}

void DrawPoint_Opaque_GRAPHICS(GRAPHICS* graphics, int x, int y, int color) {
	if (x < graphics->Width_Clip + graphics->X_clip && x >= graphics->X_clip && y < graphics->Height_Clip + graphics->Y_clip && y >= graphics->Y_clip) {
 		((int*)graphics->Buffer)[x + (y * graphics->Width)]= color;
	}
}

void DrawPoint_Opaque_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int color) {
	((int*)graphics->Buffer)[x + (y * graphics->Width)]= color;
}
#endif

void DrawLine_GRAPHICS(GRAPHICS* graphics, int x1, int y1, int x2, int y2, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	//TODO guessing that line is a stroke, maybe wrong
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;

	StrokeWidth(1);
	Stroke(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Line(x1, graphics->Height - y1, x2, graphics->Height - y2);
#else
	if (graphics->Transparent) {
		DrawLine_Transparent_GRAPHICS(graphics, x1, y1, x2, y2, color);
	} else {
		DrawLine_Opaque_GRAPHICS(graphics, x1, y1, x2, y2, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void DrawLine_Transparent_GRAPHICS(GRAPHICS* graphics, int x1, int y1, int x2, int y2, int color) {
	int dx, dy, inx, iny, e;

	dx = x2 - x1;
	dy = y2 - y1;
	inx = dx > 0 ? 1 : -1;
	iny = dy > 0 ? 1 : -1;

	dx = ABS(dx);
	dy = ABS(dy);

	if(dx >= dy) {
	  dy <<= 1;
	  e = dy - dx;
	  dx <<= 1;
	  while (x1 != x2) {
	    DrawPoint_Transparent_GRAPHICS(graphics, x1, y1, color);
	    if(e >= 0) {
	        y1 += iny;
	        e-= dx;
	    }
	    e += dy; x1 += inx;
	  }
	} else {
	  dx <<= 1;
	  e = dx - dy;
	  dy <<= 1;
	  while (y1 != y2) {
	    DrawPoint_Transparent_GRAPHICS(graphics, x1, y1, color);
	    if(e >= 0) {
	        x1 += inx;
	        e -= dy;
	    }
	    e += dx; y1 += iny;
	  }
	}
	DrawPoint_Transparent_GRAPHICS(graphics, x1, y1, color);
}

void DrawLine_Opaque_GRAPHICS(GRAPHICS* graphics, int x1, int y1, int x2, int y2, int color) {
	int dx, dy, inx, iny, e;

	dx = x2 - x1;
	dy = y2 - y1;
	inx = dx > 0 ? 1 : -1;
	iny = dy > 0 ? 1 : -1;

	dx = ABS(dx);
	dy = ABS(dy);

	if(dx >= dy) {
	  dy <<= 1;
	  e = dy - dx;
	  dx <<= 1;
	  while (x1 != x2) {
	    DrawPoint_Opaque_GRAPHICS(graphics, x1, y1, color);
	    if(e >= 0) {
	        y1 += iny;
	        e-= dx;
	    }
	    e += dy; x1 += inx;
	  }
	} else {
	  dx <<= 1;
	  e = dx - dy;
	  dy <<= 1;
	  while (y1 != y2) {
	    DrawPoint_Opaque_GRAPHICS(graphics, x1, y1, color);
	    if(e >= 0) {
	        x1 += inx;
	        e -= dy;
	    }
	    e += dx; y1 += iny;
	  }
	}
	DrawPoint_Opaque_GRAPHICS(graphics, x1, y1, color);
}

void ScanLine_GRAPHICS(GRAPHICS* graphics, long x1, long y1, long x2, long y2) {
  long sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

  sx = x2 - x1;
  sy = y2 - y1;

  if ((sy < 0) || (sy == 0 && sx < 0)) {
    k = x1; x1 = x2; x2 = k;
    k = y1; y1 = y2; y2 = k;
    sx = -sx;
    sy = -sy;
  }

  if (sx > 0) dx1 = 1;
  else if (sx < 0) dx1 = -1;
  else dx1 = 0;

  if (sy > 0) dy1 = 1;
  else if (sy < 0) dy1 = -1;
  else dy1 = 0;

  m = ABS(sx);
  n = ABS(sy);
  dx2 = dx1;
  dy2 = 0;

  if (m < n) {
    m = ABS(sy);
    n = ABS(sx);
    dx2 = 0;
    dy2 = dy1;
  }

  x = x1; y = y1;
  cnt = m + 1;
  k = n / 2;

  while (cnt--) {
    if ((y >= 0) && (y < graphics->Width)) {
      if (x < graphics->ContourX[y * graphics->Height + 0]) {
				graphics->ContourX[y * graphics->Height + 0] = x;
			}
      if (x > graphics->ContourX[y * graphics->Height + 1]) {
				graphics->ContourX[y * graphics->Height + 1] = x;
			}
    }

    k += n;
    if (k < m) {
      x += dx2;
      y += dy2;
    } else {
      k -= m;
      x += dx1;
      y += dy1;
    }
  }
}
#endif

void FillTriangle_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int x1, int y1, int x2, int y2, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	VGfloat* x = new VGfloat[3];
	VGfloat* y = new VGfloat[3];

	x[0] = x0;
	x[1] = x1;
	x[2] = x2;
	y[0] = graphics->Height - y0;
	y[1] = graphics->Height - y1;
	y[2] = graphics->Height - y2;

	StrokeWidth(0);
	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Polygon(x, y, 3);
#else
	if (graphics->Transparent) {
		FillTriangle_Transparent_GRAPHICS(graphics, x0, y0, x1, y1, x2, y2, color);
	} else {
		FillTriangle_Opaque_GRAPHICS(graphics, x0, y0, x1, y1, x2, y2, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void FillTriangle_Transparent_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int x1, int y1, int x2, int y2, int color) {
	long y;

  for (y = 0; y < graphics->Height; y++) {
    graphics->ContourX[y * graphics->Height + 0] = LONG_MAX; // min X
    graphics->ContourX[y * graphics->Height + 1] = LONG_MIN; // max X
  }

  ScanLine_GRAPHICS(graphics, x0, y0, x1, y1);
  ScanLine_GRAPHICS(graphics, x1, y1, x2, y2);
  ScanLine_GRAPHICS(graphics, x2, y2, x0, y0);

  for (y = 0; y < graphics->Height; y++) {
    if (graphics->ContourX[y * graphics->Height + 1] >= graphics->ContourX[y * graphics->Height + 0]) {
      long x = graphics->ContourX[y * graphics->Height + 0];
      long len = 1 + graphics->ContourX[y * graphics->Height + 1] - graphics->ContourX[y * graphics->Height + 0];

	  	DrawLine_Transparent_GRAPHICS(graphics, x, y, x + len, y, color);
    }
  }
}

void FillTriangle_Opaque_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int x1, int y1, int x2, int y2, int color) {
	long y;

  for (y = 0; y < graphics->Height; y++) {
		graphics->ContourX[y * graphics->Height + 0] = LONG_MAX; // min X
    graphics->ContourX[y * graphics->Height + 1] = LONG_MIN; // max X
  }

  ScanLine_GRAPHICS(graphics, x0, y0, x1, y1);
  ScanLine_GRAPHICS(graphics, x1, y1, x2, y2);
  ScanLine_GRAPHICS(graphics, x2, y2, x0, y0);

	for (y = 0; y < graphics->Height; y++) {
    if (graphics->ContourX[y * graphics->Height + 1] >= graphics->ContourX[y * graphics->Height + 0]) {
      long x = graphics->ContourX[y * graphics->Height + 0];
      long len = 1 + graphics->ContourX[y * graphics->Height + 1] - graphics->ContourX[y * graphics->Height + 0];

	  	DrawLine_Opaque_GRAPHICS(graphics, x, y, x + len, y, color);
    }
  }
}

void DrawLine_Height_GRAPHICS(GRAPHICS* graphics, int x, int y, int height, int color) {
	if (graphics->Transparent) {
		DrawLine_Height_Transparent_GRAPHICS(graphics, x, y, height, color);
	} else {
		DrawLine_Height_Opaque_GRAPHICS(graphics, x, y, height, color);
	}
}

void DrawLine_Height_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int height, int color) {
	if (graphics->Transparent) {
		DrawLine_Height_Transparent_GRAPHICS_UNSAFE(graphics, x, y, height, color);
	} else {
		DrawLine_Height_Opaque_GRAPHICS_UNSAFE(graphics, x, y, height, color);
	}
}

void DrawLine_Height_Transparent_GRAPHICS(GRAPHICS* graphics, int x, int y, int height, int color) {
	if (x < graphics->X_clip || x > graphics->Width_Clip + graphics->X_clip) return;
	if (y < graphics->Y_clip) { height -= graphics->Y_clip - y; y = graphics->Y_clip; }
	if (height <= 0) return;
	if (height + y > graphics->Height_Clip + graphics->Y_clip) height = (graphics->Height_Clip + graphics->Y_clip) - y;

	for (int i = 0; i < height; i++) {
		DrawPoint_Transparent_GRAPHICS_UNSAFE(graphics, x, y + i, color);
	}
}

void DrawLine_Height_Transparent_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int height, int color) {
	for (int i = 0; i < height; i++) {
		DrawPoint_Transparent_GRAPHICS_UNSAFE(graphics, x, y + i, color);
	}
}

void DrawLine_Height_Opaque_GRAPHICS(GRAPHICS* graphics, int x, int y, int height, int color) {
	if (x < graphics->X_clip || x > graphics->Width_Clip + graphics->X_clip) return;
	if (y < graphics->Y_clip) { height -= graphics->Y_clip - y; y = graphics->Y_clip; }
	if (height <= 0) return;
	if (height + y > graphics->Height_Clip + graphics->Y_clip) height = (graphics->Height_Clip + graphics->Y_clip) - y;

	for (int i = 0; i < height; i++) {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x, y + i, color);
	}
}

void DrawLine_Height_Opaque_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int height, int color) {
	for (int i = 0; i < height; i++) {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x, y + i, color);
	}
}

void DrawLine_Width_GRAPHICS(GRAPHICS* graphics, int x, int y, int width, int color) {
	if (graphics->Transparent) {
		DrawLine_Width_Transparent_GRAPHICS(graphics, x, y, width, color);
	} else {
		DrawLine_Width_Opaque_GRAPHICS(graphics, x, y, width, color);
	}
}

void DrawLine_Width_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int width, int color) {
	if (graphics->Transparent) {
		DrawLine_Width_Transparent_GRAPHICS_UNSAFE(graphics, x, y, width, color);
	} else {
		DrawLine_Width_Opaque_GRAPHICS_UNSAFE(graphics, x, y, width, color);
	}
}

void DrawLine_Width_Transparent_GRAPHICS(GRAPHICS* graphics, int x, int y, int width, int color) {
	if (y < graphics->Y_clip || y > graphics->Height_Clip + graphics->Y_clip) return;
	if (x < graphics->X_clip) { width -= graphics->X_clip - x; x = graphics->X_clip; }
	if (width <= 0) return;
	if (width + x > graphics->Width_Clip + graphics->X_clip) width = (graphics->Width_Clip + graphics->X_clip) - x;

	for (int i = 0; i < width; i++) {
		DrawPoint_Transparent_GRAPHICS_UNSAFE(graphics, x + i, y, color);
	}
}

void DrawLine_Width_Transparent_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int width, int color) {
	for (int i = 0; i < width; i++) {
		DrawPoint_Transparent_GRAPHICS_UNSAFE(graphics, x + i, y, color);
	}
}

void DrawLine_Width_Opaque_GRAPHICS(GRAPHICS* graphics, int x, int y, int width, int color) {
	if (y < graphics->Y_clip || y > graphics->Height_Clip + graphics->Y_clip) return;
	if (x < graphics->X_clip) { width -= graphics->X_clip - x; x = graphics->X_clip; }
	if (width <= 0) return;
	if (width + x > graphics->Width_Clip + graphics->X_clip) width = (graphics->Width_Clip + graphics->X_clip) - x;

	for (int i = 0; i < width; i++) {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x + i, y, color);
	}
}

void DrawLine_Width_Opaque_GRAPHICS_UNSAFE(GRAPHICS* graphics, int x, int y, int width, int color) {
	for (int i = 0; i < width; i++) {
		DrawPoint_Opaque_GRAPHICS_UNSAFE(graphics, x + i, y, color);
	}
}
#endif

void DrawSquare_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	StrokeWidth(1);
	Stroke(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Fill(0, 0, 0, 0);
	RectOutline(x, graphics->Height - h - y, w, h);
#else
	if (graphics->Transparent) {
		DrawSquare_Transparent_GRAPHICS(graphics, x, y, w, h, color);
	} else {
		DrawSquare_Opaque_GRAPHICS(graphics, x, y, w, h, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void DrawSquare_Transparent_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
	if (x > graphics->Width_Clip + graphics->X_clip) return;
	if (y > graphics->Height_Clip + graphics->Y_clip) return;
	if (x < graphics->X_clip) { w -= graphics->X_clip - x; x = graphics->X_clip; }
	if (y < graphics->Y_clip) { h -= graphics->Y_clip - y; y = graphics->Y_clip; }
	if (w <= 0) return;
	if (h <= 0) return;
	if (x + w > graphics->Width_Clip  + graphics->X_clip) w = (graphics->Width_Clip  + graphics->X_clip) - x;
	if (y + h > graphics->Height_Clip + graphics->Y_clip) h = (graphics->Height_Clip + graphics->Y_clip) - y;

	DrawLine_Width_Transparent_GRAPHICS_UNSAFE(graphics, x, y, w, color);
	DrawLine_Height_Transparent_GRAPHICS_UNSAFE(graphics, x, y, h, color);
	DrawLine_Width_Transparent_GRAPHICS_UNSAFE(graphics, x, y + h, w, color);
	DrawLine_Height_Transparent_GRAPHICS_UNSAFE(graphics, x + w, y, h, color);
}

void DrawSquare_Opaque_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
	if (x > graphics->Width_Clip + graphics->X_clip) return;
	if (y > graphics->Height_Clip + graphics->Y_clip) return;
	if (x < graphics->X_clip) { w -= graphics->X_clip - x; x = graphics->X_clip; }
	if (y < graphics->Y_clip) { h -= graphics->Y_clip - y; y = graphics->Y_clip; }
	if (w <= 0) return;
	if (h <= 0) return;
	if (x + w > graphics->Width_Clip  + graphics->X_clip) w = (graphics->Width_Clip  + graphics->X_clip) - x;
	if (y + h > graphics->Height_Clip + graphics->Y_clip) h = (graphics->Height_Clip + graphics->Y_clip) - y;

	DrawLine_Width_Opaque_GRAPHICS_UNSAFE(graphics, x, y, w, color);
	DrawLine_Height_Opaque_GRAPHICS_UNSAFE(graphics, x, y, h, color);
	DrawLine_Width_Opaque_GRAPHICS_UNSAFE(graphics, x, y + h, w, color);
	DrawLine_Height_Opaque_GRAPHICS_UNSAFE(graphics, x + w, y, h, color);
}
#endif

void FillSquare_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	StrokeWidth(0);
	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Rect(x, graphics->Height - h - y, w, h);
#else
	if (graphics->Transparent) {
		FillSquare_Transparent_GRAPHICS(graphics, x, y, w, h, color);
	} else {
		FillSquare_Opaque_GRAPHICS(graphics, x, y, w, h, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void FillSquare_Transparent_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
	if (x > graphics->Width_Clip + graphics->X_clip) {
		return;
	}
	if (y > graphics->Height_Clip + graphics->Y_clip) {
		return;
	}
	if (x < graphics->X_clip) {
		w -= graphics->X_clip - x;
		x = graphics->X_clip;
	}
	if (y < graphics->Y_clip) {
		h -= graphics->Y_clip - y;
		y = graphics->Y_clip;
	}
	if (w <= 0) {
		return;
	}
	if (h <= 0) {
		return;
	}
	if (x + w > graphics->Width_Clip  + graphics->X_clip) {
		w = (graphics->Width_Clip  + graphics->X_clip) - x;
	}
	if (y + h > graphics->Height_Clip + graphics->Y_clip) {
		h = (graphics->Height_Clip + graphics->Y_clip) - y;
	}

	for (int i = 0; i < h; i++) {
		DrawLine_Width_Transparent_GRAPHICS_UNSAFE(graphics, x, y + i, w, color);
	}
}

void FillSquare_Opaque_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int color) {
	if (x > graphics->Width_Clip + graphics->X_clip) return;
	if (y > graphics->Height_Clip + graphics->Y_clip) return;
	if (x < graphics->X_clip) { w -= graphics->X_clip - x; x = graphics->X_clip; }
	if (y < graphics->Y_clip) { h -= graphics->Y_clip - y; y = graphics->Y_clip; }
	if (w <= 0) return;
	if (h <= 0) return;
	if (x + w > graphics->Width_Clip  + graphics->X_clip) w = (graphics->Width_Clip  + graphics->X_clip) - x;
	if (y + h > graphics->Height_Clip + graphics->Y_clip) h = (graphics->Height_Clip + graphics->Y_clip) - y;

	for (int i = 0; i < h; i++) {
		DrawLine_Width_Opaque_GRAPHICS_UNSAFE(graphics, x, y + i, w, color);
	}
}
#endif

void DrawCircle_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	StrokeWidth(1);
	Stroke(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Fill(0, 0, 0, 0);
	CircleOutline(x0, graphics->Height - y0, radius);
#else
	if (graphics->Transparent) {
		DrawCircle_Transparent_GRAPHICS(graphics, x0, y0, radius, color);
	} else {
		DrawCircle_Opaque_GRAPHICS(graphics, x0, y0, radius, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void DrawCircle_Transparent_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		DrawPoint_Transparent_GRAPHICS(graphics, x + x0, y + y0, c);
		DrawPoint_Transparent_GRAPHICS(graphics, -x + x0, y + y0, c);

		DrawPoint_Transparent_GRAPHICS(graphics, y + x0, x + y0, c);
		DrawPoint_Transparent_GRAPHICS(graphics, -y + x0, x + y0, c);

		DrawPoint_Transparent_GRAPHICS(graphics, -x + x0, -y + y0, c);
		DrawPoint_Transparent_GRAPHICS(graphics, x + x0, -y + y0, c);

		DrawPoint_Transparent_GRAPHICS(graphics, -y + x0, -x + y0, c);
		DrawPoint_Transparent_GRAPHICS(graphics, y + x0, -x + y0, c);

		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void DrawCircle_Opaque_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		DrawPoint_Opaque_GRAPHICS(graphics, x + x0, y + y0, c);
		DrawPoint_Opaque_GRAPHICS(graphics, -x + x0, y + y0, c);

		DrawPoint_Opaque_GRAPHICS(graphics, y + x0, x + y0, c);
		DrawPoint_Opaque_GRAPHICS(graphics, -y + x0, x + y0, c);

		DrawPoint_Opaque_GRAPHICS(graphics, -x + x0, -y + y0, c);
		DrawPoint_Opaque_GRAPHICS(graphics, x + x0, -y + y0, c);

		DrawPoint_Opaque_GRAPHICS(graphics, -y + x0, -x + y0, c);
		DrawPoint_Opaque_GRAPHICS(graphics, y + x0, -x + y0, c);

		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}
#endif

void FillCircle_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	StrokeWidth(0);
	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Circle(x0, graphics->Height - y0, radius);
#else
	if (graphics->Transparent) {
		FillCircle_Transparent_GRAPHICS(graphics, x0, y0, radius, color);
	} else {
		FillCircle_Opaque_GRAPHICS(graphics, x0, y0, radius, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void FillCircle_Transparent_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		DrawLine_Width_Transparent_GRAPHICS(graphics, -x + x0, y + y0, x * 2, c);
		DrawLine_Width_Transparent_GRAPHICS(graphics, -y + x0, x + y0, y * 2, c);
		DrawLine_Width_Transparent_GRAPHICS(graphics, -x + x0, -y + y0, x * 2, c);
		DrawLine_Width_Transparent_GRAPHICS(graphics, -y + x0, -x + y0, y * 2, c);
		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void FillCircle_Opaque_GRAPHICS(GRAPHICS* graphics, int x0, int y0, int radius, int c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		DrawLine_Width_Opaque_GRAPHICS(graphics, -x + x0, y + y0, x * 2, c);
		DrawLine_Width_Opaque_GRAPHICS(graphics, -y + x0, x + y0, y * 2, c);
		DrawLine_Width_Opaque_GRAPHICS(graphics, -x + x0, -y + y0, x * 2, c);
		DrawLine_Width_Opaque_GRAPHICS(graphics, -y + x0, -x + y0, y * 2, c);
		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}
#endif

void FillPolygon_GRAPHICS(GRAPHICS* graphics, POLYGON &p, int color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	VGfloat* x = new VGfloat[p.corners];
	VGfloat* y = new VGfloat[p.corners];

	for (int i = 0; i < p.corners; i++) {
		x[i] = p.x_coords[i];
		y[i] = graphics->Height -  p.y_coords[i];
	}

	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Polygon(x, y, p.corners);
#else
	if (graphics->Transparent) {
		FillPolygon_Transparent_GRAPHICS(graphics, p, color);
	} else {
		FillPolygon_Opaque_GRAPHICS(graphics, p, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void FillPolygon_Transparent_GRAPHICS(GRAPHICS* graphics, POLYGON &p, int c) {
	int  nodes, pixelY, i, j, swap;
	int nodeX[10];
	for (pixelY = p.top; pixelY < p.bottom; pixelY++) {
		nodes = 0;
		j= p.corners - 1;
		for (i = 0; i < p.corners; i++) {
			if ((p.GetY(i) < (double) pixelY && p.GetY(j) >= (double) pixelY) ||  (p.GetY(j) < (double) pixelY && p.GetY(i) >= (double) pixelY)) {
				nodeX[nodes++] = (int) (p.GetX(i) + (pixelY - p.GetY(i)) / (p.GetY(j) - p.GetY(i)) * (p.GetX(j) - p.GetX(i)));
			}
			j = i;
		}
		i = 0;
		while (i < nodes - 1 && i < 9) {
			if (nodeX[i]>nodeX[i+1]) {
				swap = nodeX[i];
				nodeX[i] = nodeX[i + 1];
				nodeX[i + 1] = swap;
				if (i) i--;
			} else {
				i++;
			}
		}
		for (i = 0; i < nodes; i += 2) {
			if   (nodeX[i  ] >= p.right) break;
			if   (nodeX[i+1] > p.left) {
				if (nodeX[i    ] < p.left ) nodeX[i    ] = p.left;
				if (nodeX[i + 1] > p.right) nodeX[i + 1] = p.right;
				for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
					DrawPoint_Transparent_GRAPHICS(graphics, j, pixelY, c);
				}
			}
		}
	}
}

void FillPolygon_Opaque_GRAPHICS(GRAPHICS* graphics, POLYGON &p, int c) {
	int  nodes, pixelY, i, j, swap;
	int nodeX[10];
	for (pixelY = p.top; pixelY < p.bottom; pixelY++) {
		nodes = 0;
		j= p.corners - 1;
		for (i = 0; i < p.corners; i++) {
			if ((p.GetY(i) < (double) pixelY && p.GetY(j) >= (double) pixelY) ||  (p.GetY(j) < (double) pixelY && p.GetY(i) >= (double) pixelY)) {
				nodeX[nodes++] = (int) (p.GetX(i) + (pixelY - p.GetY(i)) / (p.GetY(j) - p.GetY(i)) * (p.GetX(j) - p.GetX(i)));
			}
			j = i;
		}
		i = 0;
		while (i < nodes - 1 && i < 9) {
			if (nodeX[i] > nodeX[i+1]) {
				swap = nodeX[i];
				nodeX[i] = nodeX[i + 1];
				nodeX[i + 1] = swap;
				if (i) i--;
			} else {
				i++;
			}
		}
		for (i = 0; i < nodes; i += 2) {
			if   (nodeX[i  ] >= p.right) break;
			if   (nodeX[i+1] > p.left) {
				if (nodeX[i    ] < p.left ) nodeX[i    ] = p.left;
				if (nodeX[i + 1] > p.right) nodeX[i + 1] = p.right;
				for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
					DrawPoint_Opaque_GRAPHICS(graphics, j, pixelY, c);
				}
			}
		}
	}
}
#endif

void FillRoundedRect_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int r, Color color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;

	//TODO: check for r == 0

	StrokeWidth(0);
	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Roundrect(x, graphics->Height - h - y, w, h, r, r);
#else
	// Check to see that the rect trying to be drawn is valid
	if (x > graphics->Width_Clip + graphics->X_clip) {
		return;
	}
	if (y > graphics->Height_Clip + graphics->Y_clip) {
		return;
	}
	if (x < graphics->X_clip) {
		w -= graphics->X_clip - x;
		x = graphics->X_clip;
	}
	if (y < graphics->Y_clip) {
		h -= graphics->Y_clip - y;
		y = graphics->Y_clip;
	}
	if (w <= 0) {
		return;
	}
	if (h <= 0) {
		return;
	}

	// If there is no rounding, the square doesn't have enough rounding to be significant
	// Fill up a regular square
	if(r <= 0) {
		FillSquare_GRAPHICS(graphics, x, y, w, h, color);
	}
	// Otherwise, we've got a long function ahead of us...
	else {
	    // Assign half the height or width - whichever is smaller
	    int halfMinDimension = h < w ? h >> 1 : w >> 1;

		// Clamp the rounding constant so that it cannot be bigger than half the smallest dimension
		if(r > halfMinDimension) {
			r = halfMinDimension;
		}

		int innerWidth = w - (r << 1);	// Internal width of the rectangular drawing between the quarter circles
		int innerHeight = h - (r << 1);	// Internal height of the rectangular drawing between the quarter circles

		// Fill the square in the middle of the rect
		FillSquare_GRAPHICS(graphics, x + r, y, innerWidth, h, color);
		// Fill the square on the left of the rect
		FillSquare_GRAPHICS(graphics, x, y + r, r, innerHeight + 1, color);
		// Fill the square on the right of the rect
		FillSquare_GRAPHICS(graphics, x + r + innerWidth, y + r, r, innerHeight + 1, color);

		// Fill arcs at the corners to created rounded edges
		FillQuarterArc_GRAPHICS(graphics, x + innerWidth + r, y, r, r, FIRST_QUADRANT, color);
		FillQuarterArc_GRAPHICS(graphics, x, y, r, r, SECOND_QUADRANT, color);
		FillQuarterArc_GRAPHICS(graphics, x, y + innerHeight + r, r, r, THIRD_QUADRANT, color);
		FillQuarterArc_GRAPHICS(graphics, x + innerWidth + r, y + innerHeight + r, r, r, FOURTH_QUADRANT, color);
	}
#endif
}

void FillQuarterArc_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int quadrant, Color color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;

	log_tmp("quadrant " + to_string(quadrant));

	int sa = 0;
	if (quadrant == FIRST_QUADRANT) {
		sa = 0;
	} else if (quadrant == SECOND_QUADRANT) {
		sa = 90;
	} else if (quadrant == THIRD_QUADRANT) {
		sa = 180;
	} else if (quadrant == FOURTH_QUADRANT) {
		sa = 270;
	}
	
	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Arc(x + w, graphics->Height - y, w * 2, h * 2, sa, 90);

	VGfloat* xp = new VGfloat[3];
	VGfloat* yp = new VGfloat[3];

	//TODO: this triangle is only correct for Quadrant 3
	xp[0] = x;
	xp[1] = x + w;
	xp[2] = x + w;
	yp[0] = graphics->Height - y;
	yp[1] = graphics->Height - y;
	yp[2] = graphics->Height - h - y;

	Fill(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Polygon(xp, yp, 3);

	//DrawSquare_GRAPHICS(graphics, x, y, w, h, 0xFFFFFFFF);
#else
	// Prevent function from continuing if dimensions are invalid
	if(x < 0 || y < 0 || w < 2 || h < 2) {
		cout << "ERROR: the arc is invalid and cannot be draw\n";
		cout << "Arc rect: " << x << ", " << y << ", " << w << ", " << h << endl;
		return;
	}

	// Check to see that the quadrant given is valid
	if(quadrant != FIRST_QUADRANT && quadrant != SECOND_QUADRANT && quadrant != THIRD_QUADRANT && quadrant != FOURTH_QUADRANT) {
		cout << "ERROR: " << quadrant << " does not describe a valid quadrant, the arc cannot be drawn\n";
		return;
	}

	// Super mathy constants are needed for Bresenham's algorithm
	const int a2 = (w * w) >> 2;
	const int b2 = (h * h) >> 2;
	const int fa2 = a2 << 2;
	const int fb2 = b2 << 2;

	int controlX, controlY;	// X-y values used to control the loops
	int selector;	// Calculated and used to decide which pixel intesected by the arc will be filled in
	int xPoints[3];	// Points used to draw the polygons that fill up the elliptical arc
  int yPoints[3];

	// Each is positive or negative one, depending on the quadrant the arc is being filled in
	int quadrantXConst = 1;
	int quadrantYConst = 1;

	// X should be negative in second and third quadrants
	if(quadrant == SECOND_QUADRANT || quadrant == THIRD_QUADRANT) {
		quadrantXConst = -1;
    x += w;
	}
	// Y should is negative in the first and second quadrants
	// (Remember that down is positive on a computer screen)
	if(quadrant == FIRST_QUADRANT || quadrant == SECOND_QUADRANT) {
		quadrantYConst = -1;
    y += h;
	}

	// Initialize the points used to generate the polygons that fill in the arc
	xPoints[0] = x;
	yPoints[0] = y;
	xPoints[2] = x;
	yPoints[2] = y;

	// Initialize the selector (I have literally NO idea why it's set to this equation...)
	selector = (b2 << 1) + (a2 * (1 - (h << 1)));

	// Start a loop with the loop control vars
	for(controlX = 0, controlY = h; b2 * controlX <= a2 * controlY; ++controlX) {
		// Don't really get it yet
		xPoints[1] = x + (controlX * quadrantXConst);
		yPoints[1] = y + (controlY * quadrantYConst);

		// Initialize and draw the polygon (yuk, inner loop is inefficient...)
		class POLYGON poly;
		for(int point = 0; point < 3; ++point) {
			poly.addPoint(xPoints[point], yPoints[point]);
		}
		FillPolygon_GRAPHICS(graphics, poly, color);

		// Ovewrite previous points with these points
		xPoints[2] = xPoints[1];
    yPoints[2] = yPoints[1];

		// If selector is positive, aggregate some mathy number into it
		// (hint: the number is almost always negative) and update control y value
		if(selector >= 0) {
			selector += fa2 * (1 - controlY);
			--controlY;
		}

		// Aggregate something mathy into the selector
		selector += b2 * ((controlX << 2) + 6);
	} // LOL THIS IS JUST THE FIRST HALF

	// Assign the selector something NEW and just as mathy
	selector = (a2 << 1) + (b2 * (1 - (w << 1)));

	// Start a new loop for the next half (bottom half? right half? no idea)
	for(controlX = w, controlY = 0; a2 * controlY <= b2 * controlX; ++controlY) {
		// Still don't really get it
		xPoints[1] = x + (controlX * quadrantXConst);
		yPoints[1] = y + (controlY * quadrantYConst);

		// Initialize and draw the polygon (yuk, inner loop is inefficient...)
		class POLYGON poly;
		for(int point = 0; point < 3; ++point) {
			poly.addPoint(xPoints[point], yPoints[point]);
		}
		FillPolygon_GRAPHICS(graphics, poly, color);

    // Ovewrite previous points with these points
		xPoints[2] = xPoints[1];
		yPoints[2] = yPoints[1];

		// If selector is positive, aggregate some mathy number into it
		// (hint: the number is almost always negative) and update control y value
		if(selector >= 0) {
			selector += fb2 * (1 - controlX);
			--controlX;
		}

		// Aggregate something mathy into the selector
		selector += a2 * ((controlY << 2) + 6);
	}
#endif
}

// Drawing a thick "line" really involves drawing a rectangle where thickness is the height
#ifndef GRAPHICS_USING_HARDWARE
void DrawThickHorizontalLine_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int thickness, Color color) {
	FillSquare_GRAPHICS(graphics, x, y, w, thickness, color);
}

// Drawing a thick line is really a rectangle where thickness is the width
void DrawThickVerticalLine_GRAPHICS(GRAPHICS* graphics, int x, int y, int h, int thickness, Color color) {
  FillSquare_GRAPHICS(graphics, x, y, thickness, h, color);
}
#endif

// Draw a rect with the desired thickness by drawing four lines
void DrawThickRect_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, Color color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;
	
	StrokeWidth(thickness);
	Stroke(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Fill(0, 0, 0, 0);
	RectOutline(x, graphics->Height - y, w, h);
#else
	DrawThickHorizontalLine_GRAPHICS(graphics, x, y, w, thickness, color);
	DrawThickHorizontalLine_GRAPHICS(graphics, x, y + h - thickness, w, thickness, color);
	DrawThickVerticalLine_GRAPHICS(graphics, x, y, h, thickness, color);
	DrawThickVerticalLine_GRAPHICS(graphics, x + w - thickness, y, h, thickness, color);
#endif
}

void DrawThickRoundedRect_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, int r, Color color) {
#ifdef GRAPHICS_USING_HARDWARE
	(void)graphics;
	uint8_t* color_bytes = (uint8_t*)&color;
	float alpha = color_bytes[3] / 255.0;

	int r_offset = r * 4.0 / 3 + 0.5;
	
	StrokeWidth(thickness);
	Stroke(color_bytes[2], color_bytes[1], color_bytes[0], alpha);
	Fill(0, 0, 0, 0);
	RoundrectOutline(x, graphics->Height - y, w, h, w - r_offset, h - r_offset);
#else
	// Check to see that the rect trying to be drawn is valid
	if(x < 0 || y < 0 || w < 0 || h < 0) {
		cout << "ERROR: The rect is invalid and cannot be drawn\n";
		cout << "Rect: " << x << ", " << y << ", " << w << ", " << h << endl;
		return;
	}

	// If there is no rounding, the square doesn't have enough rounding to be significant
	// Draw a regular thick rect
	if(r <= 0) {
		DrawThickRect_GRAPHICS(graphics, x, y, w, h, thickness, color);
	}
	// Otherwise, we've got a long function ahead of us...
	else {
    // Assign half the height or width - whichever is smaller
    int halfMinDimension = h < w ? h >> 1 : w >> 1;

		// Clamp the rounding constant so that it cannot be bigger than half the smallest dimension
		if(r > halfMinDimension) {
			r = halfMinDimension;
		}

		int innerWidth = w - (r << 1);	// Internal width of the rectangular drawing between the quarter circles
		int innerHeight = h - (r << 1);	// Internal height of the rectangular drawing between the quarter circles

		// Draw thick horizontal/vertical lines
		DrawThickHorizontalLine_GRAPHICS(graphics, x + r, y, w - (r << 1), thickness, color);
		DrawThickHorizontalLine_GRAPHICS(graphics, x + r, y + h - thickness, w - (r << 1), thickness, color);
		DrawThickVerticalLine_GRAPHICS(graphics, x, y + r, h - (r << 1), thickness, color);
		DrawThickVerticalLine_GRAPHICS(graphics, x + w - thickness, y + r, h - (r << 1), thickness, color);

    DrawThickQuarterArc_GRAPHICS(graphics, x + innerWidth + r, y, r, r, thickness, FIRST_QUADRANT, color);
    DrawThickQuarterArc_GRAPHICS(graphics, x, y, r, r, thickness, SECOND_QUADRANT, color);
    DrawThickQuarterArc_GRAPHICS(graphics, x, y + innerHeight + r, r, r, thickness, THIRD_QUADRANT, color);
    DrawThickQuarterArc_GRAPHICS(graphics, x + innerWidth + r, y + innerHeight + r, r, r, thickness, FOURTH_QUADRANT, color);
	}
#endif
}

#ifndef GRAPHICS_USING_HARDWARE
void DrawQuarterArc_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int quadrant, Color color) {
  // Prevent function from continuing if dimensions are invalid
  if(x < 0 || y < 0 || w < 0 || h < 0) {
    cout << "ERROR: the arc is invalid and cannot be draw\n";
    cout << "Arc rect: " << x << ", " << y << ", " << w << ", " << h << endl;
    return;
  }

  // Check to see that the quadrant given is valid
  if(quadrant != FIRST_QUADRANT && quadrant != SECOND_QUADRANT && quadrant != THIRD_QUADRANT && quadrant != FOURTH_QUADRANT) {
    cout << "ERROR: " << quadrant << " does not describe a valid quadrant, the arc cannot be drawn\n";
    return;
  }

  // Super mathy constants are needed for Bresenham's algorithm
  const int a2 = (w * w) >> 2;
  const int b2 = (h * h) >> 2;
  const int fa2 = a2 << 2;
  const int fb2 = b2 << 2;

  int controlX, controlY;	// X-y values used to control the loops
  int selector;	// Calculated and used to decide which pixel intesected by the arc will be filled in

  // Each is positive or negative one, depending on the quadrant the arc is being filled in
  int quadrantXConst = 1;
  int quadrantYConst = 1;

  // X should be negative in second and third quadrants
  if(quadrant == SECOND_QUADRANT || quadrant == THIRD_QUADRANT) {
    quadrantXConst = -1;
    x += w;
  }
  // Y should is negative in the first and second quadrants
  // (Remember that down is positive on a computer screen)
  if(quadrant == FIRST_QUADRANT || quadrant == SECOND_QUADRANT) {
    quadrantYConst = -1;
    y += h;
  }

  // Initialize the selector (I have literally NO idea why it's set to this equation...)
  selector = (b2 << 1) + (a2 * (1 - (h << 1)));

  // Start a loop with the loop control vars
  for(controlX = 0, controlY = h; b2 * controlX <= a2 * controlY; ++controlX) {
    // Don't really get it yet
    DrawPoint_GRAPHICS(graphics, x + (controlX * quadrantXConst), y + (controlY * quadrantYConst), color);

    // If selector is positive, aggregate some mathy number into it
    // (hint: the number is almost always negative) and update control y value
    if(selector >= 0) {
      selector += fa2 * (1 - controlY);
      --controlY;
    }

    // Aggregate something mathy into the selector
    selector += b2 * ((controlX << 2) + 6);
  } // LOL THIS IS JUST THE FIRST HALF

  // Assign the selector something NEW and just as mathy
  selector = (a2 << 1) + (b2 * (1 - (w << 1)));

  // Start a new loop for the next half (bottom half? right half? no idea)
  for(controlX = w, controlY = 0; a2 * controlY <= b2 * controlX; ++controlY) {
    // Still don't really get it
    DrawPoint_GRAPHICS(graphics, x + (controlX * quadrantXConst), y + (controlY * quadrantYConst), color);

    // If selector is positive, aggregate some mathy number into it
    // (hint: the number is almost always negative) and update control y value
    if(selector >= 0) {
      selector += fb2 * (1 - controlX);
      --controlX;
    }

    // Aggregate something mathy into the selector
    selector += a2 * ((controlY << 2) + 6);
  }
}

// Draw a quarter of an elliptical arc with the desired thickness
void DrawThickQuarterArc_GRAPHICS(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, int quadrant, Color color) {
  for(int arc = 0; arc < thickness; ++arc) {
    DrawQuarterArc_GRAPHICS(graphics, x, y, w - arc, h - arc, quadrant, color);
  }
}
#endif
