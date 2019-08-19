#ifndef _RGB2YUV_h
#define _RGB2YUV_h

#include <stdint.h>
#include <stddef.h>

using namespace std;

void rgb_to_y420p(
uint8_t* destination, uint8_t* rgb, size_t width, size_t height);

void rgb_to_y420p_c(
uint8_t* destination, uint8_t* rgb, size_t width, size_t height,
size_t screen_width);

#endif
