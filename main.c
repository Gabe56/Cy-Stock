#include "scanning.h"
#include "lcd.h"
#include "uart.h"
#include "ir.h"
#include "servo.h"
#include "ping.h"

int main() {
    lcd_init();
    init_uart(115200);
    ir_init();
    ping_init();
    servo_init();
    while(1) {
        char byte = uart_receive();
        if(byte == 'h') {
            scanning_raw_scan();
            scanning_plot_data();
            scanning_parse_objects();
        }
    }
}
