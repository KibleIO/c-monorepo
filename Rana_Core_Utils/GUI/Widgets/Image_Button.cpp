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

void Initialize_Image_Push_Button(
IMAGE_BUTTON* button, string normalDir, string hoverDir, string activeDir,
string dormantDir) {
	Initialize_Image_Button(
	button, BUTTON_TYPE_PUSH, normalDir, hoverDir, activeDir,  "", "", "",
	dormantDir);
}

void Initialize_Image_Toggle_Button(
IMAGE_BUTTON* button, string normalDir, string hoverDir, string activeDir,
string toggledNormalDir, string toggledHoverDir, string toggledActiveDir,
string dormantDir) {
	Initialize_Image_Button(
	button, BUTTON_TYPE_TOGGLE, normalDir, hoverDir, activeDir,
	toggledNormalDir, toggledHoverDir, toggledActiveDir, dormantDir);
}
void Initialize_Image_Push_Button(
IMAGE_BUTTON* button, string normalDir, string hoverDir, string activeDir) {
	Initialize_Image_Push_Button(
	button, normalDir, hoverDir, activeDir, "");
}


void Initialize_Image_Button(
IMAGE_BUTTON* button, uint8_t type, string normalDir, string hoverDir,
string activeDir, string toggledNormalDir, string toggledHoverDir,
string toggledActiveDir, string dormantDir) {
	Nullify_BMPs(button);

	// Initialize normal bmps
	Initialize_Image(&button->normal, Load_Image_GUI(normalDir, 0, 0));
	Initialize_Image(&button->hover, Load_Image_GUI(hoverDir, 0, 0));
	Initialize_Image(&button->active, Load_Image_GUI(activeDir, 0, 0));

	// Initialize toggled bmps
	Initialize_Image(
	&button->toggledNormal, Load_Image_GUI(toggledNormalDir, 0, 0));
	Initialize_Image(
	&button->toggledHover, Load_Image_GUI(toggledHoverDir, 0, 0));
	Initialize_Image(
	&button->toggledActive, Load_Image_GUI(toggledActiveDir, 0, 0));

	// Initialize dormant bmp
	Initialize_Image(&button->dormant, Load_Image_GUI(dormantDir, 0, 0));

	// Initialize the button data
	Initialize_Button(
	&button->super, type, Button_Style(
	// Button style for normal
	Nk_Button_Style(button->normal.texture->data, button->hover.texture->data,
	button->active.texture->data),
	// Button style while toggled
	Nk_Button_Style(button->toggledNormal.texture->data,
	button->toggledHover.texture->data, button->toggledActive.texture->data),
	// Button style while dormant
	Nk_Button_Style_Dormant(button->dormant.texture->data)));
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
	nk_label(ctx, "", 0);
	return Render_Image_Button_Label(button, ctx, label, interactable);
}

/*
DELETE
*/

void Delete_Image_Button(IMAGE_BUTTON* button) {
	Delete_Image(&button->normal);
	Delete_Image(&button->hover);
	Delete_Image(&button->active);

	Delete_Image(&button->toggledNormal);
	Delete_Image(&button->toggledHover);
	Delete_Image(&button->toggledActive);

	Delete_Image(&button->dormant);

	Delete_Button(&button->super);
}
