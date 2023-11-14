/*
Alec Stobbs
Discord: @PotatoeComrade

11/09/2023


*/


#include "modbus.h"
#include "../uart_lib/uart.h"

// Starts a Modbus ACII slave
void start_modbus(mb_handler_t* mbHandle){

    setup_uart(mbHandle->uart);

    // continuously listen on uart port
    while(1){
        tight_loop_contents();
        // Not valid message start
        #ifdef DEBUG_MODE
            if(fgetc(stdin) != ':') continue;
            busy_wait_ms(5);
        #else
            while(!uart_is_readable);
            if(uart_getc(mbHandle->uart) != ':') continue;
        #endif

        for (uint8_t i = 0; i < MB_MAX_BUFFER; ++i){
            mbHandle->buffer[i] = 0;
        }
        mbHandle->bufferSize = 0;
        
        read_uart(mbHandle->uart, (uint8_t*)mbHandle->buffer, MB_MAX_BUFFER);

        // Get size of message not including : and CRLF
        // buffer is 16bit so byte count = bufferSize*2
        mbHandle->bufferSize = 0;
        for(uint8_t i = 0; i < MB_MAX_BUFFER; ++i){
            if(mbHandle->buffer[i] == '\r') break;
            mbHandle->bufferSize++;
        }

        // Convert the message to numeric values
        ASCIItoHEX(mbHandle);

        // Frame too small
        if(mbHandle->bufferSize < 7){
            mbHandle->error = ERR_BAD_SIZE;
            buildError(mbHandle, &mbHandle->error);
            respond_modbus(mbHandle);
            continue;
        }

        // Message not for this device
        if(mbHandle->buffer[ID] != mbHandle->id) continue;

        // error thrown
        mb_error_t error = validateMessage(mbHandle);
        if(error > ERR_NO_ERR){
            if(error != ERR_TIME_OUT){
                buildError(mbHandle, &error);
                respond_modbus(mbHandle);
            }
            mbHandle->error = error;
            continue;
        }

        // execute function
        // some registers are combined in this implimentation
	    switch(mbHandle->buffer[FUNC]) {
		    case MB_FUNC_READ_COILS:
		    case MB_FUNC_READ_DISCRETE_INPUT:
		    	mb_FUNC1(mbHandle);
		    	break;
		    case MB_FUNC_READ_INPUT_REGISTER:
		    case MB_FUNC_READ_REGISTERS :
		    	mb_FUNC3(mbHandle);
		    	break;
		    case MB_FUNC_WRITE_COIL:
		    	mb_FUNC5(mbHandle);
		    	break;
		    case MB_FUNC_WRITE_REGISTER :
		    	mb_FUNC6(mbHandle);
		    	break;
		    case MB_FUNC_WRITE_MULTIPLE_COILS:
		    	mb_FUNC15(mbHandle);
		    	break;
		    case MB_FUNC_WRITE_MULTIPLE_REGISTERS :
		    	mb_FUNC16(mbHandle);
		    	break;
		    default:
		    	
	    }
    }
}

// check message for errors
mb_error_t validateMessage(mb_handler_t* mbHandle){
    // LRC invalid
    uint16_t lrc = calcLRC(mbHandle);
    if(mbHandle->buffer[mbHandle->bufferSize] != lrc){
        return ERR_BAD_LRC;
    }

    // Function code invalid or address range invalid
    // some registers are combined in this implimentation
    uint16_t range = 0;
    switch(mbHandle->buffer[FUNC]){
        case MB_FUNC_READ_COILS:
	    case MB_FUNC_READ_DISCRETE_INPUT:
	    case MB_FUNC_WRITE_MULTIPLE_COILS:
	        range = ((((uint16_t)mbHandle->buffer[ADD_HI]) << 8) + mbHandle->buffer[ADD_LO]) >> 4;
	        range += ((((uint16_t)mbHandle->buffer[NB_HI]) << 8) + mbHandle->buffer[NB_LO]) >> 4;
	        if (range > mbHandle->regSize) return ERR_ADDR_RANGE;
	        break;
	    case MB_FUNC_WRITE_COIL:
	        range = ((((uint16_t)mbHandle->buffer[ADD_HI]) << 8) + mbHandle->buffer[ADD_LO]) >> 4;
	        if (range > mbHandle->regSize) return ERR_ADDR_RANGE;
	        break;
	    case MB_FUNC_WRITE_REGISTER :
	        range = ((((uint16_t)mbHandle->buffer[ADD_HI]) << 8) + mbHandle->buffer[ADD_LO]);
	        if (range > mbHandle->regSize) return ERR_ADDR_RANGE;
	        break;
	    case MB_FUNC_READ_REGISTERS :
	    case MB_FUNC_READ_INPUT_REGISTER :
	    case MB_FUNC_WRITE_MULTIPLE_REGISTERS :
	        range = ((((uint16_t)mbHandle->buffer[ADD_HI]) << 8) + mbHandle->buffer[ADD_LO]);
	        range += ((((uint16_t)mbHandle->buffer[NB_HI]) << 8) + mbHandle->buffer[NB_LO]);
	        if (range > mbHandle->regSize) return ERR_ADDR_RANGE;
	        break;
        default:
            return ERR_FUNC_CODE;
            break;
    };
    return ERR_NO_ERR;
}

void mb_FUNC1(mb_handler_t* mbHandle){

    mbHandle->buffer[ID     ] = 0x04;
    mbHandle->buffer[FUNC   ] = 0x01;
    mbHandle->buffer[ADD_HI ] = 0x02;
    mbHandle->buffer[ADD_LO ] = 0x0A;
    mbHandle->buffer[NB_HI  ] = 0x11;
    mbHandle->bufferSize = 5;
    respond_modbus(mbHandle);
}

void mb_FUNC3(mb_handler_t* mbHandle){
    
    respond_modbus(mbHandle);
}

void mb_FUNC5(mb_handler_t* mbHandle){
    
    respond_modbus(mbHandle);
}

void mb_FUNC6(mb_handler_t* mbHandle){

    respond_modbus(mbHandle);
}

void mb_FUNC15(mb_handler_t* mbHandle){

    respond_modbus(mbHandle);
}

void mb_FUNC16(mb_handler_t* mbHandle){
    
    respond_modbus(mbHandle);
}

void respond_modbus(mb_handler_t* mbHandle){
    mbHandle->buffer[mbHandle->bufferSize] = calcLRC(mbHandle);
    mbHandle->bufferSize++;
    HEXtoASCII(mbHandle);
    mbHandle->buffer[mbHandle->bufferSize] = '\0';
    printf_uart(mbHandle->uart, ":%s\r\n", (uint8_t*)mbHandle->buffer);
}

void buildError(mb_handler_t* mbHandle, mb_error_t* mbError){
    mbHandle->buffer[ID]      = mbHandle->id;
    mbHandle->buffer[FUNC]    = mbHandle->buffer[FUNC] + 0x80u;
    mbHandle->buffer[2]       = *mbError;
    mbHandle->bufferSize      = 0x03u;
}

uint8_t calcLRC(mb_handler_t* mbHandle){
    uint16_t lrc = 0;
    uint8_t* u8buffer = (uint8_t*)mbHandle->buffer;

    lrc = 0;
    for(uint8_t i = 0; i < mbHandle->bufferSize<<1; ++i){
        lrc = (lrc + u8buffer[i]);
    }
    return (uint8_t)(((lrc ^ 0xFF) + 1));
}

// Convert ASCII message to numeric values
void ASCIItoHEX(mb_handler_t* mbHandle){
    uint8_t* u8buffer = (uint8_t*)(mbHandle->buffer);
    for(uint8_t i = 0; i < mbHandle->bufferSize; ++i){
        // sum the values from hi and lo bytes into the lo byte
        // LITTLE ENDIAN -> bytes needs to be swapped
        mbHandle->buffer[i] = (atoh(u8buffer[(i<<1)]) << 4) + atoh(u8buffer[(i<<1)+1]);
    }
}

// Convert ASCII coded HEX to numeric HEX
uint8_t atoh(uint8_t msg){

    if(msg >= (uint8_t)'0' && msg <= (uint8_t)'9'){
        return msg - (uint8_t)'0';
    }
    else if(msg >= (uint8_t)'A' && msg <= (uint8_t)'F'){
        return msg - (uint8_t)'A' + 0x0Au;
    }
    else if(msg >= (uint8_t)'a' && msg <= (uint8_t)'f'){
        return msg - (uint8_t)'a' + 0x0Au;
    }
    else{
        return (uint8_t)0x3Au;
    }
}

// Convert numeric HEX values to ASCII coded HEX
void HEXtoASCII(mb_handler_t* mbHandle){
    for(uint8_t i = 0; i < mbHandle->bufferSize; ++i){
        // expand the lo byte to ASCII in the hi and lo bytes
        mbHandle->buffer[i] = htoa((uint8_t)(mbHandle->buffer[i]));
    }
}

uint16_t htoa(uint8_t msg){
    uint16_t u16msg = 0;
    uint8_t msg_hi = msg&0x0Fu;
    uint8_t msg_lo = (msg&0xF0u) >> 4;
    // lo byte
    if(msg_lo >= 0 && msg_lo <= 9){
        u16msg += (msg_lo + (uint8_t)'0');
    }
    else if(msg_lo >= 10 && msg_lo <= 15){
        u16msg += (msg_lo - 0x0Au) + (uint8_t)'A';
    }
    // hi byte
    if(msg_hi >= 0 && msg_hi <= 9){
         u16msg += (uint16_t)(msg_hi + (uint8_t)'0') << 8;
    }
    else if(msg_hi >= 10 && msg_hi <= 15){
        u16msg += (uint16_t)((msg_hi - 0x0Au) + (uint8_t)'A') << 8;
    }
    return u16msg;
}