/*
 * scanning.h
 *
 *  Created on: Oct 9, 2026
 *      Author: gapelaez
 */

#ifndef SCANNING_H_
#define SCANNING_H_
#include "movement.h"


#endif /* SCANNING_H_ */


void guidance_print(int moved_dist, int moved_angle);
void do_scan(void);
void plot_data(int size);
void compare_size(ActionStack* stack, float* target_dist, int* target_angle);
void clean_data(int size);
void do_ir_dist(void);
