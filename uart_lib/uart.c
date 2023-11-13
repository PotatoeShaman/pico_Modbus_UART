/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/


#include <stdarg.h>
#include "uart.h"
#include "pico/printf.h"


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

// Blocking serial print to uart
void printf_uart(uart_inst_t* uart, const char* string, ...){
    char buff[MAX_BUFFER];
    uint8_t len = 0;

    va_list args;
    va_start(args, string);
    len = vsprintf(buff, string, args);

    #ifdef DEBUG_MODE
        printf(buff);
    #else
        uart_write_blocking(uart, buff, len);
    #endif
    va_end(args);
}

// Blocking serial read from uart
void read_uart(uart_inst_t* uart, char* buffer, uint8_t len){
    
    #ifdef DEBUG_MODE
        fgets(buffer, (len > MAX_BUFFER ? MAX_BUFFER : len), stdin);
    #else
        uart_read_blocking(uart, buffer, (len > MAX_BUFFER ? MAX_BUFFER : len));
    #endif
}