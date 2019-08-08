#ifndef IMAGE_BUTTON_H_
#define IMAGE_BUTTON_H_

// BROKEN
// The way images work no longer coincides with the way the image button
// is structured. The Image no longer has direct access to the raw image
// class it composes, so the image button has no way of... well, actually the
// raw picture handles the width and height on its own, so the
// image button doesn't need to worry about it at all.
// So maybe it does still work?  Dunno, yet. Gotta test it

#include "Button.h"
#include "Image.h"
#include "../NK_BASE.h"
#include "../../Utilities/Stuff.h"
#include "../Style.h"

struct IMAGE_BUTTON {
	BUTTON super;	// Data for the button, like inheritance

	// BMPs for the normal button
	IMAGE normal;
	IMAGE hover;
	IMAGE active;

	// BMPs for the button while toggled
	IMAGE toggledNormal;
	IMAGE toggledHover;
	IMAGE toggledActive;

	// BMP for the button while dormant
	IMAGE dormant;
};

// Initialize the image button as a toggle button,
void Initialize_Image_Toggle_Button(
IMAGE_BUTTON*, string normalDir, string hoverDir, string activeDir,
string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
string dormantDir);

// Initialize the image button as a push button
void Initialize_Image_Push_Button(
IMAGE_BUTTON*, string normalDir, string hoverDir, string activeDir,
string dormantDir);
void Initialize_Image_Push_Button(
IMAGE_BUTTON*, string normalDir, string hoverDir, string activeDir);

// Initialize the image button
void Initialize_Image_Button(
IMAGE_BUTTON*, uint8_t type, string normalDir, string hoverDir,
string activeDir, string toggledNormalDir, string toggledHoverDir,
string toggledActiveDir, string dormantDir);

// Render the image button
bool Render_Image_Button_Label(
IMAGE_BUTTON*, struct nk_context*, const char*, bool interactable = true);
bool Render_Image_Button_Label_With_Buffer(
IMAGE_BUTTON*, struct nk_context*, const char*, bool interactable = true);

// Release resources owned by image button
void Delete_Image_Button(IMAGE_BUTTON*);

#endif
