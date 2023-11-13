/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/

#ifndef UART_H
#define UART_H


#include "pico/stdlib.h"
#include "hardware/uart.h"

#define MAX_BUFFER 128

void setup_uart(uart_inst_t*);
void free_uart(uart_inst_t*);
void printf_uart(uart_inst_t*, const char*, ...);

void read_uart(uart_inst_t*, char*, uint8_t);

// UART_H //
#endif