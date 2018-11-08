#ifndef VECTOR3D_H_
#define VECTOR3D_H_

struct Vector3D {
double x, y, z;
Vector3D() : x(0.0), y(0.0), z(0.0) {}
Vector3D(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
};

#endif /* VECTOR3D_H_ */