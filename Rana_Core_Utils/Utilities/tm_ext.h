#ifndef TM_EXT_H_
#define TM_EXT_H_

#include <ctime>
#include <string>

#define TOTAL_MONTHS 12
#define DAYS_IN_WEEK 7

#define TM_YEAR_OFFSET 1900

#define MAX_TM_STRING_BUFFER 50

const int DAYS_IN_MONTH[TOTAL_MONTHS] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// Set the given time structure to the given day, month, and year
// Each argument should be passed in the same standard format of the data
// in the struct. Day should start at 1, month starts at 0, and year is taken
// as an offset from 1900 (so for instance 2020 is actually 120)
// Seconds, minutes, hours and "tm_dst" are unmodified
// If NULL is passed, a new tm is constructed and returned
// The new tm has tm_sec, tm_min, tm_hour, and tm_dst set to 0
// If any of the arguments given are invalid, the given structure is unmodified
void tmset(tm*, int day, int month, int year);
void tmset(tm*, int day, int month);

/*
UTILITIES
---------
All values should be passed in the format that they appear in a tm struct
---------
*/

// Get the day of the week [0-6] for the given day of the given month
// of the given year. Return -1 if any value is invalid
int dayofweek(int day, int month, int year);
// Get the day of the year [0-365] from the given month
// and given day of the month. If either value given is invalid, returns -1
int dayofyear(int day, int month);

// Return true if the month given and the day of the month given are valid
bool dm_valid(int day, int month);

/*
TM STRINGS
*/

std::string tm_string(const char* format, const tm*);

#endif
