#ifndef EMBEDDED_ANIMATOR_H_
#define EMBEDDED_ANIMATOR_H_

#include "../NK_BASE.h"
#include "Panel.h"
#include "Image.h"
#include "Simple_Ellipsis_Animation.h"
#include "../Style.h"

#define MAX_ANIM_FRAMES 128

struct EMBEDDED_ANIMATOR {
	IMAGE frames[MAX_ANIM_FRAMES];	// Images in the animation
	uint8_t totalFrames;	// Total animation images

	bool looping;	// True if the animation is allowed to loop
	int16_t currentFrame;	// Current frame being rendered
	int16_t frameClamp;	// Animation cannot advance past this frame
};

EMBEDDED_ANIMATOR Embedded_Animator(
string baseName, string fileExtension, uint8_t totalFrames, bool looping);

void Initialize_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR*, string baseName, string fileExtension, uint8_t totalFrames,
bool looping);

// Render the EMBEDDED_ANIMATOR in the current layout space
void Render_EMBEDDED_ANIMATOR(
EMBEDDED_ANIMATOR*, struct nk_context*, RECT_TRANSFORM);
void Render_EMBEDDED_ANIMATOR_With_Buffer(
EMBEDDED_ANIMATOR*, struct nk_context*, RECT_TRANSFORM);

// Render the embedded animator in a standalone window
// WARNING: do NOT call between nk_begin_xxx and nk_end
void Render_EMBEDDED_ANIMATOR_Standalone(
EMBEDDED_ANIMATOR*, struct nk_context*, RECT_TRANSFORM, struct nk_rect);

void Delete_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR*);

/*
HELPERS
*/

void Update_Current_Frame_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR*);

/*
MUTATORS
*/

void Clamp_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR*, uint8_t clamp);
void Unclamp_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR*);
void Restart_EMBEDDED_ANIMATOR(EMBEDDED_ANIMATOR*);

/*
ACCESSORS
*/

bool EMBEDDED_ANIMATOR_Clamped(const EMBEDDED_ANIMATOR*);
bool EMBEDDED_ANIMATOR_Finished(const EMBEDDED_ANIMATOR*);
uint8_t Total_Frames_EMBEDDED_ANIMATOR(const EMBEDDED_ANIMATOR*);

#endif
