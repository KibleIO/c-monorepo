#include "Augmented_Graphics.h"

void fillRoundedRect(GRAPHICS* graphics, int x, int y, int w, int h, int r, Color color) {
	// Check to see that the rect trying to be drawn is valid
	if(x < 0 || y < 0 || w < 0 || h < 0) {
		cout << "ERROR: The rect is invalid and cannot be drawn\n";
		cout << "Rect: " << x << ", " << y << ", " << w << ", " << h << endl;
		return;
	}

	// If there is no rounding, the square doesn't have enough rounding to be significant
	// Fill up a regular square
	if(r <= 0) {
		FillSquare_GRAPHICS(graphics, x, y, w, h, color);
	}
	// Otherwise, we've got a long function ahead of us...
	else {
    // Assign half the height or width - whichever is smaller
    int halfMinDimension = h < w ? h >> 1 : w >> 1;

		// Clamp the rounding constant so that it cannot be bigger than half the smallest dimension
		if(r > halfMinDimension) {
			r = halfMinDimension;
		}

		int innerWidth = w - (r << 1);	// Internal width of the rectangular drawing between the quarter circles
		int innerHeight = h - (r << 1);	// Internal height of the rectangular drawing between the quarter circles

		class Polygon innerRect;	// Inner rectangular drawing inside the four quarter circles

		// The +1's and -1's correct an unidentifiable off-by-one error

		// Top-left corner
		innerRect.addPoint(x + r, y);
		innerRect.addPoint(x + r, y + r - 1);
		innerRect.addPoint(x, y + r - 1);

		// Bottom-left corner
		innerRect.addPoint(x, y + r + innerHeight + 1);
		innerRect.addPoint(x + r, y + r + innerHeight + 1);
		innerRect.addPoint(x + r, y + h);

		// Bottom-right corner
		innerRect.addPoint(x + r + innerWidth, y + h);
		innerRect.addPoint(x + r + innerWidth, y + r + innerHeight + 1);
		innerRect.addPoint(x + w, y + r + innerHeight + 1);

		// Top-right corner
		innerRect.addPoint(x + w, y + r - 1);
		innerRect.addPoint(x + r + innerWidth, y + r - 1);
		innerRect.addPoint(x + r + innerWidth, y);

		// Fill up the rectangular shape inside the quarter circles
		FillPolygon_GRAPHICS(graphics, innerRect, color);

		// Fill arcs at the corners to created rounded edges
		fillQuarterArc(graphics, x + innerWidth + r, y, r, r, FIRST_QUADRANT, color);
		fillQuarterArc(graphics, x, y, r, r, SECOND_QUADRANT, color);
		fillQuarterArc(graphics, x, y + innerHeight + r, r, r, THIRD_QUADRANT, color);
		fillQuarterArc(graphics, x + innerWidth + r, y + innerHeight + r, r, r, FOURTH_QUADRANT, color);
	}
}

void fillQuarterArc(GRAPHICS* graphics, int x, int y, int w, int h, int quadrant, Color color) {
	// Prevent function from continuing if dimensions are invalid
	if(x < 0 || y < 0 || w < 0 || h < 0) {
		cout << "ERROR: the arc is invalid and cannot be draw\n";
		cout << "Arc rect: " << x << ", " << y << ", " << w << ", " << h << endl;
		return;
	}

	// Check to see that the quadrant given is valid
	if(quadrant != FIRST_QUADRANT && quadrant != SECOND_QUADRANT && quadrant != THIRD_QUADRANT && quadrant != FOURTH_QUADRANT) {
		cout << "ERROR: " << quadrant << " does not describe a valid quadrant, the arc cannot be drawn\n";
		return;
	}

	// Super mathy constants are needed for Bresenham's algorithm
	const int a2 = (w * w) >> 2;
	const int b2 = (h * h) >> 2;
	const int fa2 = a2 << 2;
	const int fb2 = b2 << 2;

	int controlX, controlY;	// X-y values used to control the loops
	int selector;	// Calculated and used to decide which pixel intesected by the arc will be filled in
	int xPoints[3];	// Points used to draw the polygons that fill up the elliptical arc
  int yPoints[3];

	// Each is positive or negative one, depending on the quadrant the arc is being filled in
	int quadrantXConst = 1;
	int quadrantYConst = 1;

	// X should be negative in second and third quadrants
	if(quadrant == SECOND_QUADRANT || quadrant == THIRD_QUADRANT) {
		quadrantXConst = -1;
    x += w;
	}
	// Y should is negative in the first and second quadrants
	// (Remember that down is positive on a computer screen)
	if(quadrant == FIRST_QUADRANT || quadrant == SECOND_QUADRANT) {
		quadrantYConst = -1;
    y += h;
	}

	// Initialize the points used to generate the polygons that fill in the arc
	xPoints[0] = x;
	yPoints[0] = y;
	xPoints[2] = x;
	yPoints[2] = y;

	// Initialize the selector (I have literally NO idea why it's set to this equation...)
	selector = (b2 << 1) + (a2 * (1 - (h << 1)));

	// Start a loop with the loop control vars
	for(controlX = 0, controlY = h; b2 * controlX <= a2 * controlY; ++controlX) {
		// Don't really get it yet
		xPoints[1] = x + (controlX * quadrantXConst);
		yPoints[1] = y + (controlY * quadrantYConst);

		// Initialize and draw the polygon (yuk, inner loop is inefficient...)
		class Polygon poly;
		for(int point = 0; point < 3; ++point) {
			poly.addPoint(xPoints[point], yPoints[point]);
		}
		FillPolygon_GRAPHICS(graphics, poly, color);

		// Ovewrite previous points with these points
		xPoints[2] = xPoints[1];
    yPoints[2] = yPoints[1];

		// If selector is positive, aggregate some mathy number into it
		// (hint: the number is almost always negative) and update control y value
		if(selector >= 0) {
			selector += fa2 * (1 - controlY);
			--controlY;
		}

		// Aggregate something mathy into the selector
		selector += b2 * ((controlX << 2) + 6);
	} // LOL THIS IS JUST THE FIRST HALF

	// Assign the selector something NEW and just as mathy
	selector = (a2 << 1) + (b2 * (1 - (w << 1)));

	// Start a new loop for the next half (bottom half? right half? no idea)
	for(controlX = w, controlY = 0; a2 * controlY <= b2 * controlX; ++controlY) {
		// Still don't really get it
		xPoints[1] = x + (controlX * quadrantXConst);
		yPoints[1] = y + (controlY * quadrantYConst);

		// Initialize and draw the polygon (yuk, inner loop is inefficient...)
		class Polygon poly;
		for(int point = 0; point < 3; ++point) {
			poly.addPoint(xPoints[point], yPoints[point]);
		}
		FillPolygon_GRAPHICS(graphics, poly, color);

    // Ovewrite previous points with these points
		xPoints[2] = xPoints[1];
    yPoints[2] = yPoints[1];

		// If selector is positive, aggregate some mathy number into it
		// (hint: the number is almost always negative) and update control y value
		if(selector >= 0) {
			selector += fb2 * (1 - controlX);
			--controlX;
		}

		// Aggregate something mathy into the selector
		selector += a2 * ((controlY << 2) + 6);
	}
}

// Drawing a thick "line" really involves drawing a rectangle where thickness is the height
void drawThickHorizontalLine(GRAPHICS* graphics, int x, int y, int w, int thickness, Color color) {
	FillSquare_GRAPHICS(graphics, x, y, w, thickness, color);
}

// Drawing a thick line is really a rectangle where thickness is the width
void drawThickVerticalLine(GRAPHICS* graphics, int x, int y, int h, int thickness, Color color) {
  FillSquare_GRAPHICS(graphics, x, y, thickness, h, color);
}

// Draw a rect with the desired thickness by drawing four lines
void drawThickRect(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, Color color) {
	drawThickHorizontalLine(graphics, x, y, w, thickness, color);
	drawThickHorizontalLine(graphics, x, y + h - thickness, w, thickness, color);
	drawThickVerticalLine(graphics, x, y, h, thickness, color);
	drawThickVerticalLine(graphics, x + w - thickness, y, h, thickness, color);
}

void drawThickRoundedRect(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, int r, Color color) {
	// Check to see that the rect trying to be drawn is valid
	if(x < 0 || y < 0 || w < 0 || h < 0) {
		cout << "ERROR: The rect is invalid and cannot be drawn\n";
		cout << "Rect: " << x << ", " << y << ", " << w << ", " << h << endl;
		return;
	}

	// If there is no rounding, the square doesn't have enough rounding to be significant
	// Draw a regular thick rect
	if(r <= 0) {
		drawThickRect(graphics, x, y, w, h, thickness, color);
	}
	// Otherwise, we've got a long function ahead of us...
	else {
    // Assign half the height or width - whichever is smaller
    int halfMinDimension = h < w ? h >> 1 : w >> 1;

		// Clamp the rounding constant so that it cannot be bigger than half the smallest dimension
		if(r > halfMinDimension) {
			r = halfMinDimension;
		}

		int innerWidth = w - (r << 1);	// Internal width of the rectangular drawing between the quarter circles
		int innerHeight = h - (r << 1);	// Internal height of the rectangular drawing between the quarter circles

		// Draw thick horizontal/vertical lines
		drawThickHorizontalLine(graphics, x + r, y, w - (r << 1), thickness, color);
		drawThickHorizontalLine(graphics, x + r, y + h - thickness, w - (r << 1), thickness, color);
		drawThickVerticalLine(graphics, x, y + r, h - (r << 1), thickness, color);
		drawThickVerticalLine(graphics, x + w - thickness, y + r, h - (r << 1), thickness, color);

    drawThickQuarterArc(graphics, x + innerWidth + r, y, r, r, thickness, FIRST_QUADRANT, color);
    drawThickQuarterArc(graphics, x, y, r, r, thickness, SECOND_QUADRANT, color);
    drawThickQuarterArc(graphics, x, y + innerHeight + r, r, r, thickness, THIRD_QUADRANT, color);
    drawThickQuarterArc(graphics, x + innerWidth + r, y + innerHeight + r, r, r, thickness, FOURTH_QUADRANT, color);
	}
}

void drawQuarterArc(GRAPHICS* graphics, int x, int y, int w, int h, int quadrant, Color color) {
  // Prevent function from continuing if dimensions are invalid
  if(x < 0 || y < 0 || w < 0 || h < 0) {
    cout << "ERROR: the arc is invalid and cannot be draw\n";
    cout << "Arc rect: " << x << ", " << y << ", " << w << ", " << h << endl;
    return;
  }

  // Check to see that the quadrant given is valid
  if(quadrant != FIRST_QUADRANT && quadrant != SECOND_QUADRANT && quadrant != THIRD_QUADRANT && quadrant != FOURTH_QUADRANT) {
    cout << "ERROR: " << quadrant << " does not describe a valid quadrant, the arc cannot be drawn\n";
    return;
  }

  // Super mathy constants are needed for Bresenham's algorithm
  const int a2 = (w * w) >> 2;
  const int b2 = (h * h) >> 2;
  const int fa2 = a2 << 2;
  const int fb2 = b2 << 2;

  int controlX, controlY;	// X-y values used to control the loops
  int selector;	// Calculated and used to decide which pixel intesected by the arc will be filled in

  // Each is positive or negative one, depending on the quadrant the arc is being filled in
  int quadrantXConst = 1;
  int quadrantYConst = 1;

  // X should be negative in second and third quadrants
  if(quadrant == SECOND_QUADRANT || quadrant == THIRD_QUADRANT) {
    quadrantXConst = -1;
    x += w;
  }
  // Y should is negative in the first and second quadrants
  // (Remember that down is positive on a computer screen)
  if(quadrant == FIRST_QUADRANT || quadrant == SECOND_QUADRANT) {
    quadrantYConst = -1;
    y += h;
  }

  // Initialize the selector (I have literally NO idea why it's set to this equation...)
  selector = (b2 << 1) + (a2 * (1 - (h << 1)));

  // Start a loop with the loop control vars
  for(controlX = 0, controlY = h; b2 * controlX <= a2 * controlY; ++controlX) {
    // Don't really get it yet
    DrawPoint_GRAPHICS(graphics, x + (controlX * quadrantXConst), y + (controlY * quadrantYConst), color);

    // If selector is positive, aggregate some mathy number into it
    // (hint: the number is almost always negative) and update control y value
    if(selector >= 0) {
      selector += fa2 * (1 - controlY);
      --controlY;
    }

    // Aggregate something mathy into the selector
    selector += b2 * ((controlX << 2) + 6);
  } // LOL THIS IS JUST THE FIRST HALF

  // Assign the selector something NEW and just as mathy
  selector = (a2 << 1) + (b2 * (1 - (w << 1)));

  // Start a new loop for the next half (bottom half? right half? no idea)
  for(controlX = w, controlY = 0; a2 * controlY <= b2 * controlX; ++controlY) {
    // Still don't really get it
    DrawPoint_GRAPHICS(graphics, x + (controlX * quadrantXConst), y + (controlY * quadrantYConst), color);

    // If selector is positive, aggregate some mathy number into it
    // (hint: the number is almost always negative) and update control y value
    if(selector >= 0) {
      selector += fb2 * (1 - controlX);
      --controlX;
    }

    // Aggregate something mathy into the selector
    selector += a2 * ((controlY << 2) + 6);
  }
}

// Draw a quarter of an elliptical arc with the desired thickness
void drawThickQuarterArc(GRAPHICS* graphics, int x, int y, int w, int h, int thickness, int quadrant, Color color) {
  for(int arc = 0; arc < thickness; ++arc) {
    drawQuarterArc(graphics, x, y, w - arc, h - arc, quadrant, color);
  }
}
