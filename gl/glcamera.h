/*
 * glcamera.h
 *
 *  Created on: Mar 6, 2010
 *      Author: pushkar
 */

#ifndef GLCAMERA_H_
#define GLCAMERA_H_

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

class glcamera {
	Vec3 vp;	// View position
	Vec3 vd;	// View direction vector
	Vec3 vu;	// View up direction
	Vec3 pr;	// Point to rotate about
	double focallength;	// Focal length in vd
	double aperture;	// Camera aperture
	double eyesep;		// Eye separation
	int screenheight, screenwidth;

public:
	glcamera() {
		aperture = 60;
		pr.set(0.0, 0.0, 0.0);
		vd.set(1, 0, 0);
		vu.set(0, 1, 0);
		vp.set(-10, 0, 0);
		focallength = 0.0f;
		eyesep = focallength/30.0;
	}

};

#endif /* GLCAMERA_H_ */
