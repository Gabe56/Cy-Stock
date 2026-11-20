/*
 * ir.c
 *
 *  Created on: Oct 21, 2026
 *      Author: gapelaez
 */
#include "ir.h"
#include "timer.h"
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "lcd.h"
#include "button.h"
#include "math.h"

#define A 124.99317
#define B -0.00182458
#define C 7.80083
#define IR_SAMPLE_TIMES 8

extern volatile int button_event;
extern volatile int button_num;

char str[50];

// insertion sort implementation
void ir_sort(int data[], int data_size) {
    int i;
    int length = data_size / sizeof(int);
    for(i = 0; i < length; i++) {
        int curr_elem = data[i];
        int k;
        for(k = i - 1; k >= 0 && data[k] > curr_elem; k--) {
            data[k + 1] = data[k];
        }
        data[k + 1] = curr_elem;
    }
}
// calibrate ir (pt2)
void ir_calibrate() {
    button_init();
    init_button_interrupts(); //using buttons
    while(1) {
        if(button_event) {
            button_event = 0;
            if(button_num == 1) {
                while(button_event == 0) { //when holding button 1
                    int measured_val = ir_raw(); // find quantized value
                    float distance = ir_read(); // convert to real distance
                    sprintf(str, "raw: %d, distance: %.1f", measured_val, distance);
                    lcd_printf(str); // send data to putty and display
                }
            }
        }
    }
}

void ir_init() {
    // init ir
    SYSCTL_RCGCGPIO_R |= 0x00000002;
    SYSCTL_RCGCADC_R |= 0x00000001; //give clock
    timer_waitMillis(1);
    GPIO_PORTB_AFSEL_R |= 0x10; //alt function enable to ADC0
    GPIO_PORTB_DEN_R &= 0xEF; // disable digital to pin 4
    GPIO_PORTB_AMSEL_R |= 0x10; // enable analog mode for pin 4

    ADC0_ACTSS_R &= 0xE; //turn off ss0 to configure
    ADC0_SSMUX0_R |= 0xAAAAAAAA; // ain10 for all muxes
    ADC0_SSCTL0_R = 0x60000000; // interrupt at 8 samples scanned and 8 samples is end of sequence
    ADC0_SAC_R |= 0x6; // 64x sampling
    ADC0_ACTSS_R |= 0x1; //reenable
}

int ir_raw() {
    ADC0_PSSI_R |= 0x1; //begin sampling
    while(~ADC0_RIS_R & 0x1) {}
    ADC0_ISC_R = 0x1; //wait until RIS has an interrupt condition and clear it

    int data[IR_SAMPLE_TIMES];
    int i;
    for(i = 0; i < IR_SAMPLE_TIMES; i++) {
        data[i] = ADC0_SSFIFO0_R;
    }
    ir_sort(data, sizeof(data)); // sort the array
    int median_data = data[IR_SAMPLE_TIMES / 2]; // return the middle index (median)
    return median_data;
}
float ir_read() {
    int median_data = ir_raw(); // get the median value
    float distance = A * exp(B * (float)median_data) + C;
    return distance; //return the converted distance
}
