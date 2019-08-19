#include "Progress_Tracker.h"

PROGRESS_TRACKER Progress_Tracker(uint32_t min, uint32_t max) {
	PROGRESS_TRACKER progress;
	Initialize_Progress_Tracker(&progress, min, max);
	return progress;
}

void Initialize_Progress_Tracker(PROGRESS_TRACKER* progress, uint32_t min,
uint32_t max) {
	// If min is actually bigger, swap min and max
	if(min > max) {
		uint32_t temp = min;
		min = max;
		max = temp;
	}

	// Set min/max
	progress->min = min;
	progress->max = max;

	// Set increments to null pointer
	progress->progressIncrements = NULL;
	Reset_Progress_Tracker(progress);
}

void Delete_Progress_Tracker(PROGRESS_TRACKER* progress) {
	if(progress->progressIncrements != NULL) {
		delete [] progress->progressIncrements;
	}
}

void Start_Progress_Tracker_Uniform(
PROGRESS_TRACKER* progress, uint32_t total_updates) {
	if(total_updates != 0) {
		uint32_t diff = progress->max - progress->min;

		// Array with each progress increment
		uint32_t* increments = new uint32_t[total_updates];
		uint32_t sum = 0;	// Sum of the progress increment

		// Setup all of the progress increments, keeping track of the sum
		for(uint32_t i = 0; i < total_updates; i++) {
			increments[i] = diff / total_updates;
			sum += increments[i];
		}

		// Add back any data lost in the integer division
		increments[total_updates - 1] += diff - sum;

		// Call overload that takes the increments
		Start_Progress_Tracker(progress, increments, total_updates);
	}
}

void Start_Progress_Tracker(
PROGRESS_TRACKER* progress, float* increment_ratios, uint32_t total_updates) {
	if(total_updates != 0) {
		uint32_t diff = progress->max - progress->min;

		// Array with each progress increment
		uint32_t* increments = new uint32_t[total_updates];
		uint32_t sum = 0;

		// Scale the ratios to fit within the number "100"
		Scale(increment_ratios, total_updates, (float)diff);

		// Get each increment by multiplying the difference in min-max
		// by the corresponding ratio
		for(uint32_t i = 0; i < total_updates; i++) {
			increments[i] = lround(increment_ratios[i]);
			sum += increments[i];
		}

		// Add back data lost from the multiplication
		increments[total_updates - 1] += diff - sum;

		// Call overload that takes exact increments
		Start_Progress_Tracker(progress, increments, total_updates);
	}
}

void Start_Progress_Tracker(
PROGRESS_TRACKER* progress, uint32_t total_updates, /* float */ ...) {
	float* increment_ratios = new float[total_updates];
	va_array(increment_ratios, total_updates, double);
	Start_Progress_Tracker(progress, increment_ratios, total_updates);
	delete [] increment_ratios;
}

// NOTE: function MOVES ownership of "progress_increments" to the tracker
// All the more reason why client code shouldn't bother with this function
void Start_Progress_Tracker(
PROGRESS_TRACKER* progress, uint32_t* progress_increments,
uint32_t total_updates) {
	progress->totalIncrements = total_updates;
	progress->currentProgress = progress->min;
	progress->currentIncrement = 0;

	if (progress->progressIncrements != NULL) {
		delete [] progress->progressIncrements;
	}
	progress->progressIncrements = progress_increments;
}

void Update_Progress_Tracker(PROGRESS_TRACKER* progress) {
	// Add the increment to current progress;
	// simultaneously update current increment
	if(progress->currentIncrement < progress->totalIncrements) {
		progress->currentProgress +=
		progress->progressIncrements[progress->currentIncrement++];
	}
}

void Finish_Progress_Tracker(PROGRESS_TRACKER* progress) {
	progress->currentProgress = progress->max;
}

void Reset_Progress_Tracker(PROGRESS_TRACKER* progress) {
	progress->currentProgress = progress->min;
	progress->currentIncrement = 0;
	progress->totalIncrements = 0;

	if(progress->progressIncrements != NULL) {
		delete [] progress->progressIncrements;
		progress->progressIncrements = NULL;
	}
}

/*
ACCESSORS
*/

uint32_t Current_Progress(const PROGRESS_TRACKER* progress) {
	return progress->currentProgress;
}
uint32_t Current_Increment(const PROGRESS_TRACKER* progress) {
	return progress->currentIncrement;
}
uint32_t Total_Increments(const PROGRESS_TRACKER* progress) {
	return progress->totalIncrements;
}
uint32_t Progress_Magnitude(const PROGRESS_TRACKER* progress) {
	return progress->max - progress->min;
}
float Progress_Ratio_Completed(const PROGRESS_TRACKER* progress) {
	return Current_Progress(progress) / (float)Progress_Magnitude(progress);
}
bool Progress_Finished(const PROGRESS_TRACKER* progress) {
	return progress->currentProgress >= progress->max;
}
