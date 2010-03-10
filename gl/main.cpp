/*
 * main.cpp
 *
 *  Created on: Mar 10, 2010
 *      Author: pushkar
 */

#include "view.h"
#include "main.h"

void _keyboard(unsigned char key) {
	switch (key) {
	case 'f':
		pallet_vector_key_control(-1);
		break;
	case 'g':
		pallet_vector_key_control(1);
		break;
	}
}

void _draw() {
	pallet_draw_vector();
}
