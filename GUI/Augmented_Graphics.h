/*
Augmented_GRAPHICS.h
--------------------
Adds some functionality to the GRAPHICS class in Rana's Core Utils, such as filled
rects with rounded edges and drawing filled arcs and thicker borders
--------------------
*/
#ifndef AUGMENTED_GRAPHICS_H_
#define AUGMENTED_GRAPHICS_H_

#include <Graphics/Graphics.h>

#define FIRST_QUADRANT 1
#define SECOND_QUADRANT 2
#define THIRD_QUADRANT 3
#define FOURTH_QUADRANT 4

//void drawThickLine(GUI*, int x1, int y1, int x2, int y2, int thickness, Color);
	// Draw a line with the specified thickness
//void drawRoundedThickSquare(GUI*, int x, int y, int w, int h, int rounding, int thickness, Color);
	// Draw an unfilled square with the desired rounding on the edges and thickness
void fillRoundedRect(GRAPHICS*, int x, int y, int w, int h, int rounding, Color);
	// Draw a filled square with the desired rounding on the edges
void fillQuarterArc(GRAPHICS*, int x, int y, int w, int h, int quadrant, Color);
	// Fill a quarter of an elliptical arc in the quadrant specified
void drawThickHorizontalLine(GRAPHICS*, int x, int y, int w, int thickness, Color);
	// Draw a line horizontally starting at the TOP LEFT and going w pixels to the right with the desired thickness
void drawThickVerticalLine(GRAPHICS*, int x, int y, int h, int thickness, Color);
	// Draw a line vertically starting at the TOP LEFT and going h pixels down with the desired thickness
void drawThickRect(GRAPHICS*, int x, int y, int w, int h, int thickness, Color);
	// Draw an unfilled rect area with the desired border thickness
void drawThickRoundedRect(GRAPHICS*, int x, int y, int w, int h, int thickness, int rounding, Color);
	// Draw an unfilled rect area with the desired border thickness and rounding off of the edges
void drawQuarterArc(GRAPHICS*, int x, int y, int w, int h, int quadrant, Color);
	// Draw a quarter of an elliptical arc in the quadrant specified
void drawThickQuarterArc(GRAPHICS*, int x, int y, int w, int h, int thickness, int quadrant, Color);

#endif // AUGMENTED_GRAPHICS_H_
