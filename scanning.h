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


typedef struct {
    float xPos;
    float yPos;
    float linear_width;
} Object;

void guidance_print(int moved_dist, int moved_angle);
void do_scan(void);
void scanning_plot_data();
void compare_size(ActionStack* stack, float* target_dist, int* target_angle);
void clean_data(int size);
void do_ir_dist(void);


void scanning_raw_scan();
void scanning_parse_objects();
void scanning_clean_scan();
