#ifndef PROGRESS_FEEDBACK_H_
#define PROGRESS_FEEDBACK_H_

#include "Embedded_Animator.h"
#include "../../Utilities/Progress_Tracker.h"

#define PROGRESS_FEEDBACK_NO_MESSAGE_DISPLAYED	-2
#define PROCESS_IN_PROGRESS	0
// #define PROCESS_FINISH_WITH_RESULT_A		1
// #define PROCESS_FINISH_WITH_RESULT_B		2
// ...

struct PROGRESS_FEEDBACK {
	EMBEDDED_ANIMATOR anim;
	MESSAGE_BOX completeMessage;

	// Borrowed pointer to the progress displayed. DON'T DELETE
	PROGRESS_TRACKER* progress;
};

void Initialize_PROGRESS_FEEDBACK(
PROGRESS_FEEDBACK*, PROGRESS_TRACKER*, string baseName, string fileExtension,
float widthRatio, float heightRatio, uint8_t totalFrames, bool looping,
struct nk_color buttonColor, struct nk_color textColor,
uint8_t framesPerString, PANEL, struct nk_style_text text_style,
struct nk_style_text informative_text_style,
struct nk_style_button default_button, struct nk_style_button misc_button);

uint8_t Render_PROGRESS_FEEDBACK(PROGRESS_FEEDBACK*, struct nk_context*);

#endif
