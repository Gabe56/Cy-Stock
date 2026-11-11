/*
 * uart.h
 *
 *  Created on: Oct 7, 2026
 *      Author: gapelaez
 */

#ifndef UART_H_
#define UART_H_



#endif /* UART_H_ */

void init_uart(int baud);
void uart_send(char data);
char* uart_receive_str_blocking();
char* uart_receive_str();
char uart_receive_blocking();
char uart_receive();
void uart_send_str(char* str);
void uart_clear_buf();
void uart_end();
