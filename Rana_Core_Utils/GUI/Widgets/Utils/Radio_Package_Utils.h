#ifndef RADIO_PACKAGE_UTILS_H_
#define RADIO_PACKAGE_UTILS_H_

#include <cstdint>

int16_t Index_Of_Selected(const bool*[], uint8_t size);
bool Select(bool*[], uint8_t size, uint8_t selection);
bool Deselect(
bool*[], uint8_t size, uint8_t deselection, bool require_one_selected = false);
void Deselect_All(bool*[], uint8_t size, bool require_one_selected = false);

#endif
