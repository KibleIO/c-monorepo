#include "Animator.h"

using std::cout;
using std::endl;

// Initialize animator with custom resizing of the images
void Initialize_Animator(ANIMATOR* anim, GRAPHICS *graphicsHandle, bool looping,
string baseName, string fileType, int totalFrames,
int x, int y, int w, int h) {
	Initialize_Animator_Fields(
	anim, graphicsHandle, looping, totalFrames, x, y);

	// Spawn a thread to initialize the animator images
  	anim->initializerThread = new thread(
	Initialize_Animator_Frames_Custom_Sizes,
	anim, baseName, fileType, w, h);
}

void Initialize_Animator(ANIMATOR* anim, GRAPHICS *graphicsHandle, bool looping,
string baseName, string fileType, int totalFrames, struct nk_rect rect) {
	Initialize_Animator(
	anim, graphicsHandle, looping, baseName, fileType, totalFrames, rect.x,
	rect.y, rect.w, rect.h);
}

// Initialize animator without resizing the images
void Initialize_Animator(ANIMATOR* anim, GRAPHICS* graphicsHandle,
bool looping, string baseName, string fileType, int totalFrames, int x, int y) {
	Initialize_Animator_Fields(
	anim, graphicsHandle, looping, totalFrames, x, y);

	// Spawn a thread to initialize the animator images
	anim->initializerThread = new thread(
	Initialize_Animator_Frames, anim, baseName, fileType);
}

void Initialize_Animator(ANIMATOR* anim, GRAPHICS *graphicsHandle, bool looping,
string baseName, string fileType, int totalFrames, struct nk_vec2 size) {
	Initialize_Animator(
	anim, graphicsHandle, looping, baseName, fileType, totalFrames, size.x,
	size.y);
}

// De-allocate memory for all pointers
void Delete_Animator(ANIMATOR* anim) {
	if(anim->initializerThread != NULL) {
		anim->initializerThread->join();
	  	delete anim->initializerThread;
	}

	for(int i = 0; i < anim->totalFrames; ++i) {
		Delete_BMP(anim->frames[i]);
		delete anim->frames[i];
	}
	delete [] anim->frames;
}

// Draws the current frame and advances one frame
void Advance_Animation(ANIMATOR* anim) {
	// Pre-check to make sure the animator has frames to draw,
	// and that the frames have finished initializing
	if(anim->totalFrames > 0 && !anim->initializerThreadRunning) {
		// Draw the current frame
		Draw_BMP(anim->frames[anim->currentFrame], anim->graphicsHandle,
			anim->x, anim->y);

		// Advance to the next frame if we are not at the last frame
		// and we are not clamped
		if(!Animation_Clamped(anim)) {
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
	}// end if totalFrames > 0
}

void Skip_Animation(ANIMATOR* anim, int frame) {
	// Check to make sure frame is within bounds
	if(frame >= 0 && frame < anim->totalFrames) {
		// If frame exceeds the clamp, set current frame to clamp
		if(anim->frameClamp != NO_FRAME_CLAMP && frame > anim->frameClamp) {
			anim->currentFrame = anim->frameClamp;
		}
		// If no clamp is set or frame is within its range, set it directly
		else {
			anim->currentFrame = frame;
		}
		// Draw the frame immediately
		Advance_Animation(anim);
	}
}

void Skip_Animation_To_Clamp(ANIMATOR* anim) {
	if(anim->frameClamp != NO_FRAME_CLAMP) {
		Skip_Animation(anim, anim->frameClamp);
	}
}

// Allows a looping animation to go again
void Restart_Animation(ANIMATOR* anim) {
  anim->currentFrame = 0;
}
void Reset_Animation(ANIMATOR* anim) {
  Restart_Animation(anim);
  Unclamp_Animation(anim);
}

// Clamp the animation so that it can only advance #numFrames further
bool Clamp_Animation_Forward(ANIMATOR* anim, int numFrames) {
	return Clamp_Animation(anim, anim->currentFrame + numFrames);
}
bool Clamp_Animation(ANIMATOR* anim, int frame) {
	if(frame >= 0 && frame < anim->totalFrames) {
		anim->frameClamp = frame;
		return true;
	}
	else {
		return false;
	}
}
// Effectively unclamp animation by setting it to an invalid value
void Unclamp_Animation(ANIMATOR* anim) {
 	anim->frameClamp = NO_FRAME_CLAMP;
}

/*
ACCESSORS
*/
// True if the current frame is equal to the clamp frame
bool Animation_Clamped(const ANIMATOR* anim) {
  if(anim->totalFrames > 0) {
    return anim->currentFrame == anim->frameClamp;
  }
  else {
    return true;
  }
}
// True if the animation is on the last frame
bool Animation_Finished(const ANIMATOR* anim) {
  if(anim->totalFrames > 0) {
    return anim->currentFrame == anim->totalFrames - 1;
  }
  else {
    return true;
  }
}
// True if the animation has finished initializing
bool Animation_Ready(const ANIMATOR* anim) {
	return !anim->initializerThreadRunning;
}

// Init animator fields
void Initialize_Animator_Fields(ANIMATOR* anim, GRAPHICS* graphicsHandle, bool looping,
  int totalFrames, int x, int y) {
  // Assign the variables of the animator passed in
  anim->frames = new BMP*[totalFrames];
  anim->graphicsHandle = graphicsHandle;
  anim->looping = looping;
  anim->totalFrames = totalFrames;
  anim->currentFrame = 0;
  anim->frameClamp = NO_FRAME_CLAMP;
  anim->x = x;
  anim->y = y;
}

// Intialize animator frames with custom resizing
void Initialize_Animator_Frames_Custom_Sizes(ANIMATOR* anim, string baseName,
string fileType, int w, int h) {
	string frameDir;
	anim->initializerThreadRunning = true;
	// Go through each of the bitmaps at the directory specified
	for(int i = 0; i < anim->totalFrames; ++i) {
		frameDir = baseName + to_string(i + 1) + fileType;

		anim->frames[i] = new BMP();
		Initialize_BMP(anim->frames[i], frameDir, w, h);
		anim->frames[i]->Transparent = true;
	}
	anim->initializerThreadRunning = false;
}

// Initialize animator frames without resizing
void Initialize_Animator_Frames(ANIMATOR* anim, string baseName,
string fileType) {
	string frameDir;
	anim->initializerThreadRunning = true;
	// Go through each of the bitmaps at the directory specified
	for(int i = 0; i < anim->totalFrames; ++i) {
		frameDir = baseName + to_string(i + 1) + fileType;

		anim->frames[i] = new BMP();
		Initialize_BMP(anim->frames[i], frameDir);
		anim->frames[i]->Transparent = true;
	}
	anim->initializerThreadRunning = false;
}
