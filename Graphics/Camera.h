#ifndef CAMERA_H_
#define CAMERA_H_

#include <cmath>
#include <stdio.h>
#include "Vector3D.h"
#include "Point.h"

using namespace std;

class Camera {
	private:
		Vector3D* c, *e, *t;
		int width, height;
	public:
		Camera(int, int);
		~Camera();
		Point convertTo2D(Vector3D *);
		void Rotate(Vector3D *, Vector3D *, Vector3D *);
		double Cos(double in);
		double Sin(double in);
};

#endif /* CAMERA_H_ */
