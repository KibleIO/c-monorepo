#ifndef BUTTON_IMAGES_H_
#define BUTTON_IMAGES_H_

#include "../BMP.h"

struct BUTTON_IMAGES {
	BMP normal;
	BMP hover;
	BMP active;
};

BUTTON_IMAGES* Button_Images(string normal_dir, string hover_dir,
	string active_dir, uint16_t width, uint16_t height);
void Delete_Button_Images(BUTTON_IMAGES*);

#endif // BUTTON_IMAGES_H_
