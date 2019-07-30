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
	Initialize_Image(&button->normal, normalDir);
	Initialize_Image(&button->hover, hoverDir);
	Initialize_Image(&button->active, activeDir);

	// Initialize toggled bmps
	Initialize_Image(&button->toggledNormal, toggledNormalDir);
	Initialize_Image(&button->toggledHover, toggledHoverDir);
	Initialize_Image(&button->toggledActive, toggledActiveDir);

	// Initialize dormant bmp
	Initialize_Image(&button->dormant, dormantDir);

	// Initialize the button data
	Initialize_Button(
	&button->super, type, Button_Style(
	// Button style for normal
	Nk_Button_Style(&button->normal.image, &button->hover.image,
	&button->active.image),
	// Button style while toggled
	Nk_Button_Style(&button->toggledNormal.image, &button->toggledHover.image,
	&button->toggledActive.image),
	// Button style while dormant
	Nk_Button_Style_Dormant(&button->dormant.image)));
}

/*
RENDERING
*/

bool Render_Image_Button_Label(IMAGE_BUTTON* button, struct nk_context* ctx,
const char* label, bool interactable) {
	Check_And_Load_Images(button, ctx, interactable);
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

void Check_And_Load_Images(
IMAGE_BUTTON* button, struct nk_context* ctx, bool interactable) {
	if (interactable) {
		if (!button->super.toggleState) {
			Check_And_Load_Image_In_Current_Rect(
			&button->normal, ctx, NO_CHANGE_RECT_TRANSFORM);
			Check_And_Load_Image_In_Current_Rect(
			&button->hover, ctx, NO_CHANGE_RECT_TRANSFORM);
			Check_And_Load_Image_In_Current_Rect(
			&button->active, ctx, NO_CHANGE_RECT_TRANSFORM);
		}
		else {
			Check_And_Load_Image_In_Current_Rect(
			&button->toggledNormal, ctx, NO_CHANGE_RECT_TRANSFORM);
			Check_And_Load_Image_In_Current_Rect(
			&button->toggledHover, ctx, NO_CHANGE_RECT_TRANSFORM);
			Check_And_Load_Image_In_Current_Rect(
			&button->toggledActive, ctx, NO_CHANGE_RECT_TRANSFORM);
		}
	}
	else {
		Check_And_Load_Image_In_Current_Rect(
		&button->dormant, ctx, NO_CHANGE_RECT_TRANSFORM);
	}
}
