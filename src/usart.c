/******************************************************************************
   Filename     : Usart.C     
   Author       : Manjula P
   Last Modified: 29/01/2010                                      
   Description  : Usart Initialisation is done here.                 
******************************************************************************/
/*  include interrupt header files  */
    #include "avr/interrupt.h"
/*  Include definition file(s) */
/*  include I/O definitions (port names, pin names, etc)*/
    #include "avr/io.h"
/*  include standard io functions and library */
    #include <stdio.h>

/********************************************************************
*     Function Name:    UartInitialisation                          *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This routine is used to initialise the uart *
                        communication                               *  
********************************************************************/
void uartInit(void)
{
   // cli();
/*  Set baud rate to 38400 when running on 4.00MHz 
 *  UBRR bit11-0 for baud rate setting
 *  set 0x06 for 38400 baudrate */
	UBRR0H = 0x00;    
    UBRR0L =  0x33; 
/*  Enable receiver and transmitter */
    UCSR0B =(1<<RXEN0)|(1<<TXEN0);
/*  Default frame rate is 8 data, 1 stop, no parity bit */
	UCSR0C = 0X86;
    UCSR0A =0X00;

}
/********************************************************************/
