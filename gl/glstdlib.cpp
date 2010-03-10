/*
 * glstdlib.cpp
 *
 *  Created on: Mar 8, 2010
 *      Author: pushkar
 */

#include "glstdlib.h"
#include <math.h>

void Vec3::normalize() {
	double length = sqrt(x*x + y*y + z*z);
	if(length != 0) {
		x /= length;
		y /= length;
		z /= length;
	}
}

void Vec3::rot(double az, double inc) {
	normalize();
	double th = acos(z);
	double ph = atan2(y, x);
	x = sin(th+inc)*cos(ph+az);
	y = sin(th+inc)*sin(ph+az);
	if(fabs(th+inc) > 0.01) z = cos(th+inc);
	normalize();
}

void Vec3::rotx(double a) {
	double _x, _y, _z;
	_x = x;
	_y = y*cos(a) - z*sin(a);
	_z = z*cos(a) + y*sin(a);
	set(_x, _y, _z);
}

void Vec3::roty(double a) {
	double _x, _y, _z;
	_x = x*cos(a) + z*sin(a);
	_y = y;
	_z = -x*sin(a) + z*cos(a);
	set(_x, _y, _z);
}

void Vec3::rotz(double a) {
	double _x, _y, _z;
	_x = x*cos(a) - y*sin(a);
	_y = y * cos(a) + x*sin(a);
	_z = z;
	set(_x, _y, _z);
}

void Vec3::add(Vec3 b) {
	x += b.x;
	y += b.y;
	z += b.z;
}

Vec3 Vec3::cross(Vec3 b) {
	Vec3 r;
	r.x = y*b.z - z*b.y;
	r.y = z*b.x - x*b.z;
	r.z = x*b.y - y*b.x;
	return r;
}
