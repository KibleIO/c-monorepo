#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "../GUI.h"
#include "../BMP.h"
#include <thread>

#define NO_FRAME_CLAMP -1

// Structure holds a pointer of bitmap pointers that can be drawn one
// after the other in sequence to produce the desired animation
// at the desired location on the given graphics handle
struct ANIMATOR {
	// Pointer to an array of bitmap pointers.
	// Each pointer is one frame in the animation
	BMP** frames;

	// The bitmaps in the array of frames are drawn on this graphics structure
	// BORROWED - don't delete!
	GRAPHICS* graphicsHandle;

	// If true, the animation loops.
	// Otherwise, Advance_Animation cannot advance past the max frames
	bool looping;
	// Total number of frames in the animation
	int totalFrames;
	// Current frame of the animation being displayed, range is [0, totalFrames)
	int currentFrame;
	// Animation cannot advance past this frame. Equals -1 if no clamp is set
	int frameClamp;

	int x, y; // x-y coordinates of the top-left corner of the animation frames

	// Thread used to initialize the animator
	thread* initializerThread;
	// True if the animator is still trying to initialize itself
	bool initializerThreadRunning;
};

// Initialize_Animator will initialize itself with image files
// named with the convention #baseName#frameNum#fileType. Note particularly that
// the function expects the file name to start at 1, NOT 0
// Example: Let's say I have 47 bitmaps for a moon-type animation
// in the folder /home/user/res.  The first bitmap should be at the directory
// /home/user/res/moon1.bmp, and the last should be at
// /home/user/res/moon47.bmp. #baseName should be set to
// "/home/user/res/moon" and #fileType should be set to ".bmp"
void Initialize_Animator(
ANIMATOR*, GRAPHICS*, bool looping, string baseName, string fileType,
int totalFrames, int x, int y, int w, int h);
void Initialize_Animator(
ANIMATOR*, GRAPHICS*, bool looping, string baseName, string fileType,
int totalFrames, struct nk_rect);

void Initialize_Animator(
ANIMATOR*, GRAPHICS*, bool looping, string baseName, string fileType,
int totalFrames, int x, int y);
void Initialize_Animator(
ANIMATOR*, GRAPHICS*, bool looping, string baseName, string fileType,
int totalFrames, struct nk_vec2);

void Delete_Animator(ANIMATOR*);

// Advance the animation by drawing the current image and advancing the current
// frame variable. Designed to be called once per frame.
// NOTE: calling this function is not guaranteed to advance the animation
// If the animation is finished but not allowed to loop, or if the animation
// has reached a clamp, calling the function will only draw the last frame
// (if it can't loop) or the clamp frame (if the animation is clamped)
void Advance_Animation(ANIMATOR*);
// Skip the animation straight to the given frame
// If the frame is past the current frame clamp, this function skips the
// animation straight to the clamp
void Skip_Animation(ANIMATOR*, int frame);
// Skip the animation straight to the frame that the animation is clamped to
void Skip_Animation_To_Clamp(ANIMATOR*);

// Call this function if you want a non-looping animation
// to start from the beginning
void Restart_Animation(ANIMATOR*);
// Start the animation from the beginning and remove all clamps
void Reset_Animation(ANIMATOR*);

// Clamps the animation so that the animation cannot advance more than
// #numFrames past the current frame
bool Clamp_Animation_Forward(ANIMATOR*, int numFrames);
bool Clamp_Animation(ANIMATOR*, int frame);
// Remove clamps on the animation
void Unclamp_Animation(ANIMATOR*);

/*
ACCESSORS
*/
// Returns true if the animation has advanced up to the clamp set by the client code
// Always false if no clamp is set
bool Animation_Clamped(const ANIMATOR*);
// True if the animation has reached the last frame
// This function is not very meaningful for animations that are allowed to loop
bool Animation_Finished(const ANIMATOR*);
// True if the animation has finished initializing
bool Animation_Ready(const ANIMATOR*);

// INITIALIZATION HELPERS
void Initialize_Animator_Fields(ANIMATOR*, GRAPHICS*, bool looping, int totalFrames, int x, int y);
void Initialize_Animator_Frames_Custom_Sizes(ANIMATOR*, string baseName, string fileType, int w, int h);
void Initialize_Animator_Frames(ANIMATOR*, string baseName, string fileType);

#endif // ANIMATOR_H_
