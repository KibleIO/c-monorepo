#include "Embedded_Animator.h"

EMBEDDED_ANIMATOR Embedded_Animator(
string baseName, string fileExtension, uint8_t totalFrames, bool looping) {
	EMBEDDED_ANIMATOR anim;
	Initialize_EMBEDDED_ANIMATOR(
	&anim, baseName, fileExtension, totalFrames, looping);
	return anim;
}

void Initialize_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR* anim, string baseName, string fileExtension,
uint8_t totalFrames, bool looping) {
	anim->totalFrames = totalFrames < MAX_ANIM_FRAMES ?
	totalFrames : MAX_ANIM_FRAMES;

	// Initialize each image with the same width/height ratios using the
	// animator standard naming convention
	for (uint8_t i = 0; i < anim->totalFrames; i++) {
		Initialize_Image(
		&anim->frames[i], baseName + to_string(i + 1) + fileExtension);
	}

	anim->looping = looping;
	anim->currentFrame = 0;
	anim->frameClamp = -1;
}

void Render_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR* anim, struct nk_context* ctx, RECT_TRANSFORM transform) {
	Render_Image(&anim->frames[anim->currentFrame], ctx, transform);
	Update_Current_Frame_EMBEDDED_ANIMATOR(anim);
}

void Render_EMBEDDED_ANIMATOR_With_Buffer(
EMBEDDED_ANIMATOR* anim, struct nk_context* ctx, RECT_TRANSFORM transform) {
	nk_label(ctx, "", 0);
	Render_EMBEDDED_ANIMATOR(anim, ctx, transform);
}

void Render_EMBEDDED_ANIMATOR_Standalone(
EMBEDDED_ANIMATOR* anim, struct nk_context* ctx, RECT_TRANSFORM transform,
struct nk_rect r) {
	PANEL panel = Panel(Nk_Window_Style());

	if (Start_Window(&panel, ctx, "animation", r, NK_WINDOW_NO_SCROLLBAR)) {
		Layout_Row_Single_Full(ctx, Breadth(Ratio_Of_Total(1)));
		Render_EMBEDDED_ANIMATOR_With_Buffer(anim, ctx, transform);
	}

	nk_end(ctx);
	Delete_Panel(&panel);
}

void Delete_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim) {
	for (uint8_t i = 0; i < anim->totalFrames; i++) {
		Delete_Image(&anim->frames[i]);
	}
}

void Update_Current_Frame_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim) {
	// Advance to the next frame if we are not at the frame clamp
	if(!EMBEDDED_ANIMATOR_Clamped(anim)) {
		(anim->currentFrame)++;
		// If the animation is allowed to loop...
		if(anim->looping) {
			//...set current frame back to zero when we've gone
			// through the whole animation
			anim->currentFrame %= anim->totalFrames;
		}
		// Otherwise, if the animation does not loop and we have gone
		// through the whole animation...
		else if(anim->currentFrame >= anim->totalFrames) {
			//...set the current frame to the last frame
			anim->currentFrame = anim->totalFrames - 1;
		}// end if looping
	}// end if clamped
}

void Clamp_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim, uint8_t clamp) {
	if (clamp < anim->totalFrames) {
		anim->frameClamp = clamp;
	}
}
void Unclamp_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim) {
	anim->frameClamp = -1;
}

void Restart_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim) {
	anim->currentFrame = 0;
}

bool EMBEDDED_ANIMATOR_Clamped(const EMBEDDED_ANIMATOR* anim) {
	return anim->currentFrame == anim->frameClamp;
}
bool EMBEDDED_ANIMATOR_Finished(const EMBEDDED_ANIMATOR* anim) {
	return anim->currentFrame == (anim->totalFrames - 1);
}
uint8_t Total_Frames_EMBEDDED_ANIMATOR(const EMBEDDED_ANIMATOR* anim) {
	return anim->totalFrames;
}
