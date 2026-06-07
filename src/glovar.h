/******************************************************************************
		   Filename     : glovar.c     
		   Author       : Manjula P
		   Last Modified: 5/12/2009                                           
		   Description  : Global variables are defined                 
******************************************************************************/
extern void uartInit(void);
extern void uartSend(unsigned char);
extern void uartStringSend(unsigned char[255]); 
/*****************************************************************************/
#ifndef EXTINT_INTERRUPT_HANDLER
#define EXTINT_INTERRUPT_HANDLER	SIGNAL
#endif
#ifndef USART_RECEIVE_INTERRUPT_HANDLER
#define USART_RECEIVE_INTERRUPT_HANDLER	SIGNAL
#endif
/*****************************************************************************/
/************************ Define keys******************************************/

#define		ENTER_KEY		0x07  //0111
#define		CURSOR_KEY		0x0B  //1011
#define		MINUS_KEY		0x0D  //1101
#define		PLUS_KEY		0x0E  //1110
#define		ESC_KEY			0x0C  //1100
#define		NO_KEY			0x0F  //1111
#define     SET_KEY         0x06  //0110

#define    MPASSWORD      1 
#define    AI_SCANTIME    10  //msec
#define    SCAN_TIME      0.010     //10msec
#define    TIMESEC        20

#define    VERSION        1.10
#define    CLOCK_FREQUENCY1    10      //MHz
/* Polynomial co-efficients for Actual tension */
#define    ACT_TENSIONA      0.0000001  //a - 7 precision
#define    ACT_TENSIONB      0.0001     //b - 4 precision
#define    ACT_TENSIONC      0.0001     //c - 4 Precision
/* Polynomial co-efficients for set tension */
#define    SET_TENSIONA      0.0000001  //a - 7 Precision
#define    SET_TENSIONB      0.0001     //b - 4 Precision
#define    SET_TENSIONC      0.0001     //c - 4 Precision

unsigned char READ_CHAR[220];
unsigned char DATA_RECEIVED=0,READ_CODE=0,RX_BUSY=0,DNLD_CODE=0;
unsigned char Received=2;
int STEPS=0,x=0;
int SCI_COUNT=0;

int CALIBRATION[200];
int Scale[2]={0,0};
int G_F_WAIT_TIME=0;
int KEYCOUNT=0;
int DELAY_RESPONSE= 0;
int CURSOR_POSITION=0;
int PEAKPERIOD=0;
int STARTINGDELAY=0;
int POSTDELAY=0;
int Time_Count=0;
int Pulsecount=0;
int Memorycount=0;
int Displaycount=0;
int Messagecount=0;
int PASS=0;
int Opmenucount=0;
int MULTIPLE=0;
int KEY=0;
int MAXVALUE=0;
int MIN=0;
int PREVIOUSPAGE=-1;
int ZEROCOUNT=0;
int CALCOUNT=0;
int COUNTS=157;
int Delaycount=0,Increment=0;
int Lengthcnt=0;
int Savecnt=0;
int Pulse_Rx=0;
int CountRx=0;
int Prev_count=-1;
int Storedcnt=0;
unsigned int LENGTH=0;


unsigned char numeric[5]={0,0,0,0,0};
              //DATADISPLAY[30]={0,1,2,3,4,5,6,7,8,9,F,1,-,S,S,F,2,- ,P,0,F ,3 ,-, P , C ,F ,4 ,-, P , n, - , ON CA5-A , ON CA5-B , ON CA5-C, OFF}
unsigned char DATADISPLAY[40]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x71,0x06,0x40,0x6D,0x6D,0x71,0x5B,0x40,0x73,0x3F,0x71,0x4F,0x40,0x73,0x39,0x71,0x66,0x40,0x73,0x37,0x40,0x01,0x02,0x04,0x00,0x73};
unsigned char SAVE=0;
unsigned char KEY_CONT_PRESS=0;
unsigned char KEY_PRESSED=0;
unsigned char KEY_OPRN_OVER=0;
unsigned char MAX_CUR_POS=0;
unsigned char SETPAGE=0;
unsigned char MAX_PAGE=12; 
unsigned char KEY_DEBOUNCE=0;
unsigned char CURSOR_CNT=0;
unsigned char DIGITS=1;
unsigned char OPMENU_PAGE=1;
unsigned char DIRECTION=1;
unsigned char SETUP_INDEX=0;
unsigned char DECIMAL_POINT=1;
unsigned char PEAKDETECT=0;
unsigned char FAULT=0;
unsigned char ENTER_KEYCOUNT=2;
unsigned char CURSOR=0;
unsigned char PREVVALUE=0;
unsigned char PREVIOUSVAL=0;
unsigned char AUTOMAN=0;
unsigned char PEAKRESET=0;
unsigned char PIDENABLE=0;
unsigned char PREVDI2=2;
unsigned char PREVDI1=2;
unsigned char PEAKSTOP=1;
unsigned char PREVPAGE=1;
unsigned char PREVCOUNT=0;
unsigned char PREVENTERCOUNT=1;
unsigned char SIGN=0;
unsigned char PUSHTOZERO=0;
unsigned char PUSHTOCAL=0;
unsigned char Weightsmall=0;
unsigned char PrevDP=-1;
unsigned char a=0;
unsigned char b=0;



float DP_Scale[3]={1.0,10.0,100.0};
float AI_ARRAY[4]={0.0,0.0,0.0,0.0};
float AI_RAWVALUE[4]={0.0,0.0,0.0,0.0};
float AI_FILTER[4]={0.0,0.0,0.0,0.0};
float AO_FILTER[2]={0.0,0.0};
float TypeScaling[2]={0.0,0.0};
float TEMPERATUREDRIFT=0.0;
float POWERDRIFT=0.0;
float Fm=0.0,Rm=0.0;
float LF_FilteredOp[2]={0.0,0.0};
float ForwardOffset=0.0;	 
float ACT_LOAD = 0.0;
float SET_LOAD = 0.0;
float Actual_TensionA=0.0;
float Actual_TensionB=0.0;
float Actual_TensionC=0.0;
float Set_TensionA=0.0;
float Set_TensionB=0.0;
float Set_TensionC=0.0;
float AFull_Scale=0.0;
float PreviousErr=0.0;
float PGain=0.0;
float ITime=0.0;
float PLmt=0.0;
float DeadBand=0.0;
float DTime=0.0;
float ILmt=0.0;
float DLmt=0.0;
float Positivelmt=0.0;
float Neglmt=0.0;
float Analogoutput_speed=0.0;
float Temp_Drift=0.0;
float Supply_Drift=0.0;
float GFA_IntegGain=0.0;

float Speed_Constant=0.0;
float Freq_Scale=0.0;
float Freq_Constant=0.0;
float Sampling_Freq=0.0;
float Frequency=0.0;
float SpeedRpm=0.0;
float FilteredFreq=0.0;
float Filter_Coeff=0.0;
float PRESCALER_2=0.0;
float total_time1=0.0;
float CLOCK_FREQUENCY=8000000.0;
float DUTYCYCLE=0.0;
float PRESCALER1=1.0;
float Freq1=0.0;
int count1=0;
float MAXI_OP1=0.0;
float MINI_OP1=0.0;
float duty1=0.0;

float total_time2=0.0;
float DUTYCYCLE3=0.0;
int count2=0;
float Freq2=0.0;
float MAXI_OP2=0.0;
float MINI_OP2=0.0;
float duty2=0.0;
float d=0;

unsigned int Overflow_count=0;
unsigned int Overflow_count1=0;
unsigned int Totalcounts=0;
unsigned int No_Of_Pulse=0;
unsigned int PREVIOUSVALUEA=0;
unsigned int TotalPulse_Rx=0;
unsigned int Forward_Slope=0;
unsigned int Reverse_Slope=0;
unsigned int Memorycount_Sec=0; 
unsigned int intcount=0;

/********************************************************************************/
enum ObjectID 
{
 
ALARM,//0
FREQUENCY1,//1
DUTYCYCLE1,
SET_FREQUENCY1,

SET_DUTYCYCLE1,
FREQUENCY2,
DUTYCYCLE2,
SET_FREQUENCY2,

SET_DUTYCYCLE2,//8
FREQUENCY3,
SPEED3,
SPARE1,//11

PARAMETERLOCK,
ACCESSCODE,//13
SPARE2,
RESPONSE,

SPARE3,
SPARE4,//17
PWM_FREQ_SEL,
CONTINUOUSDISPLAY,

DIGITALOP1,
FRE,//21
FRE_1,
FIL,

TT,
PULSE,
SPARE11,
SPARE12,//27

CHANNEL1_FREQUENCY,//28
CHANNEL1_DUTYCYCLE,
CHANNEL1_MINIMUM,
CHANNEL1_MAXIMUM,

CHANNEL1_MINIMUMBAND,
CHANNEL1_MAXIMUMBAND,
INVERT1,
SPARE14,//35

SPARE15,
CHANNEL2_FREQUENCY,
CHANNEL2_DUTYCYCLE,
CHANNEL2_MINIMUM,//39

CHANNEL2_MAXIMUM,
CHANNEL2_MINIMUMBAND,
CHANNEL2_MAXIMUMBAND,
INVERT2,

SPARE17,//44
SPARE18,
SPARE19,
SPARE20,

SPARE21,
SPARE22,
SPARE23,
SPARE24,

SPARE25,//52
SPARE26,
SPARE27,
SPARE28,//55

SPARE29,
SPARE30,
SPARE31,//58
SPARE32,
SPARE33,
SPARE34,
SPARE35,//62
SPARE36,

SPARE37,
SPARE38,
SPARE39,
SPARE40,//67

SPARE41,
SPARE42,
ANALOGINPUT1,//70
AIOFFSET1,

AISCALE1,
AIRESPONSE1,  //73
AIFULLSCALE1,
AIOUTPUT1,

FSCALEDVALUE1,
ANALOGINPUT2,
AIOFFSET2,
AISCALE2,//79

AIRESPONSE2, 
AIFULLSCALE2,
AIOUTPUT2,
FSCALEDVALUE2,

SPARE43,
SPARE44,
SPARE45,
SPARE46,  //87

SPARE47,
SPARE48,
SPARE49,
SPARE50,

SPARE51,
SPARE52,//93
SPARE53, 
SPARE54,

SPARE55,
SPARE56,
SPARE57,//98
SPARE58,

SPARE59,
SPARE60,
SPARE61,//102
SPARE62, 
 
SPARE63,
SPARE64,
SPARE65,
SPARE66,

SPARE67,
SPARE68, //109 
SPARE69,
SPARE70,

SPARE71,
SPARE72,
SPARE73,
SPARE74,

SPARE75,
SPARE76, //117
SPARE77,
SPARE78,

SPARE79,//120
SPARE80,
SPARE81,
SPARE82,

SPARE83,
SPARE84,
SPARE85,
SPARE86,//127

SPARE87,
SPARE88,
SPARE89,
SPARE90,

SPARE91,
SPARE92,
SPARE93,
SPARE94,//135

SPEEDMETHOD,//136
ENCPPR,
SPARE95,
SPDRESPONSE,

SAMPLINGINTERVAL,
FREQMULTIPLIER,
PULSES,
ZERODELAY,//143

MAXSPEED,
MINCOUNT,
SPDOUTPUT,
SPARE96,//147

ABSOLUTEVALUE,//148
DPSELECT,
VERSION_NUMBER,
FACTORYSET,

SETA,//152
SETB,
SETC,
ACTUALA,

ACTUALB,
ACTUALC,
TEMPDRIFT,//158
SUPPLYDRIFT,

PASSWORD,//160


};  
/*****************************************************************************************************/
int AO_VALUE_INDEX[16]={0,AIOUTPUT2,AIOUTPUT1,FSCALEDVALUE1,CHANNEL2_MAXIMUMBAND,0,SPDOUTPUT,SET_DUTYCYCLE1,SET_DUTYCYCLE2,0,0,0,0,0,0};

/*****************  Write the setup parameters to EEPROM ************************/

uint16_t ee_setup __attribute__((section(".eeprom"))) = 42;

/********************************************************************************/







