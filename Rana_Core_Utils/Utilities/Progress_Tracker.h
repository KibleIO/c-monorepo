#ifndef PROGRESS_TRACKER_H_
#define PROGRESS_TRACKER_H_

#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include "utilities.h"
#include "vararg_ext.h"
using namespace std;

// Encapsulates the information a method needs to keep track of and update
// its own progress
struct PROGRESS_TRACKER {
	uint32_t min;
	uint32_t max;
	// Current progress of the method
	uint32_t currentProgress;
	// Progress increases by each element in this array
	uint32_t* progressIncrements;
	uint32_t currentIncrement;
	uint32_t totalIncrements;
};

// INITIALIZATION/DELETION
void Initialize_Progress_Tracker(PROGRESS_TRACKER*, uint32_t min, uint32_t max);
void Delete_Progress_Tracker(PROGRESS_TRACKER*);

/*
EXECUTION
---------
Each assumes that min-max has already been set
---------
*/
// Make each progress increment uniform based on the min/max difference
void Start_Progress_Tracker(PROGRESS_TRACKER*, uint32_t total_updates);
// Calculate each progress increment based on an array of ratios
// The function calculates the exact value that the progress increments need
// to be based on the ratios and the min/max difference
void Start_Progress_Tracker(
PROGRESS_TRACKER*, float* increment_ratios, uint32_t total_updates);
void Start_Progress_Tracker(
PROGRESS_TRACKER*, uint32_t total_updates, /* float */ ...);
// Set the exact progress increments of the progress tracker
// NOTE: it is NOT recommended for client code to call this, since this function
// assumes that all the progress increments add up to the min/max difference
// If you want custom progress increments, give ratios in the function above
void Start_Progress_Tracker(
PROGRESS_TRACKER*, uint32_t* progress_increments, uint32_t total_updates);
// Increment current progress by the current incremental step, and go
// to the next step
void Update_Progress_Tracker(PROGRESS_TRACKER*);
// Necessary?
void Reset_Progress_Tracker(PROGRESS_TRACKER*);

/*
ACCESSORS
---------
*/
uint32_t Current_Progress(const PROGRESS_TRACKER*);
// Current increment on the progress tracker
uint32_t Current_Increment(const PROGRESS_TRACKER*);
// True if progress is finished on the tracker
bool Progress_Finished(const PROGRESS_TRACKER*);

#endif // PROGRESS_TRACKER_H_
