/***************************************************************************
           Filename     : Control.h    
           Author       : Manjula P
           Last Modified: 29/01/2010                                      
           Description  : Control functions are defined    
 ****************************************************************************
    Exponential Filter 0.0-63.2% Output is ramped in specified time.
    Input - Instantaneous Input to the filter
    Output- Filtered Output collected in previous scan(Global Variable)
    Filter- Filter Time
    Scantime- Scan time 
    Note: Filter time should be adjusted to same precision as scantime
 ******************************************************************************/
float FILTER(float LF_Input, float LF_Output, float LF_Coeff) {

    LF_Output += (LF_Input - LF_Output) * LF_Coeff;


    return LF_Output;
}

/*******************************Speed Calculation******************************************************
 Speed measurement is based on 2 methods;
 1. Edge counting method Frequency= no of edges/sampling interval(in sec)
 2. Period measurement method Frequency=(1/time between 2 rising edges)*no of pulses;
 Freq scale is used to reduce the error.
 *******************************************************************************************************/
void speed_calculation(void) {
    //calculate the  motor speed by using external interrupt0 and timer1	    		
    if (CALIBRATION[SPEEDMETHOD] == 1) //Edge counting Method
    {
        Time_Count++;
        if (Time_Count >= (CALIBRATION[SAMPLINGINTERVAL] / 10)) {
            Totalcounts = No_Of_Pulse;
            Time_Count = 0;
            No_Of_Pulse = 0;
        }
        //take the timer1 count and calculate frequency		
        Frequency = (float) Totalcounts*Sampling_Freq;

        //Apply filter	
        FilteredFreq = FILTER(Frequency, FilteredFreq, Filter_Coeff);
    } else //Period Measurement
    {
        if (Received == 1) {
            Frequency = ((8000000.0 * (float) CALIBRATION[PULSES]) / (((float) Overflow_count1 * 65535.0)+(float) Totalcounts)) * Freq_Scale;
            FilteredFreq = FILTER(Frequency, FilteredFreq, Filter_Coeff);
            Received = No_Of_Pulse = 0;
            G_F_WAIT_TIME = 0;
        } else {
            if (G_F_WAIT_TIME > (CALIBRATION[ZERODELAY] / 10)) {
                //Reset the speed if there is no pulse
                Frequency = 0.0;
                FilteredFreq = 0.0;
                Analogoutput_speed = 0.0;
                Overflow_count1 = 0;
                Totalcounts = 0;
            }
            else G_F_WAIT_TIME++;
        }
    }

    Analogoutput_speed = Frequency*Speed_Constant;
    CALIBRATION[SPDOUTPUT] = Analogoutput_speed;
    /* Frequency */
    if (FilteredFreq <= 3000.0)CALIBRATION[ACTFREQUENCY] = FilteredFreq * 10.0;
    else CALIBRATION[ACTFREQUENCY] = FilteredFreq / 10.0;

    /* Speed */
    CALIBRATION[ACTUALSPEED] = FilteredFreq*Speed_Constant;

    return;
}

/**************************Scaling the analog input***********************************************************
 Temperaturedrift=drift*(calibrated temp-actual temp);
 drift interms of (milli %/deg),actual temp,calibrated temp interms of degree.

 Supply drift=drift*(calibrated supply-actual supply);
 drift interms of (milli %/%),actual supply,calibrated supply interms of %
 Temperaturedrift,Supply drift interms of %.
 The output is increased when the temperature & supply  increases and it is reduced when the temperature & supply decreases.
 So the final actual output will increase if actual temp<cal temp or actual supply<cal supply.
 Final actual output will decrease if actual temp>cal temp or actual supply>cal supply.

 *************************************************************************************************************/
void ANALOGINPUT_SCAN() {
    unsigned char i = 0;

    for (i = 0; i < 4; i++) {
        //set channel address
        ADMUX = 0x40 + i;
        //start ADC conversion
        ADCSRA = ADCSRA | 0x40;
        //wait until conversion is over
        while (ADCSRA & 0x40)
            ADCSRA = ADCSRA & 0xEF;
        //read the counts from ADC register
        CALIBRATION[ANALOGINPUT1 + (i * 7)] = (ADCL | (ADCH << 8)); //y = 0.000006x2 + 1.0025x + 2.3407		
        /* Add Polynomial function*/
        if (i == 0)CALIBRATION[ANALOGINPUT1 + (i * 7)] = (int) ((Actual_TensionA * (float) CALIBRATION[ANALOGINPUT1 + (i * 7)]*(float) CALIBRATION[ANALOGINPUT1 + (i * 7)])+(Actual_TensionB * (float) CALIBRATION[ANALOGINPUT1 + (i * 7)]) + Actual_TensionC);
        if (i == 1)CALIBRATION[ANALOGINPUT1 + (i * 7)] = (int) ((Set_TensionA * (float) CALIBRATION[ANALOGINPUT1 + (i * 7)]*(float) CALIBRATION[ANALOGINPUT1 + (i * 7)])+(Set_TensionB * (float) CALIBRATION[ANALOGINPUT1 + (i * 7)]) + Set_TensionC);

        if (i > 0) {
            AI_RAWVALUE[i] = (((float) CALIBRATION[ANALOGINPUT1 + (i * 7)] / 1023.0) * ((float) CALIBRATION[AISCALE1 + (i * 7)] / 10.0))+((float) CALIBRATION[AIOFFSET1 + (i * 7)] / 10.0);

            AI_ARRAY[i] = FILTER(AI_RAWVALUE[i], AI_ARRAY[i], AI_FILTER[i]);
            CALIBRATION[AIOUTPUT1 + (i * 7)] = AI_ARRAY[i]*10.0;
            CALIBRATION[FSCALEDVALUE1 + (i * 7)] = (AI_ARRAY[i]*(float) CALIBRATION[AIFULLSCALE1 + (i * 7)]) / 100.0;
        }
    }
    if (CALIBRATION[ABSOLUTEVALUE] == 1) {
        CALIBRATION[AIOUTPUT2] = fabs(CALIBRATION[AIOUTPUT2]);
        CALIBRATION[FSCALEDVALUE2] = (fabs(CALIBRATION[AIOUTPUT2])*(float) CALIBRATION[AIFULLSCALE2]) / 1000.0;
    }
    /*To compensate temperature*/
    TEMPERATUREDRIFT = (Temp_Drift * (((float) CALIBRATION[CALTEMP] / 10.0)-((float) CALIBRATION[FSCALEDVALUE4] / 10.0)));
    /*To compensate power supply*/
    POWERDRIFT = (Supply_Drift * (((float) CALIBRATION[CALEXCITATION] / 10.0)-((float) CALIBRATION[AIOUTPUT3] / 10.0)));

    if (DIRECTION)AI_RAWVALUE[0] = ((((float) CALIBRATION[ANALOGINPUT1]) - 512) / 5.12) + TEMPERATUREDRIFT + POWERDRIFT; //convert -100 - +100%	
    else AI_RAWVALUE[0] = ((((float) CALIBRATION[ANALOGINPUT1]) - 512) / 5.12) - TEMPERATUREDRIFT - POWERDRIFT;

    return;
}

/******************************************PID *********************************************************/
float PID(float Setpt, float FeedBk) {
    float LF_PgainOp = 0.0, LF_Error = 0.0, LF_Output = 0.0;
    float Derivativeop = 0.0;


    if (AUTOMAN == 0) //Auto Mode
    {
        /*If "Pid enable" is true*/
        if (PIDENABLE == 1) {
            LF_Error = (Setpt - fabs(FeedBk)) / 10.0;
            /* Deadband for PID is subtracted from error */
            if (LF_Error >= 0.0) {
                if (LF_Error > DeadBand) LF_Error = LF_Error - DeadBand;
                else LF_Error = 0.0;
            }
            else {
                if (fabs(LF_Error) >= DeadBand) LF_Error = LF_Error + DeadBand;
                else LF_Error = 0.0;
            }

            /* Calculation of P-Gain O/P */
            LF_PgainOp = LF_Error * PGain / 100.0;

            /* If P-Gain exceeds propotional limit, then clamp to Propotional limit */
            if (LF_Error > 0 && LF_PgainOp > PLmt) LF_PgainOp = PLmt;
            else if (LF_Error < 0.0 && LF_PgainOp < Neglmt) LF_PgainOp = Neglmt;

            /* Calculation of integral gain */
            if (ITime == 0.0)
                GFA_IntegGain = 0.0;
            else
                GFA_IntegGain = GFA_IntegGain + (LF_Error * (SCAN_TIME / ITime));

            /* If I-Gain exceeds propotional limit, then clamp to Propotional limit */
            if (LF_Error > 0.0 && GFA_IntegGain > ILmt) GFA_IntegGain = ILmt;
            else if (LF_Error < 0.0 && GFA_IntegGain < Neglmt) GFA_IntegGain = Neglmt;
            /* Calculation of derivative gain*/
            Derivativeop = (DTime / 100.0)*((LF_Error - PreviousErr) / SCAN_TIME);
            PreviousErr = LF_Error;

            /* If D-Gain exceeds propotional limit, then clamp to Propotional limit*/
            if (LF_Error > 0.0 && Derivativeop > DLmt) Derivativeop = DLmt;
            else if (LF_Error < 0.0 && Derivativeop < Neglmt) Derivativeop = Neglmt;

            /* Add proportional and integral component to get final output */
            LF_Output = (GFA_IntegGain + LF_PgainOp + Derivativeop)*10.0;

            /*positive clamp*/
            if (LF_Output > (Positivelmt * 10.0)) {
                LF_Output = Positivelmt * 10.0;
            }
            if (LF_Output < (Neglmt * 10.0)) {
                LF_Output = Neglmt * 10.0;
            }

        }
            /* PID Disable */
        else {
            LF_Output = GFA_IntegGain = Derivativeop = LF_PgainOp = 0.0;

        }
    } else {

        LF_Output = Setpt; //Man mode	
        LF_PgainOp = 0.0;
        GFA_IntegGain = 0.0;
        Derivativeop = 0.0;
    }



    return LF_Output;

}

/********************************Updating analog output and scaling**************************************************/
void UPDATE_AO(void) {
    unsigned char i = 0;
    int chaddress = 0x01;
    float LF_RawOp = 0.0;
    unsigned int VALUE = 0;


    for (i = 0; i < 2; i++) {

        LF_RawOp = CALIBRATION[AO_VALUE_INDEX[CALIBRATION[AOFUNCTION1 + (i * 9)]]]; //Select value which is configured to analog o/p           	  	
        if (CALIBRATION[AOFUNCTION1 + (i * 9)] == 6)LF_RawOp = (LF_RawOp / (float) CALIBRATION[AOFULLSCALE1 + (i * 9)])*1000.0;
        else LF_RawOp = (LF_RawOp / (float) CALIBRATION[AOFULLSCALE1 + (i * 9)])*100.0;
        CALIBRATION[AOOUTPUT1 + (i * 9)] = LF_RawOp;
        LF_RawOp = ((((fabs(LF_RawOp) / 1000.0)*2048.0)*(float) Scale[i] / 1000.0) * TypeScaling[i])+(((float) CALIBRATION[AOOFFSET1 + (i * 9)] / 1000.0)*2048.0);
        LF_RawOp += 2048.0;

        if (LF_RawOp >= 4095.0)LF_RawOp = 4095.0;
        else if (LF_RawOp <= 0.0)LF_RawOp = 0.0;
        LF_FilteredOp[i] = FILTER(LF_RawOp, LF_FilteredOp[i], AO_FILTER[i]);
        CALIBRATION[ANALOGOUTPUT1 + (i * 9)] = VALUE = (unsigned int) LF_FilteredOp[i];
        VALUE = VALUE | (chaddress << 12); //Last four bits-address ,12 bits - data  	
        /*Update DAC*/
        WriteDACData(VALUE, 16);
        chaddress = 0x09;
    }

    return;
}

/*****************************************************************************************************************/
void Lo_Hi_Trip(void) {
    float Lo_Trip = 0.0;
    float Hi_Trip = 0.0;
    float Lo_Tol = 0.0;
    float Hi_Tol = 0.0;



    Hi_Trip = (((float) CALIBRATION[SETVALUE]) + ((((float) CALIBRATION[SETVALUE])* ((float) CALIBRATION[UPLIMIT])) / 1000.0));
    Lo_Trip = (((float) CALIBRATION[SETVALUE]) - ((((float) CALIBRATION[SETVALUE])* ((float) CALIBRATION[LOLIMIT])) / 1000.0));
    Lo_Tol = (Lo_Trip + ((float) CALIBRATION[LOLIMIT]));
    Hi_Tol = (Hi_Trip + ((float) CALIBRATION[UPLIMIT]));

    if (ACT_LOAD > Hi_Trip) {
        PORTC = PORTC & 0XF0;
        PORTC = PORTC | 0X02;
    } else if (ACT_LOAD < Lo_Trip) {
        PORTC = PORTC & 0XF0;
        PORTC = PORTC | 0X01;
    } else if (ACT_LOAD > Lo_Tol && ACT_LOAD < Hi_Tol) {
        PORTC = PORTC & 0XF0;
    }

    return;
}
