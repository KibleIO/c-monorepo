#ifndef PROGRESS_UI_H_
#define PROGRESS_UI_H_

#include "Embedded_Animator.h"
#include "Message_Box.h"
#include "../../Utilities/Progress_Tracker.h"
#include "../../Utilities/LOGGING.h"

/*
PROGRESS_MESSAGE_BOX_ARGS
*/

struct PROGRESS_MESSAGE_BOX_ARGS {
	MESSAGE_BOX_ARGS args;
	bool active;
	// If true, the message box is not rendered until the animation is finished
	bool requireAnimationCompleted;
};

// Construct an active message box from the data
PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args(
MESSAGE_BOX_ARGS, bool requireAnimationCompleted);
PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args(
bool requireAnimationCompleted);
// Construct an inactive message box
PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args();

/*
PROGRESS_UI
*/

#define PROGRESS_UI_NO_MESSAGE_DISPLAYED	-2

#define PROCESS_MAX_FINISH_TYPES 16
#define PROCESS_NOT_RUNNING	-2
#define PROCESS_IN_PROGRESS	-1
// #define PROCESS_FINISH_WITH_RESULT_A		0
// #define PROCESS_FINISH_WITH_RESULT_B		1
// ...

struct PROGRESS_UI {
	EMBEDDED_ANIMATOR anim;
	MESSAGE_BOX messageBoxAppearance;
	bool messageBoxInputReceived;

	// Current progress of the process being run
	PROGRESS_TRACKER progress;
	int8_t processStatus;
};

void Initialize_PROGRESS_UI(
PROGRESS_UI*, EMBEDDED_ANIMATOR, MESSAGE_BOX, PROGRESS_TRACKER);
uint8_t Render_PROGRESS_UI(
PROGRESS_UI*, struct nk_context*, struct nk_rect, uint8_t total_message_boxes,
/* PROGRESS_MESSAGE_BOX_ARGS */...);
void Delete_PROGRESS_UI(PROGRESS_UI*);

/*
Progress tracker delegates: start and update the progress while also updating
the process's current status to render the correct widgets
*/

// START PROGRESS
void Start_PROGRESS_UI_Uniform(PROGRESS_UI*, uint32_t total_updates);
void Start_PROGRESS_UI(
PROGRESS_UI*, float increment_ratios[], uint32_t total_updates);
void Start_PROGRESS_UI(PROGRESS_UI*, uint32_t total_updates, /* float */...);
// UPDATE PROGRESS
void Update_PROGRESS_UI(PROGRESS_UI*);
// FINISH PROGRESS
void Finish_PROGRESS_UI(PROGRESS_UI*, int8_t finish_status);

// HELPERS
void Setup_Animation_Clamp_PROGRESS_UI(PROGRESS_UI*);
int8_t Render_Message_Box_PROGRESS_UI(
PROGRESS_UI*, struct nk_context*, struct nk_rect, PROGRESS_MESSAGE_BOX_ARGS[],
uint8_t total_message_boxes);
int8_t Render_Animation_PROGRESS_UI(
PROGRESS_UI*, struct nk_context*, struct nk_rect);
int8_t Render_Single_Message_Box_PROGRESS_UI(
PROGRESS_UI*, PROGRESS_MESSAGE_BOX_ARGS);
void Start_Params_PROGRESS_UI(PROGRESS_UI*);

// MUTATORS
void Set_Process_Status_PROGRESS_UI(PROGRESS_UI*, int8_t);
void Set_Process_In_Progress_PROGRESS_UI(PROGRESS_UI*);
void Set_Process_Not_Running_PROGRESS_UI(PROGRESS_UI*);

// ACCESSORS
bool Animation_Finished_PROGRESS_UI(const PROGRESS_UI*);

#endif
