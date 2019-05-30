//PLATFORMS: Linux, Windows, OSX

#ifndef POLYGON_CPP_
#define POLYGON_CPP_

#include "Polygon.h"

POLYGON::POLYGON() {
	x_coords = NULL;
	y_coords = NULL;
	corners = 0;
}

POLYGON::~POLYGON() {
	if (x_coords) {
		delete [] x_coords;
	}
	if (y_coords) {
		delete [] y_coords;
	}
}

void POLYGON::addPoint(double x, double y) {
	if (x_coords == NULL) {
		left = x;
		right = x;
		top = y;
		bottom = y;
	} else {
		if (x > right) {
			right = x;
		} else if (x < left) {
			left = x;
		}
		if (y > bottom) {
			bottom = y;
		} else if (y < top) {
			top = y;
		}
	}
	corners++;
	double *x_temp = new double[corners];
	double *y_temp = new double[corners];
	for (int i = 0; i < corners - 1; i++) {
		x_temp[i] = x_coords[i];
		y_temp[i] = y_coords[i];
	}
	if (x_coords) {
		delete [] x_coords;
	}
	if (y_coords) {
		delete [] y_coords;
	}
	x_coords = x_temp;
	y_coords = y_temp;
	x_coords[corners - 1] = x;
	y_coords[corners - 1] = y;
}

double POLYGON::GetX(int i) {
	if (i < 0 || i >= corners) {
		log_err("accessing out of bounds point " + to_string(i));
		return 0;
	}
	return x_coords[i];
}

double POLYGON::GetY(int i) {
	if (i < 0 || i >= corners) {
		log_err("accessing out of bounds point " + to_string(i));
		return 0;
	}
	return y_coords[i];
}

#endif
