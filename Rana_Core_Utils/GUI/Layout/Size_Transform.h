#ifndef SIZE_TRANSFORM_H_
#define SIZE_TRANSFORM_H_

#include <cstdint>
#include <cmath>

/*
Express standardized sizes of buttons and layout areas as ratios of the
window's overall width and height
*/

#define HEADER_RATIO 0.05
#define BODY_RATIO (1.0 - \
((HEADER_RATIO) + (FOOTER_RATIO) + (BOTTOM_BUFFER_RATIO)))
#define FOOTER_RATIO 0.1
#define BOTTOM_BUFFER_RATIO 0.05
#define SIDE_BUFFER_RATIO 0.05

#define TEXT_BUTTON_HEIGHT_RATIO 0.1
#define TEXT_BUTTON_WIDTH_RATIO 0.25
#define TEXT_BUTTON_ROUNDING_RATIO 0.2

#define ARROW_BUTTON_HEIGHT_RATIO TEXT_BUTTON_HEIGHT_RATIO
#define ARROW_BUTTON_WIDTH_RATIO 1.2

uint32_t Header_Height(uint32_t height);
uint32_t Body_Height(uint32_t height);
uint32_t Footer_Height(uint32_t height);
uint32_t Bottom_Buffer_Height(uint32_t height);
uint32_t Side_Buffer_Width(uint32_t width);

uint32_t Text_Button_Height(uint32_t height);
uint32_t Text_Button_Width(uint32_t width);
uint32_t Text_Button_Rounding(uint32_t button_height);

uint32_t Arrow_Button_Height(uint32_t height);
uint32_t Arrow_Button_Width(uint32_t height);

uint32_t Ratio_Of(uint32_t size, float ratio);

#endif
