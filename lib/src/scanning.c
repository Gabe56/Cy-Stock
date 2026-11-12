/*
 * scanning.c
 *
 *  Created on: Oct 9, 2026
 *      Author: gapelaez
 */

#include "../../sensors/include/ir.h"
#include "../../devices/include/servo.h"
#include "../../sensors/include/ping.h"
#include "../../lib/include/movement.h"
#include "../../devices/include/lcd.h"
#include "../include/scanning.h"
#include <stdint.h>
#include <math.h>

#define threshold 20.0
#define throw_out_thresh 5
#define clean_thresh 0.0
#define average_times 3.0
#define outlier_removal 15.0
#define gradual_threshold 30.0

float scan_data_IR[91];
char str[50];
int do_IR = 0;


int smallest_width = 180;
float smallest_linear_width = 1800;
int target_angle = 90;
int obj_number = 0;
float target_distance = 0;
// Got rid of software distance averaging
 int object_detected(int index) {
     int angular_width = 0;
     int angle = index * 2; // starting point
    // float distance = scan_data[index + 1];

     int i = index;
     float local_min = scan_data_IR[i + 1];
     float local_max = scan_data_IR[i + 1];
//     while((!(data[i + 1] - data[index]) > 50.0 || (data[i + 1] - data[index]) < -50.0) && i <= 90) {
     while(scan_data_IR[i+1] - scan_data_IR[i] < threshold && scan_data_IR[i+1] - local_min < gradual_threshold && scan_data_IR[i + 1] - local_max < gradual_threshold && i <= 90) { //rm thresh / 2
         if(scan_data_IR[i+1] < local_min){
            local_min = scan_data_IR[i+1];
         }
         if(scan_data_IR[i+1] > local_max){
             local_max = scan_data_IR[i+1];
         }
        angular_width += 2;
         i++;
     }
    int middle_angle = angle + angular_width / 2;

    // dist scan
    servo_move(middle_angle);
    int cycles;
    float distance;
    if(ping_send(&cycles, &distance) != 0) {
        return i + 1;
    }
    float radians = angular_width * (M_PI / 180.0);
    float linear_width = sqrt(2 * pow(distance, 2) * (1 - cos(radians)));

     if((linear_width < 3.0 && angular_width < 4) || distance > 160 || middle_angle < 15 || middle_angle > 160 || linear_width <= 0.0) {
//         sprintf(str, "thrown: %d\t%.1f\t%d\t%.1f\n\r", middle_angle, linear_width, angular_width, distance);
//         uart_send_str(str);
         return i + 1;
     }
     if(angular_width < smallest_width) smallest_width = angular_width;
     //float linear_width = sqrt(2 * pow(distance, 2) * cos(radians));
     if(linear_width < smallest_linear_width) {
         smallest_linear_width = linear_width;
         target_angle = middle_angle;
         target_distance = distance;
     }

     obj_number++;
     lcd_printf("obj found %d", obj_number);
//     sprintf(str, "object %d, radial_width: %d, linear_width: %.1f, distance: %.1f, angle: %d\n\r", obj_number, angular_width, linear_width, distance, middle_angle);
//     uart_send_str(str);
     return i + 1;
 }
void clean_data(int size) {
    int i = 1;

    //remove outliers with mean of 3 points
    while(i < size) {
        if(fabs(scan_data_IR[i] - scan_data_IR[i+1]) > outlier_removal && fabs(scan_data_IR[i - 1] - scan_data_IR[i]) > outlier_removal) {
            scan_data_IR[i] = (scan_data_IR[i+1] + scan_data_IR[i - 1]) / 2.0;
        }
        i++;
    }
//    i = 1;
//    while(i < size) {
//        if(fabs(scan_data_ping[i] - scan_data_ping[i+1]) > 75 && fabs(scan_data_IR[i - 1] - scan_data_IR[i]) > 75) {
//            scan_data_IR[i] = (scan_data_IR[i+1] + scan_data_IR[i - 1]) / 2.0;
//        }
//        i++;
//    }

    i = 0;
    // small variations will be flattened
    while(i < size) {
        float dist_val = scan_data_IR[i];
        while(!(fabs(scan_data_IR[i + 1] - scan_data_IR[i]) > clean_thresh)) {
            scan_data_IR[i] = dist_val;
            i++;
        }
        if(fabs(scan_data_IR[i + 1] - scan_data_IR[i]) > clean_thresh) {
            i++;
            float dist_val = scan_data_IR[i];
            while(!(fabs(scan_data_IR[i + 1] - scan_data_IR[i]) > clean_thresh)) {
                scan_data_IR[i] = dist_val;
                i++;
            }
        } else {
                i++;
        }
    }
}
// compare sizes of objects
void compare_size(ActionStack* stack, float* target_dist, int* target_ang) {
    int i = 0;
    while(i < 90) {
        // detect edge
//        if(fabs(data[i + 1] - distance) > 50.0) {
        // find object based on large drops
        if((scan_data_IR[i + 1] - scan_data_IR[i]) < -threshold) {
             int new_i = object_detected(i);
             i = new_i;
        } else {
            i++;
        }
    }
//    sprintf(str, "smallest radial width: %d, smallest linear width: %.1f. moving to %d, distance: %.1f", smallest_width, smallest_linear_width, target_angle, target_distance);
    servo_move(target_angle);
//    uart_send_str(str);
    *target_dist = target_distance;
    *target_ang = target_angle;
}
// create scan from data array
void do_scan(void) {
    lcd_printf("scanning...");
    timer_waitMillis(500);

//    sprintf(str, "Angle(Degrees) Distance(m)\n\r");
//    uart_send_str(str);
    //reset values for compare
    smallest_width = 180;
    smallest_linear_width = 1800;
    obj_number = 0;
    target_angle = 0;
    target_distance = 0;
    int i = 0;

    while(i <= 180){
        float dist_ir = perform_dist_measure(i);
//        sprintf(str, "%d\t%.1f\r", i, dist_ir);
//        uart_send_str(str);
        scan_data_IR[i / 2] = dist_ir;
        i = i + 2;
    }
//    int j = 0;
//    while(j < sizeof(scan_data) / sizeof(float)) {
//        sprintf(str, "%d\t%.1f\n\r", j * 2, scan_data[j]);
//        uart_send_str(str);
//        j++;
//    }
}

void plot_data(int size) {
    int i = 0;
    while(i < size) {
        sprintf(str, "%d\t%.1f", i * 2, scan_data_IR[i]);
        uart_send_str(str);
        i ++;
    }
}

void guidance_print(int moved_dist, int moved_angle) {
    sprintf(str, "dist left = %.1f\n\r", moved_dist);
    uart_send_str(str);
    sprintf(str, "angle left = %d\t distance left = %.1f\n\r", target_angle - moved_angle, target_distance - moved_dist);
    uart_send_str(str);
}

void do_ir_dist() {
    do_IR = 1;
}
