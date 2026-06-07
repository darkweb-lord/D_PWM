/******************************************************************************
   Filename     : serial.c     
   Author       : Manjula P
   Last Modified: 27/05/09                                          
   Description  : SPI initialisation, Read ,write are done here.                 
******************************************************************************/
/*  include I/O definitions (port names, pin names, etc) */
    #include <avr/io.h>
/*  include interrupt header files */
    #include "avr/interrupt.h"
/********************************************************************
*     Function Name:    SPI initialisation                          *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This routine is used to  initialise the SPI *
                        communication                               *  
********************************************************************/
void spiInit(void){

/*  0 - PORT as input  1- port as output
 *  Write Logical One to SS,MISO,SCK (DB4,5,7) and Zero to MOSI(DB6) pin */
    int i;
    DDRB=(1<<DDB5)|(1<<DDB7)|(1<<DDB4);           
    PORTD |= 0X20;
    //SPCR0=(1<<SPE0)|(1<<MSTR0)|(1<<SPR00)|(1<<CPHA0);
    for(i=0;i<100;i++);
}
/********************************************************************
*     Function Name:    SPIPUT                                      *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This routine is used to write the values to *
                        metering IC through spi data register       *  
********************************************************************/
void spiPut(char L_C_data){
    char L_C_value;
/*  Start transmission by writing Value to SPDR register */
    SPDR0=L_C_data;
/*  Wait until Transmission Interrupt Occur*/  
    while(!(SPSR0 & (1<<SPIF0)));
/*  get value from SPDR*/ 
    L_C_value=SPSR0;   
}

/********************************************************************
*     Function Name:    SPIGET                                      *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This routine is used to read the values from*
                        metering IC through spi data register       *  
********************************************************************/
char spiGet(char L_C_data){
/*  Bytewise Operation */
    char L_C_value;
/*  Start transmission by writing Value to SPDR register */
    SPDR0=L_C_data;
/*  Wait until Transmission Interrupt Occur */
    while(!(SPSR0 & (1<<SPIF0))); 
    L_C_value=SPSR0;
/*  Get the SPDR value */
    L_C_value=SPDR0;
/*  return the SPDR value */
    return(L_C_value);
}
/*******************************************************************************/
