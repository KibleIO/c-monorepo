#ifndef POLYGON_CPP_
#define POLYGON_CPP_

#include "Polygon.h"

Polygon::Polygon() {
	x_coords = NULL;
	y_coords = NULL;
	corners = 0;
}

Polygon::~Polygon() { /*delete [] x_coords; delete [] y_coords;*/ }

void Polygon::addPoint(double x, double y) {
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
	delete [] x_coords;
	delete [] y_coords;
	x_coords = x_temp;
	y_coords = y_temp;
	x_coords[corners - 1] = x;
	y_coords[corners - 1] = y;
}

double Polygon::GetX(int i) {
	return x_coords[i];
}

double Polygon::GetY(int i) {
	return y_coords[i];
}

#endif
