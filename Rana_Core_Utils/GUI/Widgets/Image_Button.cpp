#include "Image_Button.h"

/*
INITIALIZING
*/

void Nullify_BMPs(IMAGE_BUTTON* button) {
	button->normal = NULLIFY;
	button->hover = NULLIFY;
	button->active = NULLIFY;
	button->toggledNormal = NULLIFY;
	button->toggledHover = NULLIFY;
	button->toggledActive = NULLIFY;
	button->dormant = NULLIFY;
}

void Initialize_Image_Push_Button(IMAGE_BUTTON* button,
string normalDir, string hoverDir, string activeDir,
string dormantDir, uint32_t width, uint32_t height) {
	Nullify_BMPs(button);
	Initialize_Image_Button(button, BUTTON_TYPE_PUSH,
		normalDir, hoverDir, activeDir, "", "", "", dormantDir, width, height);
}

void Initialize_Image_Toggle_Button(IMAGE_BUTTON* button,
string normalDir, string hoverDir, string activeDir,
string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
string dormantDir, uint32_t width, uint32_t height) {
	Nullify_BMPs(button);
	Initialize_Image_Button(button, BUTTON_TYPE_TOGGLE,
		normalDir, hoverDir, activeDir,
		toggledNormalDir, toggledHoverDir, toggledActiveDir,
		dormantDir, width, height);
}
void Initialize_Image_Push_Button(IMAGE_BUTTON* button,
string normalDir, string hoverDir, string activeDir,
uint32_t width, uint32_t height) {
	Nullify_BMPs(button);
	Initialize_Image_Push_Button(button,
		normalDir, hoverDir, activeDir, "", width, height);
}


void Initialize_Image_Button(IMAGE_BUTTON* button, uint8_t type,
string normalDir, string hoverDir, string activeDir,
string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
string dormantDir, uint32_t width, uint32_t height) {
	Nullify_BMPs(button);

	// Initialize normal bmps
	Initialize_Image_Button_BMP(&button->normal, normalDir, width, height);
	Initialize_Image_Button_BMP(&button->hover, hoverDir, width, height);
	Initialize_Image_Button_BMP(&button->active, activeDir, width, height);

	// Initialize toggled bmps
	Initialize_Image_Button_BMP(&button->toggledNormal, toggledNormalDir,
		width, height);
	Initialize_Image_Button_BMP(&button->toggledHover, toggledHoverDir,
		width, height);
	Initialize_Image_Button_BMP(&button->toggledActive, toggledActiveDir,
		width, height);

	// Initialize dorman bmp
	Initialize_Image_Button_BMP(&button->dormant, dormantDir, width, height);

	// Initialize the button data
	Initialize_Button(&button->super, type,
		Button_Style(
			// Button style for normal
			Nk_Button_Style(
				&button->normal,
				&button->hover,
				&button->active
			),
			// Button style while toggled
			Nk_Button_Style(
				&button->toggledNormal,
				&button->toggledHover,
				&button->toggledActive
			),
			// Button style while dormant
			Nk_Button_Style_Dormant(&button->dormant)
		)
	);
}

/*
RENDERING
*/

bool Render_Image_Button_Label(IMAGE_BUTTON* button, struct nk_context* ctx,
const char* label, bool interactable) {
	return Render_Button_Label(&button->super, ctx, label, interactable);
}
bool Render_Image_Button_Label_With_Buffer(IMAGE_BUTTON* button,
struct nk_context* ctx, const char* label, bool interactable) {
	return Render_Button_Label_With_Buffer(&button->super, ctx,
		label, interactable);
}

/*
DELETE
*/

void Delete_Image_Button(IMAGE_BUTTON* button) {
	Delete_BMP(&button->normal);
	Delete_BMP(&button->hover);
	Delete_BMP(&button->active);

	Delete_BMP(&button->toggledNormal);
	Delete_BMP(&button->toggledHover);
	Delete_BMP(&button->toggledActive);

	Delete_BMP(&button->dormant);

	Delete_Button(&button->super);
}

/*
INITIALIZATION HELPERS
*/
void Initialize_Image_Button_BMP(BMP* image, string dir,
uint32_t width, uint32_t height) {
	if(!dir.empty()) {
		Initialize_BMP(image, dir, width, height);
		image->Transparent = true;
	}
	else {
		image = NULL;
	}
}
