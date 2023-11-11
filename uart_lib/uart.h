/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/

#ifndef UART_H
#define UART_H


#include "pico/stdlib.h"
#include "hardware/uart.h"

void setup_uart0(uart_inst_t*);
void free_uart0(uart_inst_t*);
void setup_uart1(uart_inst_t*);
void free_uart1(uart_inst_t*);
void printf_uart(uart_inst_t*, const char*, ...);


// UART_H //
#endif