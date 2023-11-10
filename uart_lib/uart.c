/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/


#include <stdarg.h>
#include "uart.h"

#include "../uart_modbus_constants.h"

#ifndef UART0_OVERRIDE
    #define UART0_BAUD          115200
    #define UART0_TX_PIN        0
    #define UART0_RX_PIN        1
    #define UART0_DATA_BITS     8
    #define UART0_STOP_BITS     1
    #define UART0_PARITY        0
#endif

#ifndef UART1_OVERRIDE
    #define UART1_BAUD          115200
    #define UART1_TX_PIN        4
    #define UART1_RX_PIN        5
    #define UART1_DATA_BITS     8
    #define UART1_STOP_BITS     1
    #define UART1_PARITY        0
#endif

void setup_uart0(uart_inst_t*);
void free_uart0(uart_inst_t*);
void setup_uart1(uart_inst_t*);
void free_uart1(uart_inst_t*);
void printf_uart(uart_inst_t*, const char*, ...);

void setup_uart(uart_inst_t* uart){
    uint chan = uart_get_index(uart);
    uart_init(uart, (chan ? UART0_BAUD : UART1_BAUD));
    gpio_set_function((chan ? UART0_TX_PIN : UART1_TX_PIN), GPIO_FUNC_UART);
    gpio_set_function((chan ? UART0_RX_PIN: UART1_RX_PIN), GPIO_FUNC_UART);
    uart_set_format(uart, (chan ? UART0_DATA_BITS : UART1_DATA_BITS), (chan ? UART0_STOP_BITS : UART1_STOP_BITS), (chan ? UART0_PARITY : UART1_PARITY));
}

void free_uart(uart_inst_t* uart){
    uint chan = uart_get_index(uart);
    uart_deinit(uart);
    gpio_set_function((chan ? UART0_TX_PIN : UART1_TX_PIN), GPIO_FUNC_NULL);
    gpio_set_function((chan ? UART0_RX_PIN : UART1_RX_PIN), GPIO_FUNC_NULL);
}

void printf_uart(uart_inst_t* uart, const char* string, ...){
    char buff[512];
    va_list args;
    va_start(args, string);
    vsprintf(buff, string, args);
    uart_puts(uart, buff);
    va_end(args);

    #ifdef DEBUG_MODE
        printf(buff);
    #endif
}

#include "pico/stdio_usb.h"

