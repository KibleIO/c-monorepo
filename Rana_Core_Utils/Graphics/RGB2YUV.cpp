#include "RGB2YUV.h"

void rgb_to_y420p(uint8_t* destination, uint8_t* rgb, size_t width, size_t height) {
	size_t image_size = width * height;
	size_t upos = image_size;
	size_t vpos = upos + upos / 4;
	size_t i = 0;

	for (size_t line = 0; line < height; line++) {
		if (!(line & 1)) {
			for (size_t x = 0; x < width; x += 2) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				uint8_t* ru = rgb + 4 * i;
				uint8_t* rg = rgb + 4 * i + 1;
				uint8_t* rb = rgb + 4 * i + 2;
				uint8_t ur = 0;
				uint8_t ug = 0;
				uint8_t ub = 0;

				destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

				if (x == width - 1) {
					if (line == height - 1) {
						// Right Bottom, no averaging
						destination[vpos++] =
						((-38 * r + -74 * g + 112 * b) >> 8) + 128;
						destination[upos++] =
						((112 * r + -94 * g + -18 * b) >> 8) + 128;
					} else {
						// Right edge of screen, only average below
						ur = (*ru / 2 + *(ru + width * 4) / 2);
						ug = (*rg / 2 + *(rg + width * 4) / 2);
						ub = (*rb / 2 + *(rb + width * 4) / 2);

						destination[vpos++] =
						((-38 * ur + -74 * ug + 112 * ub) >> 8) + 128;
						destination[upos++] =
						((112 * ur + -94 * ug + -18 * ub) >> 8) + 128;
					}
				} else if (line == height - 1) {
					// Bottom line, only average right
					ur = (*ru / 2 + *(ru + 4) / 2);
					ug = (*rg / 2 + *(rg + 4) / 2);
					ub = (*rb / 2 + *(rb + 4) / 2);

					destination[vpos++] =
					((-38 * ur + -74 * ug + 112 * ub) >> 8) + 128;
					destination[upos++] =
					((112 * ur + -94 * ug + -18 * ub) >> 8) + 128;
				} else {
					// Anywhere else, average below and right
					ur = (*ru / 4 + *(ru + 4) / 4 + *(ru + width * 4) / 4 +
					*(ru + width * 4 + 4) / 4);
					ug = (*rg / 4 + *(rg + 4) / 4 + *(rg + width * 4) / 4 +
					*(rg + width * 4 + 4) / 4);
					ub = (*rb / 4 + *(rb + 4) / 4 + *(rb + width * 4) / 4 +
					*(rb + width * 4 + 4) / 4);

					destination[vpos++] =
					((-38 * ur + -74 * ug + 112 * ub) >> 8) + 128;
					destination[upos++] =
					((112 * ur + -94 * ug + -18 * ub) >> 8) + 128;
				}

				r = rgb[4 * i];
				g = rgb[4 * i + 1];
				b = rgb[4 * i + 2];

				destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
			}
		} else if (line != 0 && line != height - 1) {
			for (size_t x = 0; x < width; x++) {
				uint8_t r = rgb[4 * i];
				uint8_t g = rgb[4 * i + 1];
				uint8_t b = rgb[4 * i + 2];

				destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
			}
		} 
	}
}
