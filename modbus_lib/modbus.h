/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/


#ifndef MODBUS_H
#define MODBUS_H

#include "pico/stdlib.h"
#include "pico/printf.h"


///////////////////////////////////
//      Modbus Configuration
///////////////////////////////////

// The address of this device on the bus
#define MB_ADDR                 0x04

//#define MB_COILS_ADDR           0x01

// Reads the ON/OFF status of discrete coils in the slave.
//#define MB_FUNC_READ_COILS      0x01

#define MB_MAX_BUFFER           128
#define MB_REG_SIZE             8

typedef enum MB_FC {
    MB_FUNC_READ_COILS                  = 1,	// FCT=1 -> read coils or digital outputs
    MB_FUNC_READ_DISCRETE_INPUT         = 2,	// FCT=2 -> read digital inputs
    MB_FUNC_READ_REGISTERS              = 3,	// FCT=3 -> read registers or analog outputs
    MB_FUNC_READ_INPUT_REGISTER         = 4,	// FCT=4 -> read analog inputs
    MB_FUNC_WRITE_COIL                  = 5,	// FCT=5 -> write single coil or output
    MB_FUNC_WRITE_REGISTER              = 6,   // FCT=6 -> write single register
    MB_FUNC_WRITE_MULTIPLE_COILS        = 15,  // FCT=15 -> write multiple coils or outputs
    MB_FUNC_WRITE_MULTIPLE_REGISTERS    = 16	// FCT=16 -> write multiple registers
}mb_functioncode_t;

typedef enum MB_MSG {
    ID                                  = 0, // ID field
    FUNC                                = 1, // Function code position
    ADD_HI                              = 2, // Address high byte
    ADD_LO                              = 3, // Address low byte
    NB_HI                               = 4, // Number of coils or registers high byte
    NB_LO                               = 5, // Number of coils or registers low byte
    BYTE_CNT                            = 6  // byte counter
}mb_message_t;

typedef enum MB_ERR
{
    ERR_EXECUTE                         =  4,
    ERR_REGS_QUANT                      =  3,
    ERR_ADDR_RANGE                      =  2,
    ERR_FUNC_CODE                       =  1,
    ERR_NO_ERR                          =  0,
    ERR_NOT_MASTER                      = -1,
    ERR_POLLING                         = -2,
    ERR_BUFF_OVERFLOW                   = -3,
    ERR_BAD_LRC                         = -4,
    ERR_EXCEPTION                       = -5,
    ERR_BAD_SIZE                        = -6,
    ERR_BAD_ADDRESS                     = -7,
    ERR_TIME_OUT		                = -8,
    ERR_BAD_SLAVE_ID		            = -9

}mb_error_t;

typedef struct {
	uart_inst_t*    uart;                       // Serial port to communicate through
	uint8_t         id;                         // 0=master, 1..247=slave number
	mb_error_t      error;         // Last recieved error
	uint16_t        buffer[MB_MAX_BUFFER];      // Message buffer
	uint8_t         bufferSize;             // Size of the message buffer
    uint16_t*       regs;                       // Pointer to the external register array
    uint16_t        regSize;                    // Size of the regs array
}mb_handler_t;


void start_modbus(mb_handler_t*);
mb_error_t validateMessage(mb_handler_t*);
void mb_FUNC1(mb_handler_t*);
void mb_FUNC3(mb_handler_t*);
void mb_FUNC5(mb_handler_t*);
void mb_FUNC6(mb_handler_t*);
void mb_FUNC15(mb_handler_t*);
void mb_FUNC16(mb_handler_t*);
void respond_modbus(mb_handler_t*);
void buildError(mb_handler_t*, mb_error_t*);
uint8_t calcLRC(mb_handler_t*);
void ASCIItoHEX(mb_handler_t*);
uint8_t atoh(uint8_t);
void HEXtoASCII(mb_handler_t*);
uint16_t htoa(uint8_t);


// MODBUS _H //
#endif