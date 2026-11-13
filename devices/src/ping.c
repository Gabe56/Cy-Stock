/*
 * ping.c
 *
 *  Created on: Oct 28, 2026
 *      Author: gapelaez
 */

#include "../include/Timer.h"
#include <inc/tm4c123gh6pm.h>
#include "../include/ping.h"
#define WAIT_TIME 8.0
#define cyc_per_sec 16000000.0

volatile unsigned int after_rising = 0;
volatile int new_ping = 0;
volatile int ping_length = 0;
volatile unsigned int start_ping_time = 0;
volatile unsigned int end_ping_time = 0;

float distance_measured = 0;


void ping_int() {
    TIMER3_ICR_R |= 0x400; //enable interrupts to be sent
    if(!after_rising) { //to tell if the ping has been sent yet
        start_ping_time = ((TIMER3_TBR_R) | (TIMER3_TBPS_R << 16)); //shift as 24bit needed
        after_rising = 1;
    } else {
        end_ping_time = ((TIMER3_TBR_R) | (TIMER3_TBPS_R << 16));
        after_rising = 0;
        ping_length = start_ping_time - end_ping_time; //find length of pulse in clock cycles
        TIMER3_IMR_R &= 0xBFF; //disable interrupts being sent for now as we are done
        new_ping = 1; // signal that new data is here
    }
}
// init ping sensor
void ping_init_q() {
    SYSCTL_RCGCGPIO_R |= 0x2; //portb clock
    SYSCTL_RCGCTIMER_R |= 0x8; // timer 3 clock enable
    timer_waitMillis(1);

    GPIO_PORTB_DEN_R |= 0x8; //enable pin3
    GPIO_PORTB_DIR_R |= 0x8; //out dir
    GPIO_PORTB_DATA_R &= 0x7;
    GPIO_PORTB_AFSEL_R |= 0x8; //alternate functions
    GPIO_PORTB_PCTL_R |= 0x7000; // cc for pin 3

    TIMER3_CTL_R &= 0xEFF; //disable 3b
    TIMER3_CFG_R |= 0x4; //split timers
    TIMER3_TBMR_R |= 0x07; //edge time with capture, count down
    TIMER3_CTL_R |= 0xC00; //capture both edges
    TIMER3_TBILR_R = 0xFFFF; // set starting point
    TIMER3_TBPR_R |= 0xFF; // set pre-scalar
    TIMER3_CTL_R |= 0x100; //renable 3b
    NVIC_EN1_R |= 0x10; //enable in nvic for timer3b
    IntRegister(INT_TIMER3B, ping_int); //enable in nvic
}

float calc_distance(int cycles) {
    float millis = ((float)cycles / cyc_per_sec) * 1000.0; //convert to millis from cc

    float dist = (millis * 340.0) / 20.0; //only divide by 20 as we are in milliseconds and we need to get to m
    return dist;
}

void send_pulse() {
//    TIMER3_TBILR_R = 0xFFFF; // set starting point
//    TIMER3_TBPR_R |= 0xFF; // set pre-scalar
    GPIO_PORTB_DIR_R |= 0x8; //out dirction
    GPIO_PORTB_DATA_R |= 0x8; //hold pin 3 high for x micros
    timer_waitMicros(WAIT_TIME);
    GPIO_PORTB_DATA_R &= 0x7;
    GPIO_PORTB_DIR_R &= 0x7; // back to input
}

int ping_send(int* cycles, float* dist) {
    GPIO_PORTB_AFSEL_R &= 0x7; //disable afsel
    TIMER3_IMR_R &= 0xBFF;  //disable interrupts
    send_pulse();

    //reconfig for recieve
    GPIO_PORTB_AFSEL_R |= 0x8; //enable afsel
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;  //enable interrupts

    while(!new_ping) {}
    new_ping = 0;
    float distance = calc_distance(ping_length); //convert to cm
    *cycles = ping_length;
    *dist = distance;

    if(ping_length < 0) return 1;
    return 0;
}

