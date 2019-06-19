#include "Embedded_Animator.h"

void Initialize_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR* anim, string baseName, string fileExtension, float widthRatio,
float heightRatio, uint8_t totalFrames, bool looping,
struct nk_color buttonColor, struct nk_color textColor,
uint8_t framesPerString) {
	anim->totalFrames = totalFrames < MAX_ANIM_FRAMES ?
	totalFrames : MAX_ANIM_FRAMES;

	// Initialize each image with the same width/height ratios using the
	// animator standard naming convention
	for (uint8_t i = 0; i < anim->totalFrames; i++) {
		Initialize_Image(
		&anim->frames[i], baseName + to_string(i + 1) + fileExtension,
		widthRatio, heightRatio);
	}

	anim->looping = looping;
	anim->currentFrame = 0;
	anim->frameClamp = -1;

	anim->initializing = false;
	anim->finishedInitializing = false;
	anim->initializerThreadJoined = false;

	Initialize_SIMPLE_ELLIPSIS_ANIMATION(
	&anim->tempAnim, buttonColor, textColor, framesPerString);
}

void Render_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR* anim, struct nk_context* ctx) {
	Update_Initializer_Status_EMBEDDED_ANIMATOR(anim, ctx);

	if (anim->finishedInitializing) {
		Render_Image(&anim->frames[anim->currentFrame], ctx);
		Update_Current_Frame_EMBEDDED_ANIMATOR(anim);
	}
	else {
		Render_SIMPLE_ELLIPSIS_ANIMATION(&anim->tempAnim, ctx);
	}
}

void Render_EMBEDDED_ANIMATOR_With_Buffer(
EMBEDDED_ANIMATOR* anim, struct nk_context* ctx) {
	nk_label(ctx, "", 0);
	Render_EMBEDDED_ANIMATOR(anim, ctx);
}

void Initialize_Images_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR* anim, struct nk_rect r) {
	anim->initializing = true;
	anim->finishedInitializing = false;

	for (uint8_t i = 0; i < anim->totalFrames; i++) {
		Check_And_Load_Image(&anim->frames[i], r);
	}

	anim->initializing = false;
	anim->finishedInitializing = true;
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

void Update_Initializer_Status_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR* anim, struct nk_context* ctx) {
	// If animator is not initialized and hasn't started initializing,
	// start the initializer thread
	if (!anim->finishedInitializing && !anim->initializing) {
		anim->initializerThread = thread(
		Initialize_Images_EMBEDDED_ANIMATOR, anim, nk_widget_bounds(ctx));
	}

	// If animator finished initializing but hasn't been joined, join it
	if (anim->finishedInitializing && !anim->initializerThreadJoined) {
		anim->initializerThread.join();
		anim->initializerThreadJoined = true;
	}
}

bool EMBEDDED_ANIMATOR_Clamped(const EMBEDDED_ANIMATOR* anim) {
	return anim->currentFrame == anim->frameClamp;
}
bool EMBEDDED_ANIMATOR_Finished(const EMBEDDED_ANIMATOR* anim) {
	return anim->currentFrame == (anim->totalFrames - 1);
}
