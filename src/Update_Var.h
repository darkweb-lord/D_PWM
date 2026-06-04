/***************************************************************************
               Filename     : Update_Var.h    
               Author       : Manjula P
               Last Modified: 05/05/2010                                     
               Description  : Values are updated    
 ********************************Configuring digital inputs*************************************************/
void Digitalinput_Function(void) {
    // Read the digital inputs
    CALIBRATION[DIGITALINPUT1] = (((PINC & 0x10) >> 4)^1)^CALIBRATION[INVERT1];
    CALIBRATION[DIGITALINPUT2] = (((PINC & 0x20) >> 5)^1)^CALIBRATION[INVERT2];

    /*Select input to Reset Peak value */
    if (CALIBRATION[PEAKRESETINPUT] == 0) {
        PEAKRESET = 0;
        PEAKSTOP = 0;
    }//False
    else if (CALIBRATION[PEAKRESETINPUT] == 1) {
        PEAKRESET = 1;
        PEAKSTOP = 1;
    }// True always reset peak	
    else if (CALIBRATION[PEAKRESETINPUT] == 2) //Reset based on edge of digital input1
    {
        if (PREVDI1 != CALIBRATION[DIGITALINPUT1]) {
            PREVDI1 = CALIBRATION[DIGITALINPUT1];
            if (PREVDI1 == 1 && (CALIBRATION[PEAKRESETTYPE] == 0 || CALIBRATION[PEAKRESETTYPE] == 2)) {
                PEAKRESET = 1;
                PEAKSTOP = 0;
            }//Rising edge sense
            else if ((CALIBRATION[PEAKRESETTYPE] == 1 || CALIBRATION[PEAKRESETTYPE] == 3) && PREVDI1 == 0) {
                PEAKRESET = 1;
                PEAKSTOP = 0;
            }//Falling edge sense
            else {
                PEAKSTOP = 1;
            }
        }
    } else if (CALIBRATION[PEAKRESETINPUT] == 3) //Reset based on edge of digital input2
    {
        if (PREVDI2 != CALIBRATION[DIGITALINPUT2]) {
            PREVDI2 = CALIBRATION[DIGITALINPUT2];
            if ((CALIBRATION[PEAKRESETTYPE] == 0 || CALIBRATION[PEAKRESETTYPE] == 2) && PREVDI2 == 1) {
                PEAKRESET = 1;
                PEAKSTOP = 0;
            }//Rising edge sense
            else if ((CALIBRATION[PEAKRESETTYPE] == 1 || CALIBRATION[PEAKRESETTYPE] == 3) && PREVDI2 == 0) {
                PEAKRESET = 1;
                PEAKSTOP = 0;
            }//Falling edge sense
            else {
                PEAKSTOP = 1;
            }
        }

    }
    //Select AUTO/MAN mode
    if (CALIBRATION[AUTOMANSELECT] == 0)AUTOMAN = 0;
    else if (CALIBRATION[AUTOMANSELECT] == 1)AUTOMAN = 1;
    else if (CALIBRATION[AUTOMANSELECT] == 2)AUTOMAN = CALIBRATION[DIGITALINPUT1];
    else if (CALIBRATION[AUTOMANSELECT] == 3)AUTOMAN = CALIBRATION[DIGITALINPUT2];

    //Enable/ Disable PID
    if (CALIBRATION[PIDSTATUS] == 0)PIDENABLE = 0;
    else if (CALIBRATION[PIDSTATUS] == 1)PIDENABLE = 1;
    else if (CALIBRATION[PIDSTATUS] == 2)PIDENABLE = CALIBRATION[DIGITALINPUT1];
    else if (CALIBRATION[PIDSTATUS] == 3)PIDENABLE = CALIBRATION[DIGITALINPUT2];

    /*Reset length count */
    if (PREVDI2 != CALIBRATION[DIGITALINPUT2]) {
        PREVDI2 = CALIBRATION[DIGITALINPUT2];
        Lengthcnt = 0;
        Storedcnt = 0;
        LENGTH = 0;
        CALIBRATION[LENGTHCOUNT] = 0;
        eeprom_write_word((&ee_setup + 330), 0);
    }
    return;
}

/****************************Calibrating Load cell********************************************************/
void LOADCELL_CALIB(void) {
    unsigned char i = 0;

    // Read the digital inputs
    PUSHTOCAL = ((PINC & 0x40) >> 6); //Cal
    PUSHTOZERO = ((PINC & 0x80) >> 7); //Zero	

    /* Check the direction of load cell*/
    if (AI_RAWVALUE[0]<(ForwardOffset / 10.0))DIRECTION = 1;
    else DIRECTION = 0;

    /*Loadcell calibration,set offset*/
    if (PUSHTOZERO != PREVVALUE) {

        if (PUSHTOZERO == 1)PREVVALUE = PUSHTOZERO;
        else {
            ZEROCOUNT++;
            if (ZEROCOUNT >= 60) {
                PREVVALUE = PUSHTOZERO;
                ForwardOffset = AI_RAWVALUE[0]*10.0;
                CALIBRATION[OFFSET] = ForwardOffset; //interms of %  	  
                eeprom_write_word((&ee_setup + 352), CALIBRATION[OFFSET]);
                OPMENU_PAGE = ENTER_KEYCOUNT = 0;
                FAULT = 1;
                ZEROCOUNT = 0;
            }
        }
    }
    /*set Max values during calibration*/
    if (PUSHTOCAL != PREVIOUSVAL) {


        if (PUSHTOCAL == 1)PREVIOUSVAL = PUSHTOCAL;
        else {

            CALCOUNT++;
            if (CALCOUNT >= 60) {
                PREVIOUSVAL = PUSHTOCAL;
                if (fabs(AI_RAWVALUE[0]-(ForwardOffset / 10.0)) <= CALIBRATION[MINTENSION]) {
                    Weightsmall = 1;
                    //Fm=Rm=Forward_Slope=Reverse_Slope=CALIBRATION[FORWARDSLOPE]=CALIBRATION[REVERSESLOPE]=CALIBRATION[FORWARDCALSIGNAL]=CALIBRATION[REVERSECALSIGNAL]=CALIBRATION[CALTEMP]=CALIBRATION[CALEXCITATION]=0;
                } else {
                    Weightsmall = 0;

                    if (DIRECTION) //Forward direction
                    {
                        CALIBRATION[FORWARDCALSIGNAL] = AI_RAWVALUE[0]*10.0;
                        //slope(kg/%)
                        Fm = fabs(((float) CALIBRATION[CALWEIGHT]*10.0) / (DP_Scale[CALIBRATION[DPSELECT]]*(AI_RAWVALUE[0]-(ForwardOffset / 10.0))));
                        Forward_Slope = Fm * 100.0;
                        CALIBRATION[FORWARDSLOPE] = Fm * 10.0;
                    } else {
                        //Reverse direction
                        CALIBRATION[REVERSECALSIGNAL] = AI_RAWVALUE[0]*10.0;
                        //slope(kg/%)
                        Rm = fabs(((float) CALIBRATION[CALWEIGHT]*10.0) / (DP_Scale[CALIBRATION[DPSELECT]]*(AI_RAWVALUE[0]-(ForwardOffset / 10.0))));
                        Reverse_Slope = Rm * 100.0;
                        CALIBRATION[REVERSESLOPE] = Rm * 10.0;
                    }
                    CALIBRATION[CALTEMP] = CALIBRATION[TEMPERATURE];
                    CALIBRATION[CALEXCITATION] = CALIBRATION[AIOUTPUT3];
                    eeprom_write_word((&ee_setup + 358), CALIBRATION[FORWARDCALSIGNAL]);
                    eeprom_write_word((&ee_setup + 360), CALIBRATION[REVERSECALSIGNAL]);
                    eeprom_write_word((&ee_setup + 366), Forward_Slope);
                    eeprom_write_word((&ee_setup + 368), Reverse_Slope);
                    eeprom_write_word((&ee_setup + 370), CALIBRATION[CALTEMP]);
                    eeprom_write_word((&ee_setup + 372), CALIBRATION[CALEXCITATION]);
                }

                OPMENU_PAGE = ENTER_KEYCOUNT = 0;
                FAULT = 2;
                CALCOUNT = 0;

            }
        }

    }
    /* Display text messages*/
    if (OPMENU_PAGE == 0) {

        for (i = 0; i < 5; i++) {
            if (FAULT == 2) {
                if (Weightsmall == 1) {
                    Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[14 - i]);
                }//weight too small (F1-SS)		                
                else {
                    Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[24 - i]);
                }//Push to cal pressed(F3-PC)					
            }
            else if (FAULT == 1) {
                Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[19 - i]); //Push to zero pressed(F2-PO)		        
            }
            else if (FAULT == 3) {
                Write_To_STLed316((0x00 + i), SEGMENT_DATA_PAGE, INCREMENT_ADDRESS, DATADISPLAY[29 - i]); //Password F4-Pn
            }
        }
        //2 seconds
        if (Messagecount > 200) {
            if (FAULT < 3) {
                OPMENU_PAGE = DIGITS;
                ENTER_KEYCOUNT = 2;
            } else if (FAULT == 3) {
                OPMENU_PAGE = 13;
                ENTER_KEYCOUNT = 2;
                CALIBRATION[ACCESSCODE] = 0;
            }
            Messagecount = FAULT = 0;
        }
    }

    return;
}

/********************************Update Polynomial co efficients**************************************/
void UpdatePolynomial_Coeff(void) {

    Actual_TensionA = (float) CALIBRATION[ACTUALA] * ACT_TENSIONA;
    if (CALIBRATION[ACTUALB] < 0)Actual_TensionB = (float) CALIBRATION[ACTUALB] * ACT_TENSIONB * 10.0;
    else Actual_TensionB = (float) CALIBRATION[ACTUALB] * ACT_TENSIONB;
    if (CALIBRATION[ACTUALC] < 0)Actual_TensionC = (float) CALIBRATION[ACTUALC] * ACT_TENSIONC * 10.0;
    else Actual_TensionC = (float) CALIBRATION[ACTUALC] * ACT_TENSIONC;

    Set_TensionA = (float) CALIBRATION[SETA] * SET_TENSIONA;
    if (CALIBRATION[SETB] < 0)Set_TensionB = (float) CALIBRATION[SETB] * SET_TENSIONB * 10.0;
    else Set_TensionB = (float) CALIBRATION[SETB] * SET_TENSIONB;
    if (CALIBRATION[SETC] < 0)Set_TensionC = (float) CALIBRATION[SETC] * SET_TENSIONC * 10.0;
    else Set_TensionC = (float) CALIBRATION[SETC] * SET_TENSIONC;
    return;
}

/*****************************************Update analog output range ***********************************************/
void AO_Rangeselect(void) {

    unsigned char i = 0;

    for (i = 0; i < 2; i++) {
        switch (CALIBRATION[AOTYPE1 + (i * 9)]) //Output Range selection
        {

            case 0: TypeScaling[i] = 0.5;
                Scale[i] = CALIBRATION[AOFORWARDSCALE1 + (i * 9)];
                break;
            case 1: TypeScaling[i] = 1.0;
                Scale[i] = CALIBRATION[AOFORWARDSCALE1 + (i * 9)];
                break;
            case 2: if (CALIBRATION[AOOUTPUT1 + (i * 9)] < 0.0) {
                    TypeScaling[i] = -0.5;
                    Scale[i] = CALIBRATION[AOREVERSESCALE1 + (i * 9)];
                } else {
                    TypeScaling[i] = 0.5;
                    Scale[i] = CALIBRATION[AOFORWARDSCALE1 + (i * 9)];
                }

                break;
            case 3: if (CALIBRATION[AOOUTPUT1 + (i * 9)] < 0.0) {
                    TypeScaling[i] = -1.0;
                    Scale[i] = CALIBRATION[AOREVERSESCALE1 + (i * 9)];
                } else {
                    TypeScaling[i] = 1.0;
                    Scale[i] = CALIBRATION[AOFORWARDSCALE1 + (i * 9)];
                }

                break;
            case 4: TypeScaling[i] = 1.0;
                Scale[i] = CALIBRATION[AOFORWARDSCALE1 + (i * 9)];
                break;
            default: break;
        }
    }
    return;
}

/*****************************************Update PID variables and drift constants******************************************************/
void Value_updation(void) {
    unsigned char i = 0;

    PGain = (float) CALIBRATION[PGAIN] / 10.0;

    ITime = (float) CALIBRATION[IGAIN] / 10.0;

    PLmt = (float) CALIBRATION[PLIMIT] / 10.0;

    DeadBand = (float) CALIBRATION[DEADBAND] / 10.0;

    DTime = (float) CALIBRATION[DGAIN] / 10.0;

    ILmt = (float) CALIBRATION[ILIMIT] / 10.0;

    DLmt = (float) CALIBRATION[DLIMIT] / 10.0;

    Positivelmt = CALIBRATION[POSLIMIT] / 10.0;

    Neglmt = CALIBRATION[NEGLIMIT] / 10.0;

    Temp_Drift = (float) CALIBRATION[TEMPDRIFT] / 10000.0;

    Supply_Drift = (float) CALIBRATION[SUPPLYDRIFT] / 10000.0;

    for (i = 0; i < 4; i++)AI_FILTER[i] = 1.0 - exp(-(AI_SCANTIME / (float) CALIBRATION[AIRESPONSE1 + (i * 7)]));

    for (i = 0; i < 2; i++)AO_FILTER[i] = 1.0 - exp(-(AI_SCANTIME / (float) CALIBRATION[AORESPONSE1 + (i * 9)]));

    return;
}

/*************************************Updating necessary speed constants*************************************************/
void Update_Speedconstants(void) {

    Speed_Constant = 60.0 / (float) CALIBRATION[ENCPPR];

    Freq_Scale = (float) CALIBRATION[FREQMULTIPLIER] / 10000.0;

    Sampling_Freq = 1000.0 / (float) CALIBRATION[SAMPLINGINTERVAL];

    Filter_Coeff = 1.0 - exp(-(AI_SCANTIME / (float) CALIBRATION[SPDRESPONSE]));


    return;

}

/**********************************************************************************************************************/
void Precision_Updation(void) {
    /*float DP_ARRAY[3]={0.1,1.0,10.0}; 
        
      switch(PrevDP)
      {
         case 0:  CALIBRATION[MAXTENSION]=CALIBRATION[MAXTENSION]*DP_Scale[CALIBRATION[DPSELECT]];

                  CALIBRATION[CALWEIGHT]=CALIBRATION[CALWEIGHT]*DP_Scale[CALIBRATION[DPSELECT]];
                  break;

         case 1:  CALIBRATION[MAXTENSION]=CALIBRATION[MAXTENSION]*DP_ARRAY[CALIBRATION[DPSELECT]];

                  CALIBRATION[CALWEIGHT]=CALIBRATION[CALWEIGHT]*DP_ARRAY[CALIBRATION[DPSELECT]];
                  break;

         case 2:  CALIBRATION[MAXTENSION]=CALIBRATION[MAXTENSION]/DP_Scale[CALIBRATION[DPSELECT]];

                  CALIBRATION[CALWEIGHT]=CALIBRATION[CALWEIGHT]/DP_Scale[CALIBRATION[DPSELECT]];
                  break;
         default: break;
      }		
		
      eeprom_write_word((&ee_setup+32),CALIBRATION[MAXTENSION]);
      eeprom_write_word((&ee_setup+34),CALIBRATION[CALWEIGHT]);*/
    return;
}

/**********************************************************************************************************************/
void Count_Saving(void) {

    /*if(Prev_count!=Lengthcnt)
    {
	     
          CountRx++;
          if(CountRx>10)
          {		    
             eeprom_write_word((&ee_setup+330),CALIBRATION[LENGTHCOUNT]);
             CountRx=0;Savecnt=0;
          }
          Prev_count=Lengthcnt;
    }
     else 
     {
        if(Pulse_Rx==1)
        {
          Savecnt++;
          if(Savecnt>1500)
          {
                if(Prev_count!=Lengthcnt)
                {
                    eeprom_write_word((&ee_setup+330),CALIBRATION[LENGTHCOUNT]);
                    Prev_count=Lengthcnt;
                }
                Savecnt=0;Pulse_Rx=0;CountRx=0;
          }  
        } 
     }*/


    return;
}
/********************************************************************************/
