/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/


#include "pico/printf.h"
#include "pico/stdlib.h"

#include "modbus_lib/modbus.h"
#include "uart_lib/uart.h"

uint16_t mb_regs[MB_REG_SIZE];
mb_handler_t mbHandle = {
    .uart       = uart0,
    .id         = 0x04,
    .error      = ERR_NO_ERR,
    .bufferSize = 0,
    .regs       = mb_regs,
    .regSize    = MB_REG_SIZE
};

void main(){
    stdio_init_all();
    setup_uart(uart0);

    start_modbus(&mbHandle);

    char buf[MB_MAX_BUFFER];

    while(1){
        busy_wait_ms(1000);
        //printf(":%02X%02X%02X%02X%02X%02X\r\n", MB_ADDR, 0x01, 0x02, 0x0A, 0x11, 0xDE);
        fgets(&buf[0], MB_MAX_BUFFER, stdin);
        printf_uart(uart0, ":0401020A11DE\r\n");
        //printf("%c%c%c%c%c%c%c", 0x00, 0x01, 0x02, 0x0A, 0x11, 0xB3, 0x50);
    }
}