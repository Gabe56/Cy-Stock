/*
 * servo.c
 *
 *  Created on: Nov 4, 2026
 *      Author: gapelaez
 */
#include "servo.h"
#include <inc/tm4c123gh6pm.h>
#include "timer.h"
#include "lcd.h"
#include "button.h"

// for bot 24 (cursed)
// 180 degrees: TBMATCHR = 73500
// 135 degrees: TBMATCHR = around 83000
// 90 degrees: TBMATCHR = 21500
// 45 degrees: TBMATCHR = 28000
// 0 degrees: TBMATCHR = 35000

//bot 24


//
//#define servo_starting_pos 21500 // 90 degrees
//#define servo_calibration 155.556
//
//int servo_180 = 7000;
//int servo_135 = 13500;
//int servo_90  = 21000;
//int servo_45 = 28500;
//int servo_0 = 35000;

// bot 3
#define servo_starting_pos 21016 // 90 degrees
//#define servo_calibration 152.97222222 // 35000 - 7465 / 180
#define servo_calibration 152.127777778 // 34848 - 7465 / 180


//int servo_180 = 7400;
//int servo_135 = 13500;
//int servo_90  = 21016;
//int servo_45 = 28500;
//int servo_0 = 34848;

int servo_180 = 34848;
int servo_135 = 27986;
int servo_90 = 21016;
int servo_45 = 14262;
int servo_0 = 7400;


//
//int servo_conversion  (int value){
//   if(value <= servo_45){
//       return (servo_0 - servo_45) / 45;
//   }
//   else if(value <= servo_90){
//       return (servo_45 - servo_90) / 45;
//   }
//   else if(value <= servo_135){
//       //temp fix
//       return (servo_90 - servo_135) / 45;
//   }
//   else if(value <= servo_180){
//       return (servo_135 - servo_180) / 45;
//   }
//   return 0;
//}


void servo_init() {
    SYSCTL_RCGCGPIO_R |= 0x2; //portb clock
    SYSCTL_RCGCTIMER_R |= 0x2; // timer 1 clock enable
    timer_waitMillis(1);

    GPIO_PORTB_DEN_R |= 0x20; //enable pin5
    GPIO_PORTB_DIR_R |= 0x20; //out dir
    GPIO_PORTB_AFSEL_R |= 0x20; //alternate functions
    GPIO_PORTB_PCTL_R |= 0x700000; // cc for pin 5

    TIMER1_CTL_R &= 0xEFF; //disable 1b
    TIMER1_CFG_R |= 0x4; //split timers
    TIMER1_TBMR_R |= 0xA; //pwm mode
    TIMER1_CTL_R |= 0x4000; // inversion
    TIMER1_TBPR_R = 0x02; // set pre-scalar
    TIMER1_TBILR_R = 0x7100; // set starting point, 10ms = 0.01s, 160,000 ticks
    TIMER1_TBMATCHR_R = servo_starting_pos; //high for 1ms = 0.001s
    TIMER1_CTL_R |= 0x100; //renable 1b
}

void servo_calibrate() {
    // get everything ready
    lcd_init();
    servo_init();
    button_init();
    int dir = 1;
    int servo_angle = 90;
    servo_move(servo_angle);


    while(1) {
        int button = button_getButton();
        if(button == 1) {
            servo_angle = servo_angle + 1 * dir; //move servo 1 degrees left or right depending by direction
        }
        if(button == 2) {
            servo_angle = servo_angle + 5 * dir;  //move servo 5 degrees left or right depending by direction
        }
        if(button == 3) {
            dir = dir * -1; //flip direction
        }
        if(button == 4) { //move to 0 if right, 180 if left
            if(dir < 0) {
                servo_angle = 0;
            } else {
                servo_angle = 180;
            }
        }
        //cap values for print statement
        if (servo_angle >= 180){
            servo_angle = 180;
        }
        if(servo_angle <= 0){
            servo_angle = 0;
        }

        servo_move(servo_angle); //move servo and print
        lcd_printf("angle: %d, dir: %d,   raw: %d", servo_angle, dir, TIMER1_TBMATCHR_R);
        timer_waitMillis(250);
    }
}

void servo_move(int degrees) {
    // cap values
    if (degrees < 0){
        degrees = 0;
    }
    if (degrees > 180){
        degrees = 180;
    }

    //linear formula for servo value
    int move = servo_0 + ((servo_180 - servo_0) * degrees) / 180;

    // set pwm pulse width
    TIMER1_TBMATCHR_R &= 0x00000000;
    TIMER1_TBMATCHR_R = move;
    TIMER1_CTL_R |= 0x100;
    timer_waitMillis(250);
}


