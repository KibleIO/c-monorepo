#ifndef CAMERA_CPP_
#define CAMERA_CPP_

#include "Camera.h"

Camera::Camera(int w, int h) {
	width = w;
	height = h;
	c = new Vector3D(0, 0, 0);
	e = new Vector3D(width / -2, height / -2, 500); //500 is an arbitrary distance for eye
	t = new Vector3D(0, 0, 0);
}

Camera::~Camera() {
	delete c;
	delete e;
	delete t;
}

double Camera::Cos(double in) {
	return cos((M_PI / 180) * in);
}

double Camera::Sin(double in) {
	return sin((M_PI / 180) * in);
}

Point Camera::convertTo2D(Vector3D *a) {
	Point b;
	double x = a->x - c->x, y = a->y - c->y, z = a->z - c->z;
	Vector3D d(Cos(t->y) * (Sin(t->z) * y + Cos(t->z) * x) - Sin(t->y) * z,
			Sin(t->x) * (Cos(t->y) * z + Sin(t->y) * (Sin(t->z) * y + Cos(t->z) * x)) + Cos(t->x) * (Cos(t->z) * y - Sin(t->z) * x),
			Cos(t->x) * (Cos(t->y) * z + Sin(t->y) * (Sin(t->z) * y + Cos(t->z) * x)) - Sin(t->x) * (Cos(t->z) * y - Sin(t->z) * x));
	if (d.z < 0) {
		return b;
	}
	b.x = (int) ((e->z / d.z) * d.x - e->x);
	b.y = (int) ((e->z / d.z) * d.y - e->y);
	return b;
}

void Camera::Rotate(Vector3D *rot, Vector3D *loc, Vector3D *center) {
	double x = loc->x - center->x, y = loc->y - center->y, z = loc->z - center->z;
	loc->x = (Cos(rot->y) * (Sin(rot->z) * y + Cos(rot->z) * x) - Sin(rot->y) * z) + center->x;
	loc->y = Sin(rot->x) * (Cos(rot->y) * z + Sin(rot->y) * (Sin(rot->z) * y + Cos(rot->z) * x)) + Cos(rot->x) * (Cos(rot->z) * y - Sin(rot->z) * x) + center->y;
	loc->z = Cos(rot->x) * (Cos(rot->y) * z + Sin(rot->y) * (Sin(rot->z) * y + Cos(rot->z) * x)) - Sin(rot->x) * (Cos(rot->z) * y - Sin(rot->z) * x) + center->z; 
	//sexy algorithm, am I right or am I right?
}

#endif /* CAMERA_CPP_ */
