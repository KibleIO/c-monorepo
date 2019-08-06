#ifndef RADIO_PACKAGE_UTILS_H_
#define RADIO_PACKAGE_UTILS_H_

#include <cstdint>
#include <iostream>
using namespace std;

/*
FUNCTIONS
*/

void Radio_Select(
bool*[], uint8_t size, uint8_t index, bool require_one_selected);
void Radio_Deselect(
bool*[], uint8_t size, uint8_t index, bool require_one_selected);
void Make_Radio_Selection(
bool*[], uint8_t size, uint8_t index, bool require_one_selected,
bool selection);

int16_t Index_Of_Selected(const bool*[], uint8_t size);

// Ensure that only one bool is true
// (or possibly none if require_one_selected is false)
void Enforce_Radio(
bool*[], uint8_t size, int16_t index_clicked, bool require_one_selected);
void Enforce_Radio_On_Clicked(
bool*[], uint8_t size, int16_t index_clicked, bool require_one_selected,
bool clicked);

/*
HELPERS
*/

// Falsify all but the ignored bool
void Deselect_All_But_One(bool*[], uint8_t size, int16_t ignored);
// Select the given bool if no others are currently selected,
// and at least one is required to be selected
void Select_If_None_Selected(
bool*[], uint8_t size, int16_t select, bool require_one_selected);

#endif
