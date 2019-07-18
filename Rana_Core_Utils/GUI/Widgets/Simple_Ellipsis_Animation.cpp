#include "Simple_Ellipsis_Animation.h"

SIMPLE_ELLIPSIS_ANIMATION Simple_Ellipsis_Animation(
struct nk_color buttonColor, struct nk_color textColor,
uint8_t framesPerString) {
	SIMPLE_ELLIPSIS_ANIMATION anim;
	Initialize_SIMPLE_ELLIPSIS_ANIMATION(
	&anim, buttonColor, textColor, framesPerString);
	return anim;
}

void Initialize_SIMPLE_ELLIPSIS_ANIMATION(
SIMPLE_ELLIPSIS_ANIMATION* anim, struct nk_color buttonColor,
struct nk_color textColor, uint8_t framesPerString) {
	Initialize_Push_Button(
	&anim->button, Nk_Button_Style_Dormant(buttonColor, textColor));
	anim->currentString = 0;
	anim->currentFrame = 0;
	anim->framesPerString = framesPerString;
}

void Render_SIMPLE_ELLIPSIS_ANIMATION(
SIMPLE_ELLIPSIS_ANIMATION* anim, struct nk_context* ctx) {
	// Update number of frames current string has rendered
	anim->currentFrame = (anim->currentFrame + 1) % anim->framesPerString;

	// If frame looped back down to one, update current string
	if (anim->currentFrame == 0) {
		anim->currentString = (anim->currentString + 1) % TOTAL_STRINGS;
	}

	// Render the button with the current string
	Render_Button_Label(
	&anim->button, ctx, anim->loadingStrings[anim->currentString]);
}

void Render_SIMPLE_ELLIPSIS_ANIMATION_With_Buffer(
SIMPLE_ELLIPSIS_ANIMATION* anim, struct nk_context* ctx) {
	nk_label(ctx, "", 0);
	Render_SIMPLE_ELLIPSIS_ANIMATION(anim, ctx);
}

void Delete_SIMPLE_ELLIPSIS_ANIMATION(SIMPLE_ELLIPSIS_ANIMATION* anim) {
	Delete_Button(&anim->button);
}
