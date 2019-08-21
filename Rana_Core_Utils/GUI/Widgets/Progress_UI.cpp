#include "Progress_UI.h"

/*
PROGRESS_MESSAGE_BOX_ARGS
*/

PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args(
MESSAGE_BOX_ARGS args, bool requireAnimationCompleted) {
	PROGRESS_MESSAGE_BOX_ARGS progress_args;
	progress_args.args = args;
	progress_args.active = true;
	progress_args.requireAnimationCompleted = requireAnimationCompleted;
	return progress_args;
}

PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args(
bool requireAnimationCompleted) {
	PROGRESS_MESSAGE_BOX_ARGS progress_args;
	progress_args.active = false;
	progress_args.requireAnimationCompleted = requireAnimationCompleted;
	return progress_args;
}

PROGRESS_MESSAGE_BOX_ARGS Progress_Message_Box_Args() {
	PROGRESS_MESSAGE_BOX_ARGS args;
	args.active = false;
	return args;
}

/*
PROGRESS_UI
*/

void Initialize_PROGRESS_UI(
PROGRESS_UI* ui, EMBEDDED_ANIMATOR anim, MESSAGE_BOX message_box,
PROGRESS_TRACKER progress) {
	ui->anim = anim;
	ui->messageBoxAppearance = message_box;

	ui->progress = progress;
	ui->processStatus = PROCESS_NOT_RUNNING;
}

int8_t Render_PROGRESS_UI(
PROGRESS_UI* ui, struct nk_context* ctx,
struct nk_rect animation_rect, uint8_t total_message_boxes,
/* PROGRESS_MESSAGE_BOX_ARGS */...) {
	PROGRESS_MESSAGE_BOX_ARGS message_box_data[PROCESS_MAX_FINISH_TYPES];
	va_array(message_box_data, total_message_boxes, PROGRESS_MESSAGE_BOX_ARGS);

	// If the process is in progress, render the animation
	if (ui->processStatus == PROCESS_IN_PROGRESS) {
		return Render_Animation_PROGRESS_UI(ui, ctx, animation_rect);
	}
	// If the status is something other than "in progress" and "not running",
	// render the appropriate message box
	else if (ui->processStatus != PROCESS_NOT_RUNNING) {
		return Render_Message_Box_PROGRESS_UI(
		ui, ctx, animation_rect, message_box_data, total_message_boxes);
	}
	else {
		return PROGRESS_UI_NO_MESSAGE_DISPLAYED;
	}
}

void Start_PROGRESS_UI_Uniform(PROGRESS_UI* ui, uint32_t total_updates) {
	Start_Progress_Tracker_Uniform(&ui->progress, total_updates);
	Start_Params_PROGRESS_UI(ui);
}

void Start_PROGRESS_UI(
PROGRESS_UI* ui, float increment_ratios[], uint32_t total_updates) {
	Start_Progress_Tracker(&ui->progress, increment_ratios, total_updates);
	Start_Params_PROGRESS_UI(ui);
}

void Start_PROGRESS_UI(
PROGRESS_UI* ui, uint32_t total_updates, /* float */...) {
	float* increment_ratios = new float[total_updates];
	va_array(increment_ratios, total_updates, double);
	Start_PROGRESS_UI(ui, increment_ratios, total_updates);
	delete [] increment_ratios;
}

void Update_PROGRESS_UI(PROGRESS_UI* ui) {
	Update_Progress_Tracker(&ui->progress);
	Setup_Animation_Clamp_PROGRESS_UI(ui);
}

void Finish_PROGRESS_UI(PROGRESS_UI* ui, int8_t finish_status) {
	Finish_Progress_Tracker(&ui->progress);
	Unclamp_EMBEDDED_ANIMATOR(&ui->anim);
	ui->processStatus = finish_status;
}

void Delete_PROGRESS_UI(PROGRESS_UI* ui) {
	Delete_EMBEDDED_ANIMATOR(&ui->anim);
	Delete_MESSAGE_BOX(&ui->messageBoxAppearance);
	Delete_Progress_Tracker(&ui->progress);
}

void Setup_Animation_Clamp_PROGRESS_UI(PROGRESS_UI* ui) {
	if (!Progress_Finished(&ui->progress)) {
		Clamp_EMBEDDED_ANIMATOR(
		&ui->anim, Total_Frames_EMBEDDED_ANIMATOR(&ui->anim) *
		Progress_Ratio_Completed(&ui->progress));
	}
	else {
		Unclamp_EMBEDDED_ANIMATOR(&ui->anim);
	}
}

int8_t Render_Message_Box_PROGRESS_UI(
PROGRESS_UI* ui, struct nk_context* ctx, struct nk_rect animation_rect,
PROGRESS_MESSAGE_BOX_ARGS message_box_data[], uint8_t total_message_boxes) {
	// Precheck to see if there is message box assigned to this process status
	if (ui->processStatus < total_message_boxes) {
		// Check again to see if the message box needs to wait
		// for the animation to complete before displaying
		if (message_box_data[ui->processStatus].requireAnimationCompleted) {
			if (EMBEDDED_ANIMATOR_Finished(&ui->anim)) {
				return Render_Single_Message_Box_PROGRESS_UI(
				ui, message_box_data[ui->processStatus]);
			}
			else {
				return Render_Animation_PROGRESS_UI(ui, ctx, animation_rect);
			}
		}
		// Render message box immediately,
		// if it does not wait for the animation
		else {
			return Render_Single_Message_Box_PROGRESS_UI(
			ui, message_box_data[ui->processStatus]);
		}
	}
	else {
		return PROGRESS_UI_NO_MESSAGE_DISPLAYED;
	}
}

int8_t Render_Animation_PROGRESS_UI(
PROGRESS_UI* ui, struct nk_context* ctx, struct nk_rect animation_rect) {
	Render_EMBEDDED_ANIMATOR_Standalone(
	&ui->anim, ctx, NO_CHANGE_RECT_TRANSFORM, animation_rect);
	return PROGRESS_UI_NO_MESSAGE_DISPLAYED;
}

int8_t Render_Single_Message_Box_PROGRESS_UI(
PROGRESS_UI* ui, PROGRESS_MESSAGE_BOX_ARGS args) {
	// Render the box only if it should be rendered
	// and the user has not given input on it yet
	if (args.active && !ui->messageBoxInputReceived) {
		int8_t result = Render_MESSAGE_BOX(
		&ui->messageBoxAppearance, args.args);

		// If input received, set the bool
		if (result >= 0) {
			ui->messageBoxInputReceived = true;
			ui->processStatus = PROCESS_NOT_RUNNING;
		}

		return result;
	}
	else {
		return PROGRESS_UI_NO_MESSAGE_DISPLAYED;
	}
}

void Start_Params_PROGRESS_UI(PROGRESS_UI* ui) {
	ui->processStatus = PROCESS_IN_PROGRESS;
	ui->messageBoxInputReceived = false;
	Restart_EMBEDDED_ANIMATOR(&ui->anim);
}

void Set_Process_Status_PROGRESS_UI(
PROGRESS_UI* ui, int8_t status) {
	if (status < PROCESS_MAX_FINISH_TYPES && status >= PROCESS_NOT_RUNNING) {
		ui->processStatus = status;
	}
	else {
		log_err(string("Progress UI cannot represent process finish status ") +
		to_string((int)status));
	}
}

void Set_Process_In_Progress_PROGRESS_UI(PROGRESS_UI* ui) {
	Set_Process_Status_PROGRESS_UI(ui, PROCESS_IN_PROGRESS);
}

void Set_Process_Not_Running_PROGRESS_UI(PROGRESS_UI* ui) {
	Set_Process_Status_PROGRESS_UI(ui, PROCESS_NOT_RUNNING);
}

// ACCESSORS
bool Animation_Finished_PROGRESS_UI(const PROGRESS_UI* ui) {
	return EMBEDDED_ANIMATOR_Finished(&ui->anim);
}
bool Process_Not_Running_PROGRESS_UI(const PROGRESS_UI* ui) {
	return ui->processStatus == PROCESS_NOT_RUNNING;
}
bool Process_In_Progress_PROGRESS_UI(const PROGRESS_UI* ui) {
	return ui->processStatus == PROCESS_IN_PROGRESS;
}
int8_t Process_Finish_Status_PROGRESS_UI(const PROGRESS_UI* ui) {
	return ui->processStatus;
}
