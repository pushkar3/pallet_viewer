/*
 * glcamera.h
 *
 *  Created on: Mar 6, 2010
 *      Author: pushkar
 *      Brief: FPS camera for opengl
 */

#ifndef GLCAMERA_H_
#define GLCAMERA_H_

#include "glstdlib.h"

class glcamera {
	Vec3 foc;			// focal vector
	Vec3 side;			// side vector
	double trans;
	double rots;
public:
	Vec3 vpos;			// View position
	Vec3 vdir;			// View direction vector
	Vec3 vup;			// View up direction
	double focallength;	// Focal length in vd
	double aperture;	// Camera aperture
	double eyesep;		// Eye separation
	int screenheight, screenwidth;

public:
	void init();
	glcamera();
	~glcamera() {};

	Vec3 get_focalvector();
	Vec3 get_sidevector();
	void set_focallength(double f);
	void camera_set_scale(double _trans, double _rots);

	void key_up();
	void key_down();
	void key_left();
	void key_right();
	void mouse(int dx, int dy);

};

#endif /* GLCAMERA_H_ */
