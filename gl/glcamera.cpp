/*
 * glcamera.cpp
 *
 *  Created on: Mar 6, 2010
 *      Author: pushkar
 */

#include "glstdlib.h"
#include "glcamera.h"
#include <stdlib.h>

Vec3 glcamera::get_focalvector() {
	vdir.normalize();
	foc.x = vpos.x + focallength*vdir.x;
	foc.y = vpos.y + focallength*vdir.y;
	foc.z = vpos.z + focallength*vdir.z;
	return foc;
}

Vec3 glcamera::get_sidevector() {
	Vec3 _side = vdir.cross(vup);
	_side.normalize();
	_side.scale(eyesep/2.0);
	side.set(_side.x, _side.y, _side.z);
	return side;
}

void glcamera::init() {
	aperture = 60;
	vpos.set_unit_x();
	vpos.z = -1.0f;
	vpos.scale(-500);
	vdir.set_unit_x();
	vup.set_unit_z();
	foc = get_focalvector();
	focallength = 10.0f;
	eyesep = focallength/30.0;
	trans = 10.0f;
	rots = 0.008;
}

glcamera::glcamera() {
	init();
}

void glcamera::set_focallength(double f) {
	focallength = f;
	eyesep = focallength/4.0;
}

void glcamera::camera_set_scale(double _trans, double _rots) {
	trans = _trans;
	rots = _rots;
}

void glcamera::key_up() {
	Vec3 _vdir;
	_vdir.set(vdir);
	_vdir.scale(trans);
	vpos.add(_vdir);
}

void glcamera::key_down() {
	Vec3 _vdir;
	_vdir.set(vdir);
	_vdir.scale(-trans);
	vpos.add(_vdir);
}

void glcamera::key_left() {
	Vec3 _side;
	_side = get_sidevector();
	_side.normalize();
	_side.scale(-trans);
	vpos.add(_side);
}

void glcamera::key_right() {
	Vec3 _side;
	_side = get_sidevector();
	_side.normalize();
	_side.scale(trans);
	vpos.add(_side);
}


void glcamera::mouse(int dx, int dy) {

	double _dx = 0.0f, _dy = 0.0f;
	if(dx != 0) _dx = dx/abs(dx);
	if(dy != 0) _dy = dy/abs(dy);
	vdir.rot(_dx*rots, _dy*rots);
}
