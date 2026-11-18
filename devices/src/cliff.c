#include "../include/cliff.h"
#include "../include/lcd.h"

int cliff_f_r;
int cliff_r;
int cliff_f_l;
int cliff_l;

oi_t* interface;

#define CLIFF_WHITE_THRESHOLD 2800 
#define CLIFF_BLACK_THRESHOLD 100


void cliff_update() {
    cliff_f_r = interface->cliffFrontRightSignal;
    cliff_r = interface->cliffRightSignal;
    cliff_f_l = interface->cliffFrontLeftSignal;
    cliff_l = interface->cliffLeftSignal;
}

void cliff_init(oi_t* intf) {
    interface = intf;
    cliff_f_r = 0;
    cliff_r = 0;
    cliff_f_l = 0;
    cliff_l = 0;
}
void cliff_calibrate_raw() {
    oi_t intf = {};
    oi_init(&intf);
    cliff_init(&intf);
    lcd_init();

    while(1) {
        oi_update();
        cliff_update();
        lcd_printf("fr: %d, fl %d, r: %d, l: %d", cliff_f_r, cliff_f_l, cliff_r, cliff_l);
    }
}

char* cliff_color_to_string(cliff_color color) {
    switch(color) {
        case WHITE:
            return "WHITE";
        case BLACK:
            return "BLACK";
        case OTHER:
            return "OTHER";
        default:
            return "INVALID";
    }
}
void cliff_calibrate_color() {
    oi_t intf = {};
    oi_init(&intf);
    cliff_init(&intf);
    lcd_init();

    while(1) {
        oi_update();
        cliff_values values = cliff_status();

        lcd_printf("fr: %s, fl %s, r: %s, l: %s", cliff_color_to_string(values.front_right), cliff_color_to_string(values.front_left), cliff_color_to_string(values.right), cliff_color_to_string(values.left));
    }
}
cliff_color get_color(int value) {
    if(value < CLIFF_BLACK_THRESHOLD) {
        return BLACK;
    }
    if(value > CLIFF_WHITE_THRESHOLD) {
        return WHITE;
    }

    return OTHER;
}
cliff_values cliff_status() {
    cliff_update();
    cliff_values values;
    values.front_right = get_color(cliff_f_r);
    values.front_left = get_color(cliff_f_l);
    values.right = get_color(cliff_r);
    values.left = get_color(cliff_l);

    return values;
}



