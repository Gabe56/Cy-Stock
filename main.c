#include "cliff.h"
#include "lcd.h"
#include "timer.h"


int main() {
    oi_t* interface = oi_alloc();
    oi_init(interface);
    cliff_init(interface);
    lcd_init();

    //cliff_calibrate(interface);

     while (1){
         cliff_update(interface);
         cliff_values status = cliff_status();
         lcd_printf("fr: %d, fl %d, r: %d, l: %d", status.front_right, status.front_left, status.right, status.left);
     }
    return 0;
}
