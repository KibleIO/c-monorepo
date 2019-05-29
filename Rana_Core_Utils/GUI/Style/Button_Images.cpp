#include "Button_Images.h"

BUTTON_IMAGES* Button_Images(string normal_dir, string hover_dir,
string active_dir, uint16_t width, uint16_t height) {
	BUTTON_IMAGES* images = new BUTTON_IMAGES;
	Initialize_BMP(&images->normal, normal_dir, width, height);
	images->normal.Transparent = true;
	Initialize_BMP(&images->hover, hover_dir, width, height);
	images->hover.Transparent = true;
	Initialize_BMP(&images->active, active_dir, width, height);
	images->active.Transparent = true;
	return images;
}
