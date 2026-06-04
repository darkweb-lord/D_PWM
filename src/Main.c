/***************************************************************************
               Filename     : Main.C     
               Author       : SHYAM GANESH K S 
               Last Modified: 05/06/2026                                    
               Description  : Main function & Timer Routines are defined
               Purpose : for HAL Lucknow L3089 PWM with variable frequency
 ****************************************************************************
               Filename     : Main.C     
               Author       : Manjula P
               Last Modified: 05/05/2010                                    
               Description  : Main function & Timer Routines are defined 
 ****************************************************************************/

/*  include I/O definitions (port names, pin names, etc) */
#include "avr/io.h"

/*  include interrupt header files */
#include "avr/interrupt.h"

/*  include EEPROM header files */
#include "avr/eeprom.h"

/*  include basic mathematics constants and functions.*/
#include "math.h"

#include "string.h"

/*  include timer header files */
#include "timer.h"

/*  include global variables */
#include "glovar.h"

/*  Include spi functions */
#include "Writespi.h"

#include "Init_Variables.h"

#include "Update_Var.h"

/*  include display functions */
#include "led.h"

/*  include key functions  */
#include "scankeys.h"

/*  include sci function */
#include "Sci.h"

/*  include control function */
#include "Control.h"

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

/***********************************************************************************/
void timerAttach(u08 interruptNum, void (*userFunc)(void)) {
    TimerIntFunc[interruptNum] = userFunc;
}

/********************************************************************
 *     Function Name:    Initialisation                              *
 *     Return Value:     void                                        *
 *     Parameters:       void                                        *
 *     Description:      This routine is used to initialise Timer,   *
                        IO PORT register                            *  
 ********************************************************************/
void Initialisation(void) {

    /*  Initialize timer1
     *  Set prescaler is 1 */
    TCCR1B = 0x01;
    /*  Reset timercount1 */
    TCNT1 = 0x00;
    /*  Enable timer1 overflow interrupt */
    TIMSK1 = 0x01;

    /*  Initialize timer2
        Set prescaler is 1024 */
    TCCR2B = 0x07;
    /*  Reset timercount0 */
    TCNT2 = 0x00;
    /*  Enable timer0 overflow interrupt	*/
    TIMSK2 = 0x01;

    DDRC = 0x0F;
    PORTC = 0x00;
    DDRD = 0xF2;
    DDRB = (1 << DDB5) | (1 << DDB7) | (1 << DDB4);
    PORTD |= 0x20;
    //Intialise ADC
    DDRA = 0x00; // 0-i/p
    PORTA = 0x00;
    /* Enable ADC */
    ADCSRA = 0x8A;
    /* set rising edge sense */
    EICRA = 0x03;
    /* Enable external interrupt */
    EIMSK = EIMSK | 0x01;
    /* Enable global interrupt */
    sei();

}

/**********************************PC Communication****************************************************/
void PC_Communication(void) {
    /* For PC communication */
    if (SCI_COUNT > 2) {
        if (RX_BUSY == 0) sci_rx(10);
        if (DATA_RECEIVED == 1) sci_main();
        SCI_COUNT = 0;
    }
}

/**********************************Enable Peak capturing**************************************************/
void Peak_Update(void) {

    if (PEAKRESET == 1) {
        if (CALIBRATION[PEAKRESETTYPE] == 0 || CALIBRATION[PEAKRESETTYPE] == 1) {
            CALIBRATION[PEAK] = CALIBRATION[REVERSEPEAK] = CALIBRATION[FORWARDPEAK] = PREVIOUSVALUEA = PEAKPERIOD = PEAKDETECT = STARTINGDELAY = PEAKRESET = POSTDELAY = 0;
        }//Reset Peak  
        else {
            if (POSTDELAY >= (CALIBRATION[PEAKPOSTDELAY]*100))CALIBRATION[PEAK] = CALIBRATION[REVERSEPEAK] = CALIBRATION[FORWARDPEAK] = PREVIOUSVALUEA = PEAKPERIOD = PEAKDETECT = STARTINGDELAY = PEAKRESET = POSTDELAY = 0; //Reset Peak  
        }
    } else {
        switch (CALIBRATION[PEAKCAPMETHOD]) {


            case 1: //Time based
                if (STARTINGDELAY >= (CALIBRATION[PEAKPREDELAY] / 10)) // Starting delay to capture the peak after reset
                {
                    if (PEAKPERIOD <= (CALIBRATION[PEAKCAPTUREPERIOD]*100))PEAKDETECT = 1; // Time period to how far the peak should be captured
                    else PEAKDETECT = 0;
                } else {
                    PEAKPERIOD = 0;
                    PEAKDETECT = 0;
                }
                break;

            case 2: //level based
                if (PEAKSTOP == 0)PEAKDETECT = 1;
                else PEAKPERIOD = STARTINGDELAY = PEAKDETECT = 0;
                break;

            case 3: //Time & level based
                if (STARTINGDELAY >= (CALIBRATION[PEAKPREDELAY] / 10)) // Starting delay to capture the peak after reset
                {
                    if ((PEAKPERIOD <= (CALIBRATION[PEAKCAPTUREPERIOD]*100)) && PEAKSTOP == 0)PEAKDETECT = 1; // Time period to how far the peak should be captured
                    else PEAKDETECT = 0;
                } else {
                    PEAKPERIOD = 0;
                    PEAKDETECT = 0;
                }
                break;
            case 4: //Time & Position based

                if (STARTINGDELAY >= (CALIBRATION[PEAKPREDELAY] / 10)) // Starting delay to capture the peak after reset
                {
                    if ((PEAKPERIOD <= (CALIBRATION[PEAKCAPTUREPERIOD]*100)) && CALIBRATION[FSCALEDVALUE2] >= CALIBRATION[MINPOSITION] && CALIBRATION[FSCALEDVALUE2] <= CALIBRATION[MAXPOSITION])PEAKDETECT = 1; // Time period to how far the peak should be captured
                    else PEAKDETECT = 0;
                } else {
                    PEAKPERIOD = 0;
                    PEAKDETECT = 0;
                }
                break;
            case 5: //Time,level & Position based

                if (STARTINGDELAY >= (CALIBRATION[PEAKPREDELAY] / 10)) // Starting delay to capture the peak after reset
                {
                    if ((PEAKPERIOD <= (CALIBRATION[PEAKCAPTUREPERIOD]*100)) && PEAKSTOP == 0 && CALIBRATION[FSCALEDVALUE2] >= CALIBRATION[MINPOSITION] && CALIBRATION[FSCALEDVALUE2] <= CALIBRATION[MAXPOSITION])PEAKDETECT = 1; // Time period to how far the peak should be captured
                    else PEAKDETECT = 0;
                } else {
                    PEAKPERIOD = 0;
                    PEAKDETECT = 0;
                }
                break;

            default: PEAKPERIOD = PEAKDETECT = STARTINGDELAY = 0;
                break;
        }
    }

}

/******************************Capturing Peak values**************************************************/
void Peak_Capturing(void) {
    if (PEAKDETECT == 1) // Time period to capture the peak
    {
        if ((unsigned int) fabs(ACT_LOAD) > PREVIOUSVALUEA) {
            PREVIOUSVALUEA = (unsigned int) fabs(ACT_LOAD);
            if (DIRECTION)CALIBRATION[PEAK] = CALIBRATION[FORWARDPEAK] = ((float) PREVIOUSVALUEA * DP_Scale[CALIBRATION[DPSELECT]]) / 10.0;
            else CALIBRATION[PEAK] = CALIBRATION[REVERSEPEAK] = ((float) PREVIOUSVALUEA*-1.0 * DP_Scale[CALIBRATION[DPSELECT]]) / 10.0;
        }
    }
}

/*****************************Update Actual Tension***************************************************/
void Update_actualvalues(void) {


    if (DIRECTION) ACT_LOAD = -(AI_RAWVALUE[0]-(ForwardOffset / 10.0)) * Fm;
    else ACT_LOAD = -(AI_RAWVALUE[0]-(ForwardOffset / 10.0)) * Rm;


    if (ACT_LOAD >= (99999 / DP_Scale[CALIBRATION[DPSELECT]]))ACT_LOAD = 99999 / DP_Scale[CALIBRATION[DPSELECT]];
    if (ACT_LOAD <= -(99999 / DP_Scale[CALIBRATION[DPSELECT]]))ACT_LOAD = -99999 / DP_Scale[CALIBRATION[DPSELECT]];

    if (CALIBRATION[ABSOLUTEVALUE] == 1)ACT_LOAD = fabs(ACT_LOAD);

    CALIBRATION[AIOUTPUT1] = (ACT_LOAD * 100.0 * DP_Scale[CALIBRATION[DPSELECT]]) / (float) CALIBRATION[MAXTENSION]; // in terms of %
    AI_ARRAY[0] = FILTER(ACT_LOAD, AI_ARRAY[0], AI_FILTER[0]); // Apply filter to actual tension display
    /* PID function */
    if (CALIBRATION[AOFUNCTION1] == 5 || CALIBRATION[AOFUNCTION2] == 5)CALIBRATION[PIDOUTPUT] = PID(CALIBRATION[AIOUTPUT2], CALIBRATION[AIOUTPUT1]);
    CALIBRATION[PIDERROR] = CALIBRATION[AIOUTPUT2] - fabs(CALIBRATION[AIOUTPUT1]);


}

/**********************************Update operator menu Display******************************************/
void OPmenu_display_Updation(void) {
    float Power = 0.0;
    /* Update actual,set values and error for every response time(ms)*/
    if (DELAY_RESPONSE >= (CALIBRATION[RESPONSE] / 10)) {
        /* Actual Tension */
        CALIBRATION[ACTUALLOAD] = (int) ((AI_ARRAY[0] * DP_Scale[CALIBRATION[DPSELECT]]) / 10.0); //kg/n/lb

        /* Set tension */
        CALIBRATION[SETLOAD] = CALIBRATION[FSCALEDVALUE2];

        /* Error output % */
        CALIBRATION[ERROR] = CALIBRATION[AIOUTPUT2] - fabs(CALIBRATION[AIOUTPUT1]);

        /* Temperature in deg (Temp sesor will produce 5v (100%) output for 110 deg full scale) */
        CALIBRATION[TEMPERATURE] = CALIBRATION[FSCALEDVALUE4];
        Power = ((float) CALIBRATION[ACTUALLOAD]*(float) CALIBRATION[ACTUALSPEED]) / 973.0;
        if (Power >= 9999)Power = 9999;
        else if (Power <= -9999)Power = -9999;
        CALIBRATION[POWER] = Power;

        DELAY_RESPONSE = 0;
    }
}

/******************************************************************************************************/
void CallInterruptFn1(void) {
    TCNT2 = 0xB1; //10msec	
    //TCNT2 = 0x9D;	  
    switch (CALIBRATION[CONTROLLER]) {
        case 1: /* Reading analog inputs */
            ANALOGINPUT_SCAN();

            /* Update actual tension */
            Update_actualvalues();

            CALIBRATION[CONTINUOUSDISPLAY] = 1;

            break;
        case 2:/* Reading analog inputs */
            ANALOGINPUT_SCAN();

            /* Update actual tension */
            Update_actualvalues();

            /* Force(peak) calculation */
            Peak_Capturing();

            /* counting time for peak capturing */
            if (CALIBRATION[PEAKPREDELAY] > 0)STARTINGDELAY++;
            if (CALIBRATION[PEAKCAPTUREPERIOD] > 0)PEAKPERIOD++;
            if (CALIBRATION[PEAKPOSTDELAY] > 0 && PEAKRESET == 1)POSTDELAY++;

            CALIBRATION[CONTINUOUSDISPLAY] = 4;

            break;

        case 5: /* Reading analog inputs */
            ANALOGINPUT_SCAN();

            /* Update actual tension */
            Update_actualvalues();

            /* Force(peak) calculation */
            Peak_Capturing();

            /* counting time for peak capturing */
            if (CALIBRATION[PEAKPREDELAY] > 0)STARTINGDELAY++;
            if (CALIBRATION[PEAKCAPTUREPERIOD] > 0)PEAKPERIOD++;
            if (CALIBRATION[PEAKPOSTDELAY] > 0 && PEAKRESET == 1)POSTDELAY++;

            /* calculate speed using 2 methods */
            speed_calculation();
            LENGTH = Storedcnt + (Lengthcnt / CALIBRATION[ENCPPR]);
            //Pulse_Rx=1;
            /* Saving length during power failure */
            if (CALIBRATION[AIOUTPUT3] < CALIBRATION[THRESHOLD] && SAVE > 0 && CALIBRATION[AIOUTPUT3] > 10) {
                if (fabs(LENGTH - Storedcnt) > 0)eeprom_write_word((&ee_setup + 330), LENGTH);
                SAVE = 0;
            }
            CALIBRATION[LENGTHCOUNT] = LENGTH;
            break;

        default:break;
    }
    /* serial download function*/
    SCI_DOWNLOAD();
    /* Analog output function */
    UPDATE_AO();

    KEYCOUNT++;

    DELAY_RESPONSE++;

    if (CURSOR == TRUE)Displaycount++;
    else Displaycount = 0;

    if (FAULT > 0)Messagecount++;
}

/********************************************************************
 *     Function Name:    Main                                        *
 *     Return Value:     void                                        *
 *     Parameters:       void                                        *
 *     Description:      Main Routine                                *  
 ********************************************************************/
int main() {
    nested_delay(1000, 500);
    /*uart initialisation*/
    uartInit();
    nested_delay(1, 100);
    /*timer initialisation*/
    Initialisation();
    nested_delay(1, 100);
    /*Intialise calibration array */
    initialize_variable();
    /* Attach timer2 interrupt function */
    timerAttach(TIMER2OVERFLOW_INT, CallInterruptFn1);
    /*Reading initial status of Push to zero & Push to cal digital input */
    PREVVALUE = ((PINC & 0x80) >> 7); //zero
    PREVIOUSVAL = ((PINC & 0x40) >> 6); //CAL	
    nested_delay(10, 100);
    /*Read values from memory*/
    Read_memory();
    nested_delay(10, 100);
    /*Led initialisation*/
    LED_Init();
    DIGITS = OPMENU_PAGE = CALIBRATION[CONTINUOUSDISPLAY];
    /* Operator menu first page display*/
    MSG_HANDLER();
    CALIBRATION[FACTORYSET] = 0;
    PREVDI2 = CALIBRATION[DIGITALINPUT2];
    SAVE = 1;

    for (;;) {
        /* 40msec */
        if (KEYCOUNT >= 4) {

            /* Read digital inputs */
            LOADCELL_CALIB();
            /* Configure digital inputs*/
            Digitalinput_Function();
            /* Update PID variables and speed constants */
            Value_updation();
            /* Update Polynomial co efficient */
            UpdatePolynomial_Coeff();
            /* Update analog input values */
            OPmenu_display_Updation();

            if (CALIBRATION[CONTROLLER] == 2 || CALIBRATION[CONTROLLER] == 5) {
                /* Update Peak values */
                Peak_Update();
            }
            if (CALIBRATION[CONTROLLER] == 5) {
                Update_Speedconstants(); //updating constants used in speed calculation
            }

            /* Serial communication */
            PC_Communication();
            /* Display function */
            if (ENTER_KEYCOUNT > 1)split_numeric(fabs(CALIBRATION[SETUP_INDEX]));

            DISPLAY_VALUE();
            /* Key functions */
            SCAN_KEYS();
            /* Set factory set values */
            if (CALIBRATION[FACTORYSET] > 0 && CURSOR == FALSE) {
                cli();
                initialize_variable();
                save_parameters();
                CALIBRATION[FACTORYSET] = 0;
                sei();
            }
            /* Select Analog output range */
            AO_Rangeselect();
            KEYCOUNT = 0;
        }
    }
    return 0;
}

/*******************************************************************************
 *     Function Name:    Timer1 Interrupt Handler                               *
 *     Description:      Timer1 overflow Interrupt routine                      *  
 *******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1) {
    /* set global interrupt */
    sei();
    if (CALIBRATION[CONTROLLER] == 5)Overflow_count++;
}

/*******************************************************************************
 *     Function Name:    Timer2 Interrupt Handler                               *
 *     Description:      Timer2 compare Interrupt routine - 30msec              *  
 *******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2) {
    /* set global interrupt */
    sei();
    if (TimerIntFunc[TIMER2OVERFLOW_INT])
        TimerIntFunc[TIMER2OVERFLOW_INT]();

}

/********************EXTERNEL INTERRUPT 0 ROUTINE************************************************/
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT0) {
    if (CALIBRATION[CONTROLLER] == 5) {
        Lengthcnt++;
        /*Pulse_Rx=0;	Savecnt=0;
        Count_Saving();*/
        if (CALIBRATION[SPEEDMETHOD] == 2) {
            /* calculate speed based on no of pulses and time period */
            Pulsecount++;
            if (Pulsecount >= CALIBRATION[PULSES]) {
                /* for calculate the speed take the timer1 count */
                Totalcounts = TCNT1;
                /* take the timer1 over flow interrupt count */
                Overflow_count1 = Overflow_count;
                /* reset the timer1 interrupt count */
                Overflow_count = 0;
                TCNT1 = 0x0000;
                /* enable the speed calculation */
                Received = 1;
                Pulsecount = 0;
            }
        } else No_Of_Pulse++; /* calculate speed based on edge counting */
    }

}

/*************************************************************************************************/
SIGNAL(SIG_ADC) {
}
/*************************************************************************************************/

