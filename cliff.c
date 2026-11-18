#include "open_interface.h"
#include "cliff.h"
#include "lcd.h"

int cliff_f_r;
int cliff_r;
int cliff_f_l;
int cliff_l;

#define WHITE_THRESHOLD 2800 
#define BLACK_THRESHOLD 100


void cliff_update(oi_t* interface) {
    cliff_f_r = interface->cliffFrontRightSignal;
    cliff_r = interface->cliffRightSignal;
    cliff_f_l = interface->cliffFrontLeftSignal;
    cliff_l = interface->cliffLeftSignal;
}

void cliff_init() {
    cliff_f_r = 0;
    cliff_r = 0;
    cliff_f_l = 0;
    cliff_l = 0;
    oi_init(&interface);
}
void cliff_calibrate() {
    oi_t interface = {};
    lcd_init();
    while(1) {
        oi_update(&interface);
        cliff_update();
        lcd_printf("fr: %d, fl %d, r: %d, l: %d", cliff_f_r, cliff_f_l, cliff_r, cliff_l);
    }
}
cliff_color get_color(int value) {
    if(value < BLACK_THRESHOLD) {
        return BLACK;
    }
    if(value > WHITE_THRESHOLD) {
        return WHITE;
    }

    return OTHER;
}
cliff_values cliff_status() {
    
}



