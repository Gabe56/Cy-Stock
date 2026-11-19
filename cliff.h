#include "open_interface.h"

typedef enum {
    WHITE,
    BLACK,
    YELLOW,
    OTHER
} cliff_color;

typedef struct {
    cliff_color front_right;
    cliff_color front_left;
    cliff_color right;
    cliff_color left;
} cliff_values;

void cliff_calibrate(oi_t* interface);
void cliff_update(oi_t* interface);
void cliff_init(oi_t* interface);
cliff_values cliff_status();
cliff_color get_color(int value);

