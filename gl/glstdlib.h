/*
 * gl_stdtype.h
 *
 *  Created on: Mar 8, 2010
 *      Author: pushkar
 *      Brief: Standard Library for glcamera
 */

#ifndef GL_STDTYPE_H_
#define GL_STDTYPE_H_

class Vec3 {
public:
	double x, y, z;
	void set(double _x, double _y, double _z) {
		x = _x; y = _y; z = _z;
	}
	void set(Vec3 b) {
		x = b.x; y = b.y; z = b.z;
	}
	void set_unit_x() {
		set(1, 0, 0);
	}
	void set_unit_y() {
		set(0, 1, 0);
	}
	void set_unit_z() {
		set(0, 0, 1);
	}
	void set_null() {
		set(0, 0, 0);
	}
	void scale(double s) {
		set(x*s, y*s, z*s);
	}
	void zero() {
		x = y = z = 0.0f;
	}
	void normalize();
	void rot(double az, double inc);
	void rotx(double a);
	void roty(double a);
	void rotz(double a);
	void add(Vec3 b);
	Vec3 cross(Vec3 b);
};

#endif /* GL_STDTYPE_H_ */
