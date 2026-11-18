void cliff_calibrate();
void cliff_update();
void cliff_init();


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