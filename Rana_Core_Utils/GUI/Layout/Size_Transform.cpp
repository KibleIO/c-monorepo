#include "Size_Transform.h"

uint32_t Header_Height(uint32_t height) {
	return Ratio_Of(height, HEADER_RATIO);
}
uint32_t Body_Height(uint32_t height) {
	return Ratio_Of(height, BODY_RATIO);
}
uint32_t Footer_Height(uint32_t height) {
	return Ratio_Of(height, FOOTER_RATIO);
}
uint32_t Bottom_Buffer_Height(uint32_t height) {
	return Ratio_Of(height, BOTTOM_BUFFER_RATIO);
}
uint32_t Side_Buffer_Width(uint32_t width) {
	return Ratio_Of(width, SIDE_BUFFER_RATIO);
}

uint32_t Text_Button_Height(uint32_t height) {
	return Ratio_Of(height, TEXT_BUTTON_HEIGHT_RATIO);
}
uint32_t Text_Button_Width(uint32_t width) {
	return Ratio_Of(width, TEXT_BUTTON_WIDTH_RATIO);
}
uint32_t Text_Button_Rounding(uint32_t button_height) {
	return Ratio_Of(button_height, TEXT_BUTTON_ROUNDING_RATIO);
}

uint32_t Arrow_Button_Height(uint32_t height) {
	return Ratio_Of(height, ARROW_BUTTON_HEIGHT_RATIO);
}
uint32_t Arrow_Button_Width(uint32_t height) {
	return Ratio_Of(Arrow_Button_Height(height), ARROW_BUTTON_WIDTH_RATIO);
}

uint32_t Ratio_Of(uint32_t size, float ratio) {
	return (uint32_t)round(size * ratio);
}
