/******************************************************************************
               Filename     : Writespi.h     
               Author       : Manjula P
               Last Modified: 5/12/2009                                      
               Description  : Metering parameters are read here.
 **************************************************************************/
#ifndef _STLED316_DRIVER_
#define _STLED316_DRIVER_

#define WRITE_COMMAND 0X0
#define READ_COMMAND  0X40
#define FIXD_ADDRESS 0X20
#define INCREMENT_ADDRESS 0X0

#define DISPLAY_ON 0XD
#define DISPLAY_OFF 0XE

#define CHIP_CONFIG_PAGE (0X2 << 3)
#define CHIP_CONFIG_ADDRESS 0X0
#define CHIP_CONFIGURATION 0XFD						//6 digits,8segments,constand brightness all

#define DIGIT_BRIGHTNESS_PAGE (0X2 << 3)
#define DIGIT_BRIGHTNESS_ADDRESS 0X1				//0X1-0X3
#define DIGIT_BRIGHTNESS 0X70

#define SEGMENT_DATA_PAGE (0X0 << 3)
#define SEGMENT1_DATA_ADDRESS 0X0					//0X0-0X5

#define LED_DATA_PAGE (0X1 << 3)
#define LED_DATA_ADDRESS 0X0

#endif

/*****************************************************************************
                         Delay function
   This routine is used for creating delay in displaying LCD chars, 
this delay is necessary to match the timing specified for LCD modules. 
10000 Counts causes a delay of 1msec (approx)      
 ******************************************************************************/
void nested_delay(int delay, int loopdelay) //To Generate delay
{
    int i, j;
    for (j = 0; j < delay; j++) {
        for (i = 0; i < loopdelay; i++) {
        }
    }
    return;
}

/****************************Update DAC**************************************************/
void WriteDACData(unsigned int wdata, int length) {
    int i;
    unsigned int portval = 1;

    portval <<= (length - 1);
    PORTD |= 0x10;
    PORTD &= 0XDF; /* Chip select */
    //nested_delay(1,1);                                  
    for (i = 0; i < length; i++) {
        if (wdata & portval) PORTB |= 0x20;
        else PORTB &= ~0x20;

        PORTB |= 0x80;
        //  nested_delay(1,1);        
        PORTB &= ~0x80;
        wdata <<= 1;
    }
    PORTD |= 0x20; /* Chip unselect */

}

/*********************************Update STLED*********************************************/
void WriteSTLEDData(unsigned char data, unsigned char chipsel) {
    unsigned char i;
    unsigned char temp;
    PORTD |= 0x20;
    PORTD &= 0xEF; //chip deselect
    nested_delay(1, 1);
    for (i = 0; i < 8; i++) {
        PORTB &= ~0x80; //clock low

        temp = data & (0x1 << i);

        if (temp)
            PORTB |= 0x20; //data high
        else
            PORTB &= ~0x20; //data low

        PORTB |= 0x80; //clock high

        //nested_delay(1,1);					
    }
    if (chipsel)PORTD |= 0x10; //chip select				
}

/*********************************Initialise STLED******************************************/
void LED_Init() {
    /*const/vriable brightness,number of digits  congfig */

    WriteSTLEDData((CHIP_CONFIG_ADDRESS + CHIP_CONFIG_PAGE + FIXD_ADDRESS + WRITE_COMMAND), 0);

    WriteSTLEDData(CHIP_CONFIGURATION, 1);

    /* brightness config */

    WriteSTLEDData((DIGIT_BRIGHTNESS_PAGE + DIGIT_BRIGHTNESS_ADDRESS + INCREMENT_ADDRESS + WRITE_COMMAND), 0);

    WriteSTLEDData(DIGIT_BRIGHTNESS, 0);

    WriteSTLEDData(DIGIT_BRIGHTNESS, 0);

    WriteSTLEDData(DIGIT_BRIGHTNESS, 1);

    return;
}

/***********************************************************************************************/
void Write_To_STLed316(unsigned char dataaddress, unsigned char PAGE, unsigned char address, unsigned char data) {

    /*start write display data */
    WriteSTLEDData((dataaddress + PAGE + address + WRITE_COMMAND), 0);
    WriteSTLEDData(data, 1);
    /* write display on command */
    WriteSTLEDData(DISPLAY_ON, 1);

    return;
}
/**********************************************************************************************/
