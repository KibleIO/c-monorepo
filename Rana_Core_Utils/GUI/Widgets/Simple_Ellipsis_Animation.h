#ifndef SIMPLE_ELLIPSIS_ANIMATION_H_
#define SIMPLE_ELLIPSIS_ANIMATION_H_

#include "../NK_BASE.h"
#include "Button.h"

#define TOTAL_STRINGS 4

struct SIMPLE_ELLIPSIS_ANIMATION {
	const char* loadingStrings[TOTAL_STRINGS] = {
		"", ".", "..", "..."
	};

	BUTTON button;	// Button used to display the strings
	uint8_t currentString;	// Current string to display in the button
	// Number of frames the current string has been rendered
	uint8_t currentFrame;
	// Number of frames that each string stays on the screen
	uint8_t framesPerString;
};

SIMPLE_ELLIPSIS_ANIMATION Simple_Ellipsis_Animation(
struct nk_color buttonColor, struct nk_color textColor,
uint8_t framesPerString);
void Initialize_SIMPLE_ELLIPSIS_ANIMATION(
SIMPLE_ELLIPSIS_ANIMATION*, struct nk_color buttonColor,
struct nk_color textColor, uint8_t framesPerString);
void Render_SIMPLE_ELLIPSIS_ANIMATION(
SIMPLE_ELLIPSIS_ANIMATION*, struct nk_context*);
void Render_SIMPLE_ELLIPSIS_ANIMATION_With_Buffer(
SIMPLE_ELLIPSIS_ANIMATION*, struct nk_context*);
void Delete_SIMPLE_ELLIPSIS_ANIMATION(SIMPLE_ELLIPSIS_ANIMATION*);

#endif
