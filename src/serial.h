/******************************************************************************
   Filename     : serial.c     
   Author       : S.Vasu Pandi 
   Last Modified: 26/06/2007                                        
   Description  : SPI initialisation are done here.                 
******************************************************************************/
// include I/O definitions (port names, pin names, etc)
//include spi read function  
//#include "spi.h"
/***********************SPI Initialisation************************************/

void spiInit(void){

int i;

// 0 - PORT as input  1- port as output
//Write Logical One to SS,MISO,SCK (DB4,5,7) and Zero to MOSI(DB6) pin

DDRB=(1<<DDB5)|(1<<DDB7)|(1<<DDB4);           

SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA);

//write value 1 to PORTB7,6
PORTB|=0X80;

PORTB|=0X40;

for(i=0;i<10;i++);

PORTB &=0XEF;

}

/***********************START TRANSMISSION************************************/
void spiPut(char L_C_data){
char L_C_value;

// Start transmission by writing Value to SPDR register
SPDR=L_C_data;

// Wait until Transmission Interrupt Occur    
while(!(SPSR & (1<<SPIF)));

//get value from SPDR
L_C_value=SPSR;

L_C_value=SPDR;

}
/***********************GET SPI VALUES ************************************/
char spiGet(char L_C_data){

// Bytewise Operation
char L_C_value;

//Start transmission by writing Value to SPDR register
SPDR=L_C_data;

// Wait until Transmission Interrupt Occur
while(!(SPSR & (1<<SPIF))); 

L_C_value=SPSR;

//Get the SPDR value
L_C_value=SPDR;

//return the SPDR value
return(L_C_value);

}
