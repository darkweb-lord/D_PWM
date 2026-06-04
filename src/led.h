/******************************************************************************
           Filename     : led.h     
           Author       : Manjula P
           Last Modified: 29/01/2010                                       
           Description  : Display functions are defined
 *******************************************************************************/

/*************************Split the digits******************************************************************/
void split_numeric(int value) {
    unsigned char j = 0;
    while (j < 5) {
        numeric[j] = value % 10;
        value = value / 10;
        j++;
    }
    if (SIGN)numeric[4] = 30;
    return;
}

/************************************************************************************************************/
void Limitdigit_function(int index) {
    unsigned char i = 0;
    //To display 0.0/0.00
    if (DECIMAL_POINT == 1 && index < 10)index = index + 10;
    else if (DECIMAL_POINT == 2 && index < 100)index = index + 100;
    else if (DECIMAL_POINT == 3 && index < 1000)index = index + 1000;

    /* limit the no of digits */
    if (index < 10) {
        for (i = 1; i < 5; i++)numeric[i] = 34;
    } else if (index < 100) {
        for (i = 2; i < 5; i++)numeric[i] = 34;
    } else if (index < 1000) {
        for (i = 3; i < 5; i++)numeric[i] = 34;
    } else if (index < 10000) {
        numeric[4] = 34;
    }

    return;
}

/***********************To display values in LED************************************************************/
void DISPLAY_VALUE(void) {
    unsigned char i = 0;
    unsigned char DP = 0x00;

    /* limit no of digits */
    if (OPMENU_PAGE > 0) {
        if (ENTER_KEYCOUNT <= 1) {
            Limitdigit_function(OPMENU_PAGE);
            numeric[4] = 35;
        } else {
            Limitdigit_function(fabs(CALIBRATION[SETUP_INDEX]));
        }
        if (SIGN && ENTER_KEYCOUNT > 1)numeric[4] = 30; //To display negative sign
    }
    /* Display values */
    if (ENTER_KEYCOUNT > 1 && (OPMENU_PAGE == 1 || OPMENU_PAGE == 4 || OPMENU_PAGE == 14 || OPMENU_PAGE == 16 || OPMENU_PAGE == 17)) Write_To_STLed316(0x00, LED_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[31 + CALIBRATION[UNIT]]);
    else Write_To_STLed316(0x00, LED_DATA_PAGE, INCREMENT_ADDRESS, 0x00);
    for (i = 0; i < 5; i++) {

        if ((i == 1 && DECIMAL_POINT == 1) || (i == 2 && DECIMAL_POINT == 2) || (i == 3 && DECIMAL_POINT == 3))DP = 0x80;
        else DP = 0x00;
        if (OPMENU_PAGE > 0) {

            if (CURSOR == FALSE) {
                // writing the value to display		
                Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[numeric[i]] | DP);
            } else {
                if (i == CURSOR_POSITION) //LED Blinking for cursor display 
                {
                    if (Displaycount >= 30) //300msec
                    {
                        Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DP);
                        Displaycount = 0;
                    } else {
                        Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[numeric[i]] | DP);

                    }

                } else {
                    Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[numeric[i]] | DP);
                }
            }

        }

    }

    return;
}
/*******************************************************************************************************************/
