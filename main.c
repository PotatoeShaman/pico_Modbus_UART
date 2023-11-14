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
    busy_wait_ms(3000);

    start_modbus(&mbHandle);

    //uint8_t buf[MB_MAX_BUFFER];
    //uint16_t* pbuf = mbHandle.buffer;
    //uint8_t* buf = (uint8_t*)pbuf;

    while(1){
        tight_loop_contents();
        //fgets(buf, 3, stdin);
    }
}