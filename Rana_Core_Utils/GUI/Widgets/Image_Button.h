#ifndef IMAGE_BUTTON_H_
#define IMAGE_BUTTON_H_

#include "../GUI.h"
#include "Utilities/Stuff.h"
#include "Button.h"
#include "../Style.h"

struct IMAGE_BUTTON {
	BUTTON super;	// Data for the button, like inheritance

	// BMPs for the normal button
	BMP normal;
	BMP hover;
	BMP active;

	// BMPs for the button while toggled
	BMP toggledNormal;
	BMP toggledHover;
	BMP toggledActive;

	// BMP for the button while dormant
	BMP dormant;
};

// Initialize the image button as a toggle button,
void Initialize_Image_Toggle_Button(IMAGE_BUTTON*,
	string normalDir, string hoverDir, string activeDir,
	string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
	string dormantDir, uint32_t width, uint32_t height);

// Initialize the image button as a push button
void Initialize_Image_Push_Button(IMAGE_BUTTON*,
	string normalDir, string hoverDir, string activeDir,
	string dormantDir, uint32_t width, uint32_t height);
void Initialize_Image_Push_Button(IMAGE_BUTTON*,
	string normalDir, string hoverDir, string activeDir,
	uint32_t width, uint32_t height);

// Initialize the image button
void Initialize_Image_Button(IMAGE_BUTTON*, uint8_t type,
	string normalDir, string hoverDir, string activeDir,
	string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
	string dormantDir, uint32_t width, uint32_t height);

// Render the image button
bool Render_Image_Button_Label(IMAGE_BUTTON*, struct nk_context*, const char*,
	bool interactable = true);
bool Render_Image_Button_Label_With_Buffer(IMAGE_BUTTON*, struct nk_context*,
	const char*, bool interactable = true);

// Release resources owned by image button
void Delete_Image_Button(IMAGE_BUTTON*);

/*
INITIALIZATION HELPERS
*/
void Initialize_Image_Button_BMP(BMP*, string dir,
	uint32_t width, uint32_t height);

#endif
