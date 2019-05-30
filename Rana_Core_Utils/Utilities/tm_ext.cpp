#include "tm_ext.h"

void tmset(tm* t, int day, int month, int year) {
	// If day of month is valid and pointer is non-null, modify it
	if(dm_valid(day, month) && t != NULL) {
		// Setup the day, month and year
		t->tm_mday = day;
		t->tm_mon = month;
		t->tm_year = year;

		// Calculate the current day of the week and of the year
		t->tm_wday = dayofweek(day, month, year);
		t->tm_yday = dayofyear(day, month);
	}
}

void tmset(tm* t, int day, int month) {
	if (t != NULL) {
		tmset(t, day, month, t->tm_year);
	}
}

int dayofweek(int day, int month, int year) {
	// Initial error checks
	if(!dm_valid(day, month)) {
		return -1;
	}

	// Convert to 1-based for algorithm
	month++;
	year += TM_YEAR_OFFSET;

	int utility_ar[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

	// Decrement year if month is less than 3
	year -= month < 3;

	// do the thing that gets the result
	return ( year + year / 4 - year / 100 + year / 400 +
	utility_ar[month - 1] + day) % DAYS_IN_WEEK;
}

int dayofyear(int day, int month) {
	if(!dm_valid(day, month)) {
		return -1;
	}

	// Add up all the days in the month before the current month
	int sum = 0;
	for(int i = 0; i < month; i++) {
		sum += DAYS_IN_MONTH[i];
	}

	return sum + day;
}

bool dm_valid(int day, int month) {
	return month >= 0 && month < TOTAL_MONTHS &&
	day >= 1 && day <= DAYS_IN_MONTH[month];
}

std::string tm_string(const char* format, tm* t) {
	char buffer[MAX_TM_STRING_BUFFER];
	strftime(buffer, MAX_TM_STRING_BUFFER, format, t);
	return std::string(buffer);
}
