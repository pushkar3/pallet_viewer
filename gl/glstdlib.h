/*
 * gl_stdtype.h
 *
 *  Created on: Mar 8, 2010
 *      Author: pushkar
 */

#ifndef GL_STDTYPE_H_
#define GL_STDTYPE_H_

#include <math.h>

class Vec3 {
public:
	double x, y, z;
	void set(double _x, double _y, double _z) {
		x = _x; y = _y; z = _z;
	}
	void zero() {
		x = y = z = 0.0f;
	}
	void normalize() {
		double length = sqrt(x*x + y*y + z*z);
		if(length != 0) {
			x /= length;
			y /= length;
			z /= length;
		}
	}
};

class Mat33 {
public:
	double e[9];

	Mat33() {
		identity();
	}

	inline double d(int row, int col) {
		return e[row*3 + col];
	}

	void identity() {
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				d(i, j) = 0.0f;
				if(i == j) d(i, j) = 1.0f;
			}
		}
	}

	Mat33 mult(Mat33 b) {
		Mat33 a;
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				a.d(i, j) = d(i,0)*b.d(0,j)
		        + d(i,1)*b.d(1,j)
		        + d(i,2)*b.d(2,j);
			}
		}
		return a;
	}
};

#endif /* GL_STDTYPE_H_ */
