/*
 * uart.c
 *
 *  Created on: Oct 7, 2026
 *      Author: gapelaez
 */

#include "uart.h"
#include "Timer.h"
#include <stdlib.h>

char blocking_str_buf[21];
int blocking_str_p = 0;

char data_buf[21];
int data_p = 0;

char str_buf[21];
int str_p = 0;


void uart_int() {
    //clear interrupt, then add byte to stack
    UART1_ICR_R |= 0x00000010;
    data_p++;
    data_buf[data_p] = (char)UART1_DR_R;
}

void init_uart(int baud) {
    // sets up uart on portb
    SYSCTL_RCGCUART_R |= 0x00000002;

    SYSCTL_RCGCGPIO_R |= 0x00000002;
    timer_waitMillis(1);            // Small delay before accessing device after turning on clock
    GPIO_PORTB_AFSEL_R |= 0x00000003;
    GPIO_PORTB_PCTL_R &=  0xFFFFFF11;     // Force 0's in the disired locations
    GPIO_PORTB_PCTL_R |=  0x00000011;     // Force 1's in the disired locations
    GPIO_PORTB_DEN_R |=   0x00000003;

    GPIO_PORTB_DIR_R &= 0xFFFFFFF2;      // Force 0's in the disired locations
    GPIO_PORTB_DIR_R |= 0x00000002;      // Force 1's in the disired locataions



    //compute baud values [UART clock= 16 MHz]
    double fbrd;
    int    ibrd;

    fbrd =  16000000 / (16.0f * baud); // page 903
    ibrd = (int)fbrd;
    fbrd = (fbrd - ibrd) * 64 + 0.5;

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R |= ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD 0b0111 0b0000
    UART1_LCRH_R |= 0xFFFFFF60;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R  &= 0xFFFFFFF0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x00000001;        // enable UART1

    // setup interrupt
//    UART1_IFLS_R &= 0xFFFFFFC7;
    UART1_IM_R |= 0x00000010;
    NVIC_EN0_R |= 0x00000040;
    IntRegister(INT_UART1, uart_int);
}

void uart_clear_buf() {
    str_p = 0;
    memset(str_buf, NULL, sizeof(str_buf)); //clear str_buf
}
void uart_send(char data) {
    while(UART1_FR_R & 0x00000020) {} // wait until transmit fifo has room
    UART1_DR_R = (UART1_DR_R & 0xFFFF000) | data;
}

char uart_receive_blocking() {
    while(UART1_FR_R & 0x00000010) {} //wait until data is available
    char received_data = (char)UART1_DR_R;
    return received_data;
}

char uart_receive() {
    if(data_p < 0) return NULL;
// if we have data, then pop off stack and return
    char data = data_buf[data_p];
    data_p--;
    uart_send(data);
    return data;
}
char uart_receive_no_echo() {
    //same as receive but does not echo data back
    if(data_p < 0) return NULL;

    char data = data_buf[data_p];
    data_p--;
    return data;
}

char* uart_receive_str_blocking() {
    //unused, but blocks receive str
    blocking_str_p = 0;
    memset(blocking_str_buf, NULL, sizeof(blocking_str_buf));

    char data = uart_receive_blocking();

    while(data != '\r' && blocking_str_p < 19) {
        blocking_str_buf[blocking_str_p] = data;
        blocking_str_p++;
        data = uart_receive_blocking();
    }
    blocking_str_buf[blocking_str_p] = '\0';
    return blocking_str_buf;
}

char* uart_receive_str() {
    char data = uart_receive_no_echo(); // get data
    if(data == '\n' || str_p > 19) { // find when str is completed
        str_buf[str_p] = '\0'; // needs null term
        return str_buf;
    } else if(data) { //add data to str_buf if it exists
        str_buf[str_p] = data;
        str_p++;
    }
    return NULL; //otherwise str is not done
}
//char* uart_receive_str() {
//    char data = uart_receive(); // get data
//    if(data == '\n' || str_p > 19) { // find when str is completed
//        str_buf[str_p] = '\0'; // needs null term
//        return str_buf;
//    } else if(data) { //add data to str_buf if it exists
//        str_buf[str_p] = data;
//        str_p++;
//    }
//    return NULL; //otherwise str is not done
//}
void uart_send_str(char* str) {
    int i = 0;
    //loop through char in str and send them 1 by 1
    while(i < strlen(str)) {
        uart_send(str[i]);
        i++;
    }
    // send newline char to complete
    uart_send('\n');
//    uart_send('\0');
}

//void uart_send_str(char* str) {
//    int i = 0;
//    //loop through char in str and send them 1 by 1
//    while(i < strlen(str)) {
//        uart_send(str[i]);
//        i++;
//    }
//    // send newline char to complete
//    uart_send('\n');
//    uart_send('\r');
//    uart_send('\0');
//}

void uart_end() {
    // send end char
    uart_send_str("END");
}

