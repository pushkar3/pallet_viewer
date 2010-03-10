/*
 * main.h
 *
 *  Created on: Mar 10, 2010
 *      Author: pushkar
 *      Brief: User defined functions for a viewer
 */

#ifndef MAIN_H_
#define MAIN_H_

void draw_wired_cube(int x, int y, int z, int width, int length, int height);
void pallet_dump_vector();
void pallet_vector_key_control(int key);
void pallet_draw_vector();

void _keyboard(unsigned char key);
void _draw();

#endif /* MAIN_H_ */
