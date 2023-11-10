/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023

This file is the collection of common constants,
such as pin assignments, address spaces, debugging, and system states
*/


#ifndef UART_MODBUS_CONSTANTS_H
#define UART_MODBUS_CONSTANTS_H


///////////////////////////////////
//      USB Serial Debugging
///////////////////////////////////
#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEBUG_PRINTF(x,y) printf(x,y)
#else
    #define DEBUG_PRINTF(x) do { } while(0)
#endif

// UART_MODBUS_CONSTANTS_H //
#endif