/***************************************************************************
               Filename     : Init_Variables.h    
               Author       : Manjula P
               Last Modified: 29/01/2010                                      
               Description  : Values are initialised    
 **********************************************************************************/

/**************************Read values from memory***********************************/
void Read_memory(void) {
    unsigned char i = 0, j = 0;


    for (j = 0; j < 6; j++) {

        for (i = 21; i < 28; i++)CALIBRATION[i] = 0;
        CALIBRATION[PARAMETERLOCK] = 1;

        for (i = 14; i < 21; i++)CALIBRATION[i] = eeprom_read_word(&ee_setup + (i * 2));

        for (i = 28; i < 161; i++)CALIBRATION[i] = eeprom_read_word(&ee_setup + (i * 2));

        /* Read calibration parameters */
        CALIBRATION[OFFSET] = eeprom_read_word(&ee_setup + 352);
        CALIBRATION[FORWARDCALSIGNAL] = eeprom_read_word(&ee_setup + 358);
        CALIBRATION[REVERSECALSIGNAL] = eeprom_read_word(&ee_setup + 360);
        Forward_Slope = eeprom_read_word(&ee_setup + 366);
        Reverse_Slope = eeprom_read_word(&ee_setup + 368);
        CALIBRATION[CALTEMP] = eeprom_read_word(&ee_setup + 370);
        CALIBRATION[CALEXCITATION] = eeprom_read_word(&ee_setup + 372);
        ForwardOffset = (float) CALIBRATION[OFFSET];
        Fm = (float) Forward_Slope / 100.0;
        Rm = (float) Reverse_Slope / 100.0;
        CALIBRATION[FORWARDSLOPE] = Forward_Slope / 10;
        CALIBRATION[REVERSESLOPE] = Reverse_Slope / 10;


        CALIBRATION[VERSION_NUMBER] = VERSION * 10;
        nested_delay(10, 10);
        //Calculating timer counts based on clock freq,1024 prescale,Overflow interrupt
        COUNTS = 255.0 - ((CLOCK_FREQUENCY * 1000000 * 0.010) / 1024.0);

        if (CALIBRATION[ZERODELAY] < 0 || CALIBRATION[ZERODELAY] > 20000)CALIBRATION[ZERODELAY] = 5000;

    }
    Storedcnt = eeprom_read_word(&ee_setup + 330);


}

/*****************************************************************************************************/
void initialize_variable(void) {
    unsigned char i = 0;
    for (i = 0; i < 12; i++)CALIBRATION[i] = 0;

    CALIBRATION[ACCESSCODE] = 0;

    CALIBRATION[UNIT] = 0;
    CALIBRATION[RESPONSE] = 40;

    CALIBRATION[AOTYPE1] = CALIBRATION[AOTYPE2] = 3;
    CALIBRATION[AOREVERSESCALE1] = 984;
    CALIBRATION[AOFORWARDSCALE1] = 986;
    CALIBRATION[AOFORWARDSCALE2] = 979;
    CALIBRATION[AOREVERSESCALE2] = 984;

    CALIBRATION[AOOFFSET1] = -23;
    CALIBRATION[AOOFFSET2] = -22;
    CALIBRATION[AORESPONSE1] = CALIBRATION[AORESPONSE2] = 1;
    CALIBRATION[PASSWORD] = 4;
    CALIBRATION[INVERT1] = 0;
    CALIBRATION[INVERT2] = 0;
    CALIBRATION[DPSELECT] = 1;
    CALIBRATION[ABSOLUTEVALUE] = 0;
    CALIBRATION[ZERODELAY] = 5000;
    CALIBRATION[THRESHOLD] = 470;

    if (CALIBRATION[FACTORYSET] == 1 || CALIBRATION[FACTORYSET] == 2 || CALIBRATION[FACTORYSET] == 5) //DTI
    {

        if (CALIBRATION[FACTORYSET] == 2) {
            CALIBRATION[CONTROLLER] = 2;
            CALIBRATION[CONTINUOUSDISPLAY] = 4;
        } else if (CALIBRATION[FACTORYSET] == 1) {
            CALIBRATION[CONTROLLER] = 1;
            CALIBRATION[CONTINUOUSDISPLAY] = 1;
        } else {
            CALIBRATION[CONTROLLER] = 5;
            CALIBRATION[CONTINUOUSDISPLAY] = 1;
        }

        CALIBRATION[CALWEIGHT] = 1000;
        CALIBRATION[MAXTENSION] = 1000;
        CALIBRATION[MINTENSION] = 20;

        CALIBRATION[AUTOMANSELECT] = 0;
        CALIBRATION[PIDSTATUS] = 1;
        CALIBRATION[PGAIN] = 500;
        CALIBRATION[PLIMIT] = 500;
        CALIBRATION[IGAIN] = 100;
        CALIBRATION[ILIMIT] = 1000;
        CALIBRATION[DGAIN] = 0;
        CALIBRATION[DLIMIT] = 1000;

        CALIBRATION[POSLIMIT] = 1000;
        CALIBRATION[NEGLIMIT] = -1000;
        CALIBRATION[DEADBAND] = 0;

        CALIBRATION[TEMPDRIFT] = 0;
        CALIBRATION[SUPPLYDRIFT] = 0;


        CALIBRATION[AIOFFSET1] = CALIBRATION[AIOFFSET3] = 0;
        CALIBRATION[AIOFFSET2] = -7;
        CALIBRATION[AISCALE1] = CALIBRATION[AISCALE3] = 1000;
        CALIBRATION[AISCALE2] = 1010;
        CALIBRATION[AIRESPONSE1] = CALIBRATION[AIRESPONSE2] = CALIBRATION[AIRESPONSE3] = 100;
        CALIBRATION[AIFULLSCALE1] = CALIBRATION[AIFULLSCALE2] = 1000;
        CALIBRATION[AIFULLSCALE3] = 50;
        CALIBRATION[AIFULLSCALE4] = 1100;
        CALIBRATION[AIRESPONSE4] = 100;
        CALIBRATION[AISCALE4] = 1455;
        CALIBRATION[AIOFFSET4] = -455;


        if (CALIBRATION[FACTORYSET] == 2)CALIBRATION[AOFUNCTION1] = 1;
        else CALIBRATION[AOFUNCTION1] = 4;
        CALIBRATION[AOFUNCTION2] = 2;
        CALIBRATION[AOFULLSCALE1] = 100;
        CALIBRATION[AOFULLSCALE2] = 100;


        CALIBRATION[SETA] = CALIBRATION[SETC] = CALIBRATION[ACTUALA] = CALIBRATION[ACTUALC] = 0;
        CALIBRATION[SETB] = CALIBRATION[ACTUALB] = 10000;

        CALIBRATION[PEAKCAPMETHOD] = 3;
        CALIBRATION[PEAKRESETINPUT] = 2;
        CALIBRATION[PEAKRESETTYPE] = 2;
        CALIBRATION[PEAKCAPTUREPERIOD] = 0;
        CALIBRATION[PEAKPREDELAY] = 50;
        CALIBRATION[PEAKPOSTDELAY] = 0;
        CALIBRATION[MINPOSITION] = 200;
        CALIBRATION[MAXPOSITION] = 800;
    }
    if (CALIBRATION[FACTORYSET] == 5) //DSI/Lengthcounter/all
    {

        CALIBRATION[SPEEDMETHOD] = 2;
        CALIBRATION[PULSES] = 10;
        CALIBRATION[ENCPPR] = 10;
        CALIBRATION[SAMPLINGINTERVAL] = 500;
        CALIBRATION[FREQMULTIPLIER] = 10000;
        CALIBRATION[SPDRESPONSE] = 100;

        CALIBRATION[AOFUNCTION1] = 2;
        CALIBRATION[AOFUNCTION2] = 6;
        CALIBRATION[AOFULLSCALE2] = 3000;
        CALIBRATION[AOTYPE2] = 1;
    }


}

/************************************Factory set*****************************************/
void save_parameters(void) {
    unsigned char i = 0;

    for (i = 14; i < 21; i++)eeprom_write_word((&ee_setup + (i * 2)), CALIBRATION[i]);

    for (i = 28; i < 161; i++)eeprom_write_word((&ee_setup + (i * 2)), CALIBRATION[i]);

}
/***************************************************************************/

