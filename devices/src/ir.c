/*
 * adc.c
 *
 *  Created on: Oct 21, 2026
 *      Author: gapelaez
 */
#include "../include/ir.h"
#include "../include/Timer.h"
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "../include/uart.h"
#include "../include/lcd.h"
#include "../include/button.h"
#include <math.h>
#include "../include/servo.h"

#define A 124.99317
#define B -0.00182458
#define C 7.80083
#define AVERAGE_TIMES 10
#define MEDIAN_SIZE 25

extern volatile int button_event;
extern volatile int button_num;

char str[50];

//#define data_size 8
//int data[data_size];

// selection sort implementation, o(n^2) is fine for small values
void sort_arr(float arr[], int length) {
    int i;
    for(i = 0; i < length; i++) {
        int j;
        int smallest = i;
        for(j = i + 1; j < length; j++) {
            if(arr[j] < arr[smallest]) smallest = j;
        }
        float temp = arr[i];
        arr[i] = arr[smallest];
        arr[smallest] = temp;
    }
}
// apply exponential regression to quantized values
float fix(int input) {
    return A * exp(B * (float)input) + C;
}
float perform_dist_measure(int angle) {
    // goal is to find median to reduce anamalies
    servo_move(angle);
    float values[MEDIAN_SIZE];
    int median = MEDIAN_SIZE / 2;
    int i = 0;
    while(i < MEDIAN_SIZE) {
        values[i] = fix(adc_ir_read()); // read multiple scans
        i++;
    }
    sort_arr(values, sizeof(values) / sizeof(float)); // sort the array
    return values[median]; // return the middle index (median)
}

// calibrate ir (pt2)
void calibrate_ir() {
    button_init();
    init_button_interrupts(); //using buttons
    while(1) {
        if(button_event) {
            button_event = 0;
            if(button_num == 1) {
                while(button_event == 0) { //when holding button 1
                    int measured_val = adc_ir_read(); // find quantized value
                    float real_val = perform_dist_measure(90); //do scan with median
                    sprintf(str, "raw: %d, real: %.1f", measured_val, real_val);
                    uart_send_str(str);
                    lcd_printf(str); // send data to putty and display
                }
            }
        }
    }
}

void adc_ir_init() {
    // init ir
    SYSCTL_RCGCGPIO_R |= 0x00000002;
    SYSCTL_RCGCADC_R |= 0x00000001; //give clock
    timer_waitMillis(1);
    GPIO_PORTB_AFSEL_R |= 0x10; //alt function enable to ADC0
    GPIO_PORTB_DEN_R &= 0xEF; // disable digital to pin 4
    GPIO_PORTB_AMSEL_R |= 0x10; // enable analog mode for pin 4

    ADC0_ACTSS_R &= 0xE; //turn off ss0 to configure
    ADC0_SSMUX0_R |= 0xAAAAAAAA; // ain10 for all muxes
    ADC0_SSCTL0_R = 0x6; // interrupt at 1 sample scanned and 1 sample is end of sequence
    ADC0_SAC_R |= 0x6; // 64x sampling
    ADC0_ACTSS_R |= 0x1; //reenable
}
int adc_ir_read() {
    ADC0_PSSI_R |= 0x1; //begin sampling
    while(~ADC0_RIS_R & 0x1) {}
    ADC0_ISC_R = 0x1; //wait until RIS has an interrupt condition and clear it
    int data= ADC0_SSFIFO0_R;
    return data; //return the quantized data
}
