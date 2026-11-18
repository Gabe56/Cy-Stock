#include "open_interface.h"

typedef enum {
    WHITE,
    BLACK,
    OTHER
} cliff_color;

typedef struct {
    cliff_color front_right;
    cliff_color front_left;
    cliff_color right;
    cliff_color left;
} cliff_values;

void cliff_calibrate_raw();
void cliff_calibrate_color();
void cliff_update();
void cliff_init(oi_t* intf);
cliff_color get_color(int value);
char* cliff_color_to_string(cliff_color color);
cliff_values cliff_status();