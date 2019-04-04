//PLATFORMS: Linux, Windows, OSX

#ifndef POLYGON_H_
#define POLYGON_H_

#include <stdio.h>

using namespace std;

class POLYGON {
	private:
		double* x_coords;
		double* y_coords;
	public:
		int corners;
		double top, bottom; //minY, maxY
		double left, right; //minX, maxX
		POLYGON();
		~POLYGON();
		void addPoint(double x, double y);
		double GetX(int i);
		double GetY(int i);
};

#endif
