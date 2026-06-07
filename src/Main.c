/***************************************************************************
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


volatile static unsigned short ONTIME1_MSB;
volatile static unsigned short OFFTIME1_MSB;
volatile static unsigned short ONTIME1_LSB;
volatile static unsigned short OFFTIME1_LSB;
volatile static unsigned short ONTIME2;
volatile static unsigned short OFFTIME2;
volatile static unsigned short on_time;
volatile static unsigned short off_time;
 
/***********************************************************************************/
void timerAttach( u08 interruptNum, void (*userFunc)(void) )
{
	TimerIntFunc[interruptNum] = userFunc;
}
/********************************************************************
*     Function Name:    Initialisation                              *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This routine is used to initialise Timer,   *
                        IO PORT register                            *  
********************************************************************/
void Initialisation(void)
{
/*  Initialize timer1 */
	TCCR2A=0X00;
/*  Set prescaler is 0 */
	TCCR2B=0x00;
/*  Reset timercount1 */
	TCNT2=0x00;
/*  Enable timer1 match interrupt */						
	TIMSK2=0x02;
	OCR2A=0X00;
	
/*  Initialize timer2
    Set prescaler is 1024 */
	TCCR0B=0x05;
/*  Reset timercount0 */
	TCNT0=0x00;
/*  Enable timer0 overflow interrupt	*/						
	TIMSK0=0x01;

	DDRC   = 0x0F;
	PORTC  = 0x00;
	DDRD   = 0xF2;	
	DDRB=(1<<DDB5)|(1<<DDB7)|(1<<DDB4);           
    PORTD |= 0x00;
	//Intialise ADC
	DDRA   = 0x00;      // 0-i/p
	PORTA  = 0x00; 
	/* Enable ADC */
    ADCSRA = 0x8A;			
   
	/* Enable global interrupt */
	sei();	
	
}
/**********************************pwm&freq initialize***************************/
void pwm_freq_initialize(void)
{
	
if (CALIBRATION[PWM_FREQ_SEL]==3)  
    {
/*  Set prescaler is 8*/
    /*TCCR1B=0X02;*/
/*  Reset timercount1*/
	TCNT1=0X0000;
/*  Enable timer1 overflow interrupt */
	/*TIMSK1=0X01;*/
/*  External interrupt*/
	EIMSK=0X01;
	EICRA=0X03;
	}

	else
    {
/*  Initialize timer1 */
	TCCR1A=0X00;
/*  Set prescaler is 1 */
	TCCR1B=0x01;
/*  Reset timercount1 */
	TCNT1=0x0000;
/*  Enable timer1 match interrupt */					
	/*TIMSK1=0x03;*/
	EIMSK=0X00;
	EICRA=0X00;
	}
}
/**********************************PC Communication****************************************************/
void PC_Communication(void)
{
    /* For PC communication */
	if(SCI_COUNT > 2 )
	{            	
		if(RX_BUSY==0) sci_rx(10);          
		if(DATA_RECEIVED==1) sci_main();     			
		SCI_COUNT=0;
	}
}

/************************************set prescaler**************************************************************/
void prescaler2(void)
{  
  /*if ((float)CALIBRATION[SET_FREQUENCY2]==1){TCCR2B=0x01;PRESCALER_2=1.0;CALIBRATION[FREQUENCY2]=38000;}*/

  if ((float)CALIBRATION[SET_FREQUENCY2]==2){TCCR2B=0x02; PRESCALER_2=8.0;CALIBRATION[FREQUENCY2]=4000;}

  else if ((float)CALIBRATION[SET_FREQUENCY2]==3){TCCR2B=0x03; PRESCALER_2=32.0;CALIBRATION[FREQUENCY2]=999;}

  else if ((float)CALIBRATION[SET_FREQUENCY2]==4){TCCR2B=0x04; PRESCALER_2=64.0;CALIBRATION[FREQUENCY2]=450;}

  else if ((float)CALIBRATION[SET_FREQUENCY2]==5){TCCR2B=0x05; PRESCALER_2=128.0;CALIBRATION[FREQUENCY2]=250;}

  else if ((float)CALIBRATION[SET_FREQUENCY2]==6){TCCR2B=0x06; PRESCALER_2=256.0; CALIBRATION[FREQUENCY2]=131;}

  else if ((float)CALIBRATION[SET_FREQUENCY2]==7){TCCR2B=0x07; PRESCALER_2=1024.0; CALIBRATION[FREQUENCY2]=30;}
  }
/************************************AI1&MMI***********************************************************/
void AI1_MMIcalculation(void)
{
  if (CALIBRATION[CHANNEL1_FREQUENCY]==0){Freq1=(float)CALIBRATION[FSCALEDVALUE1];}
  else Freq1=(float)CALIBRATION[SET_FREQUENCY1];
  if (CALIBRATION[CHANNEL1_DUTYCYCLE]==0){duty1=(float)CALIBRATION[FSCALEDVALUE1];}
  else duty1=(float)CALIBRATION[SET_DUTYCYCLE1]; 
}
  
/***********************************AI2&MMI***************************************************/
void AI2_MMIcalculation(void)
{
  Freq2=CALIBRATION[FREQUENCY2];
  if (CALIBRATION[CHANNEL2_DUTYCYCLE]==0){duty2=(float)CALIBRATION[FSCALEDVALUE2];}
  else duty2=(float)CALIBRATION[SET_DUTYCYCLE2];
}  
/************************************Counts Calculation***********************************************/
void countscalculation_AI1(void)
{	
     u16 on_time,off_time;
     total_time1=(1/Freq1);
	 MAXI_OP1=((((float)CALIBRATION[CHANNEL1_MAXIMUM]-(float)CALIBRATION[CHANNEL1_MINIMUMBAND])/(float)CALIBRATION[CHANNEL1_MAXIMUMBAND]-(float)CALIBRATION[CHANNEL1_MINIMUMBAND])*100.0);
     MINI_OP1=((((float)CALIBRATION[CHANNEL1_MINIMUM]-(float)CALIBRATION[CHANNEL1_MINIMUMBAND])/(float)CALIBRATION[CHANNEL1_MAXIMUMBAND]-(float)CALIBRATION[CHANNEL1_MINIMUMBAND])*100.0);
     DUTYCYCLE=MINI_OP1+((duty1/10.0*((MAXI_OP1-MINI_OP1)/100.0)));
	 CALIBRATION[FREQUENCY1]=Freq1;
	 CALIBRATION[DUTYCYCLE1]=DUTYCYCLE*10.0;

     if (CALIBRATION[INVERT1]==0)
	 {
	  if(DUTYCYCLE>90.0)
	  {
	  TCCR1A=0X80;
	  CALIBRATION[DIGITALOP1]=0;
	  a=0;
	  }	 
	  if(DUTYCYCLE<10.0)
	  {
	  TCCR1A=0XC0;
	  CALIBRATION[DIGITALOP1]=1;
      a=1;
	  }
	 }

     if((DUTYCYCLE>10.0)&(DUTYCYCLE<90.0))
     {
	 TCCR1A=0X43;
	 TIMSK1=0x03;
	 on_time=(u16)floor(((CLOCK_FREQUENCY/PRESCALER1)*(total_time1)*(DUTYCYCLE/100.0)));
	 off_time=(u16)floor(((CLOCK_FREQUENCY/PRESCALER1)*(total_time1)*(100.0-DUTYCYCLE)/100.0));	
	 ONTIME1_LSB = (uint8_t)(on_time & 0X00FF);
     ONTIME1_MSB = (uint8_t)((on_time & 0XFF00)>>8);
     OFFTIME1_LSB = (uint8_t)(off_time & 0X00FF);
     OFFTIME1_MSB= (uint8_t)((off_time & 0XFF00)>>8);
	 if(count1==0)
	 {
	 TCCR1A=0X43;
	 TCCR1B|=0X18;
	 OCR1AH=ONTIME1_MSB;
	 OCR1AL=ONTIME1_LSB;
	 a=1;
	 }
	 count1++;
	 if(count1>100)count1=100.0;	
  
     }
}
/************************************Counts Calculation AI2***********************************************/
void countscalculation_AI2(void)
{
    
     Freq2=((float)CALIBRATION[FREQUENCY2]);
	 total_time2=(1/Freq2);
	 MAXI_OP2=((((float)CALIBRATION[CHANNEL2_MAXIMUM]-(float)CALIBRATION[CHANNEL2_MINIMUMBAND])/(float)CALIBRATION[CHANNEL2_MAXIMUMBAND]-(float)CALIBRATION[CHANNEL2_MINIMUMBAND])*100.0);
     MINI_OP2=((((float)CALIBRATION[CHANNEL2_MINIMUM]-(float)CALIBRATION[CHANNEL2_MINIMUMBAND])/(float)CALIBRATION[CHANNEL2_MAXIMUMBAND]-(float)CALIBRATION[CHANNEL2_MINIMUMBAND])*100.0);
	 DUTYCYCLE3=(MINI_OP2+(duty2/10.0*((MAXI_OP2-MINI_OP2)/100.0)));
	 CALIBRATION[DUTYCYCLE2]=DUTYCYCLE3*10.0;
	 CALIBRATION[FREQUENCY2]=Freq2;

     if (CALIBRATION[INVERT2]==0)
	 {
	  if(DUTYCYCLE3>90.0)
	  {
	  TCCR2A=0X80;
	  }	 
	  if(DUTYCYCLE3<10.0)
	  {
	  TCCR2A=0XC0;
	  }
	 }
     if((DUTYCYCLE3>10.0)&(DUTYCYCLE3<90.0))
	 {
     TCCR2A=0X83;
	 ONTIME2=(uint8_t)floor(((CLOCK_FREQUENCY/PRESCALER_2)*(total_time2)*(DUTYCYCLE3/100.0)));
	 OFFTIME2=(uint8_t)floor(((CLOCK_FREQUENCY/PRESCALER_2)*(total_time2)*((100.0-DUTYCYCLE3)/100.0)));
	 OCR2A=OFFTIME2;
	 if(count2==0)
	 {
     TCCR2A|=0X83;
	 TCCR2B|=0X00;
     OCR2A=OFFTIME2;
	 b=1;
	 }
	 count2++;
	 if(count2>100)count2=100.0;
	 }	 
}
/******************************************************************************************************/
void CallInterruptFn1(void)
{  
	TCNT0 = 0xB1; //10msec	
    /*TCNT0 = 0x9D;*/	  
	switch(CALIBRATION[PWM_FREQ_SEL])
	{
	    case 1:  /* Reading analog inputs */
				ANALOGINPUT_SCAN();
				CALIBRATION[CONTINUOUSDISPLAY]=2;
				break;
        case 2:/* Reading analog inputs */
				ANALOGINPUT_SCAN();
				CALIBRATION[CONTINUOUSDISPLAY]=1;
			    break;
		case 3:/* Reading analog inputs */
				ANALOGINPUT_SCAN();
				CALIBRATION[CONTINUOUSDISPLAY]=9;  
				/*speed calculation*/
				speed_calculation();
				LENGTH=Storedcnt+(Lengthcnt/CALIBRATION[ENCPPR]);
				break;  
         default:break;
    }	
	/* serial download function*/
	SCI_DOWNLOAD();	
		 
	KEYCOUNT++;	

	DELAY_RESPONSE++;		
	
    if(CURSOR==TRUE)Displaycount++;	
	else Displaycount=0;

	if(FAULT>0)Messagecount++;
}  
/********************************************************************
*     Function Name:    Main                                        *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Main Routine                                *  
********************************************************************/
int main()
{ 
    nested_delay(1000,500); 
  /*uart initialisation*/
  	uartInit();
    nested_delay(1,100); 
  /*timer initialisation*/
	Initialisation();
    nested_delay(1,100);	
  /*Intialise calibration array */
	initialize_variable();		
  /* Attach timer2 interrupt function */
  	timerAttach(TIMER2OVERFLOW_INT,CallInterruptFn1);		
  /*Reading initial status of Push to zero & Push to cal digital input */
	PREVVALUE=((PINC&0x80)>>7); //zero
	PREVIOUSVAL=((PINC&0x40)>>6);//CAL	
	nested_delay(10,100);	
  /*Read values from memory*/
 	Read_memory();
	nested_delay(10,100);
  /*Led initialisation*/
    LED_Init(); 	
    DIGITS=OPMENU_PAGE=CALIBRATION[CONTINUOUSDISPLAY];
  /* Operator menu first page display*/
	MSG_HANDLER();  
	CALIBRATION[FACTORYSET]=0;
	SAVE=1;	
    pwm_freq_initialize();
	for(;;)
	{
	    /* 40msec */	 
		if(KEYCOUNT>=4)
		{			
			/* Update PID variables and speed constants */
		    Value_updation();				
			/* Update Polynomial co efficient */
			UpdatePolynomial_Coeff();
			/*AI1&MMI calculation*/
		   

			if(CALIBRATION[PWM_FREQ_SEL]==3) 
			{
				Update_Speedconstants(); //updating constants used in speed calculation
            }
			else
			{
			 prescaler2();
			 AI1_MMIcalculation();
			/*AI2&MMI calculation*/
		    AI2_MMIcalculation();
			/*Counts initilisation AI1*/
			countscalculation_AI1();
		    /*Counts initilisation AI2*/
	        countscalculation_AI2();
			}		    
			/* Serial communication */
			PC_Communication();	
			/* Display function */
			if(ENTER_KEYCOUNT>1)split_numeric(fabs(CALIBRATION[SETUP_INDEX]));
			DISPLAY_VALUE();
			/* Key functions */		
			SCAN_KEYS();
			/* Set factory set values */
			if(CALIBRATION[FACTORYSET]>0 && CURSOR==FALSE)
			{
				cli();
				initialize_variable();
				save_parameters();
				CALIBRATION[FACTORYSET]=0;
			    sei();												
			}		
			KEYCOUNT=0;
   		}	 	
	}
return 0 ;
}
/*******************************************************************************
*     Function Name:    Timer1 Interrupt Handler                               *
*     Description:      Timer1 match Interrupt routine                      *  
*******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1A)													
{  																		
   	if(a==0)
	{
    if (CALIBRATION[INVERT1]==0)
	{
     OCR1AH=ONTIME1_MSB;
	 OCR1AL=ONTIME1_LSB;
	 }
	 else if (CALIBRATION[INVERT1]==1)
	 {
      OCR1AH=OFFTIME1_MSB;
	  OCR1AL=OFFTIME1_LSB;
	  }
	a=1;
	}
	else 

	{
    if (CALIBRATION[INVERT1]==0)
	{
     OCR1AH=OFFTIME1_MSB;
	 OCR1AL=OFFTIME1_LSB;
	 }
	 else if (CALIBRATION[INVERT1]==1)
	 {
	 OCR1AH=ONTIME1_MSB;
	 OCR1AL=ONTIME1_LSB;
	 }

	a=0;
	}
   
}
/*******************************************************************************
*     Function Name:    Timer1 Interrupt Handler                               *
*     Description:      Timer1 overflow Interrupt routine - 30msec              *  
*******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{ 
   sei();
   if (CALIBRATION[PWM_FREQ_SEL]==3) Overflow_count++; 	
}
/*******************************************************************************
*     Function Name:    Timer1 Interrupt Handler                               *
*     Description:      Timer1 match Interrupt routine                      *  
*******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2A)
{
    /* set global interrupt */
/*	OCR2A=OFFTIME2;*/
	if(b==0)
	{
    if (CALIBRATION[INVERT2]==0)
	{
     OCR2A=OFFTIME2;
	 	 }
    b=1;
	}
	else if (CALIBRATION[INVERT2]==0)
	{
     OCR2A=ONTIME2;	
	 }
	b=0;
}
/*******************************************************************************
*     Function Name:    Timer1 Interrupt Handler                               *
*     Description:      Timer1 overflow Interrupt routine - 30msec              *  
*******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2)
{
   /* set global interrupt */
   sei();	
}
/*******************************************************************************
*     Function Name:    Timer2 Interrupt Handler                               *
*     Description:      Timer2 compare Interrupt routine - 30msec              *  
*******************************************************************************/
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
   /* set global interrupt */
   sei(); 
   if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT](); 
      	
}

/*************************************************************************************************/
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT0)
{ 
        if (CALIBRATION[PWM_FREQ_SEL]==3)
		{
		/*Pulse_Rx=0;	Savecnt=0;
		Count_Saving();*/
		TIMSK1=0X01;
		TCCR1B=0X02;
		/*EIMSK=0X01;
	    EICRA=0X03;*/
		if(CALIBRATION[SPEEDMETHOD]==2) 
		{ 	    
			/* calculate speed based on no of pulses and time period */
			Pulsecount++;
			CALIBRATION[PULSE]=Pulsecount;
			if(Pulsecount>=CALIBRATION[PULSES])
			{ 
			/* for calculate the speed take the timer1 count */
			Totalcounts = TCNT1;	
			CALIBRATION[TT]=Totalcounts;
			/* take the timer1 over flow interrupt count */
			Overflow_count1 = Overflow_count;
			/* reset the timer1 interrupt count */
			Overflow_count=0;	 
			TCNT1 = 0x0000;	
			/* enable the speed calculation */
			Received = 1;	 
			Pulsecount=0;
			}
			}		
	    
	    else No_Of_Pulse++;	/* calculate speed based on edge counting */ 
	}	   
}
/*************************************************************************************************/
SIGNAL(SIG_ADC)
{
}
/*************************************************************************************************/

