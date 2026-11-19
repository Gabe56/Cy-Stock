#include "open_interface.h"
#include "cliff.h"
#include "lcd.h"

int cliff_fr;
int cliff_r;
int cliff_fl;
int cliff_l;

//BOT 20 & 24 (Value different depending on BOT)
#define YELLOW_THRESHOLD 2800
#define WHITE_THRESHOLD 2700
#define BLACK_THRESHOLD 100

//Update cliff sensor values
void cliff_update(oi_t* interface) {
    oi_update(interface);
    cliff_fr = interface->cliffFrontRightSignal;
    cliff_r = interface->cliffRightSignal;
    cliff_fl = interface->cliffFrontLeftSignal;
    cliff_l = interface->cliffLeftSignal;
}

//Initialize cliff_sensor
void cliff_init(oi_t* interface) {
    cliff_fr = 0;
    cliff_r = 0;
    cliff_fl = 0;
    cliff_l = 0;
    oi_init(interface);
}

//Used to calibrate values for cliff sensor
void cliff_calibrate(oi_t* interface) {
    lcd_init();
    while(1) {
        oi_update(interface);
        cliff_update(interface);
        lcd_printf("fr: %d, fl %d, r: %d, l: %d", cliff_fr, cliff_fl, cliff_r, cliff_l);
    }
}

// 0 = WHITE, 1 = BLACK, 2 = YELLOW, 3 = OTHER
cliff_color get_color(int value) {
    if(value > YELLOW_THRESHOLD) {
        return YELLOW;
    }
    else if(value > WHITE_THRESHOLD) {
        return WHITE;
    }
    else if(value < BLACK_THRESHOLD) {
           return BLACK;
    }

    return OTHER;
}

// RETURNS color detected
cliff_values cliff_status() {
    cliff_values status;
    status.front_right = get_color(cliff_fr);
    status.right = get_color(cliff_r);
    status.front_left = get_color(cliff_fl);
    status.left = get_color(cliff_l);

    return status;

}



