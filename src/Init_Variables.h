/***************************************************************************
			   Filename     : Init_Variables.h    
			   Author       : Manjula P
			   Last Modified: 29/01/2010                                      
			   Description  : Values are initialised    
**********************************************************************************/
/**************************Read values from memory***********************************/
void Read_memory(void)
{
    unsigned char i=0,j=0;

   for(j=0;j<6;j++)
   {	
	CALIBRATION[SET_FREQUENCY1]=eeprom_read_word(&ee_setup+(SET_FREQUENCY1*2));
    CALIBRATION[SET_DUTYCYCLE1]=eeprom_read_word(&ee_setup+(SET_DUTYCYCLE1*2));
	CALIBRATION[SET_FREQUENCY2]=eeprom_read_word(&ee_setup+(SET_FREQUENCY2*2));
	CALIBRATION[SET_DUTYCYCLE2]=eeprom_read_word(&ee_setup+(SET_DUTYCYCLE2*2));
	CALIBRATION[FREQUENCY3]=eeprom_read_word(&ee_setup+(FREQUENCY3*2));
    CALIBRATION[SPEED3]=eeprom_read_word(&ee_setup+(SPEED3*2));
    CALIBRATION[CHANNEL1_FREQUENCY]=eeprom_read_word(&ee_setup+(CHANNEL1_FREQUENCY*2));
	CALIBRATION[CHANNEL1_DUTYCYCLE]=eeprom_read_word(&ee_setup+(CHANNEL1_DUTYCYCLE*2));
	CALIBRATION[CHANNEL1_MINIMUM]=eeprom_read_word(&ee_setup+(CHANNEL1_MINIMUM*2));
	CALIBRATION[CHANNEL1_MAXIMUM]=eeprom_read_word(&ee_setup+(CHANNEL1_MAXIMUM*2));
	CALIBRATION[CHANNEL1_MINIMUMBAND]=eeprom_read_word(&ee_setup+(CHANNEL1_MINIMUMBAND*2));
	CALIBRATION[CHANNEL1_MAXIMUMBAND]=eeprom_read_word(&ee_setup+(CHANNEL1_MAXIMUMBAND*2));
	CALIBRATION[CHANNEL2_FREQUENCY]=eeprom_read_word(&ee_setup+(CHANNEL2_FREQUENCY*2));
    CALIBRATION[CHANNEL2_DUTYCYCLE]=eeprom_read_word(&ee_setup+(CHANNEL2_DUTYCYCLE*2));
	CALIBRATION[CHANNEL2_MINIMUM]=eeprom_read_word(&ee_setup+(CHANNEL2_MINIMUM*2));
	CALIBRATION[CHANNEL2_MAXIMUM]=eeprom_read_word(&ee_setup+(CHANNEL2_MAXIMUM*2));
	CALIBRATION[CHANNEL2_MINIMUMBAND]=eeprom_read_word(&ee_setup+(CHANNEL2_MINIMUMBAND*2));
	CALIBRATION[CHANNEL2_MAXIMUMBAND]=eeprom_read_word(&ee_setup+(CHANNEL2_MAXIMUMBAND*2));
    CALIBRATION[PWM_FREQ_SEL]=eeprom_read_word(&ee_setup+(PWM_FREQ_SEL*2));

	for(i=21;i<28;i++)CALIBRATION[i]=0;
	CALIBRATION[PARAMETERLOCK]=1;

    for(i=14;i<21;i++)CALIBRATION[i]=eeprom_read_word(&ee_setup+(i*2)); 

    for(i=28;i<161;i++)CALIBRATION[i]=eeprom_read_word(&ee_setup+(i*2));  
	
	/* Read calibration parameters */	   
	Forward_Slope=eeprom_read_word(&ee_setup+366);
	Reverse_Slope=eeprom_read_word(&ee_setup+368);	
    Fm=(float)Forward_Slope/100.0;
	Rm=(float)Reverse_Slope/100.0;
	

	CALIBRATION[VERSION_NUMBER]=VERSION*10;
	nested_delay(10,10);
    //Calculating timer counts based on clock freq,1024 prescale,Overflow interrupt
	COUNTS=255.0 - ((CLOCK_FREQUENCY*1000000*0.010)/1024.0);

	if(CALIBRATION[ZERODELAY]<0||CALIBRATION[ZERODELAY]>20000)CALIBRATION[ZERODELAY]=5000;

   }
   Storedcnt=eeprom_read_word(&ee_setup+330); 

   
}
/*****************************************************************************************************/ 
void initialize_variable(void)
{
    unsigned char i=0;
    for(i=0;i<12;i++)CALIBRATION[i]=0; 
	
	CALIBRATION[ACCESSCODE]=0;    
	CALIBRATION[RESPONSE]=40;	
	CALIBRATION[PASSWORD]=4;
	CALIBRATION[DPSELECT]=1;
	CALIBRATION[ABSOLUTEVALUE]=0;
	CALIBRATION[ZERODELAY]=5000;

    CALIBRATION[SET_DUTYCYCLE1]=0;
    CALIBRATION[FREQUENCY2]=0;
    CALIBRATION[DUTYCYCLE2]=0;
    CALIBRATION[SET_FREQUENCY2]=7;
    CALIBRATION[INVERT1]=0;
	CALIBRATION[INVERT2]=0;

    CALIBRATION[SET_DUTYCYCLE2]=0;
    CALIBRATION[FREQUENCY3]=0;
    CALIBRATION[SPEED3]=0;
    


	if(CALIBRATION[FACTORYSET]==1|| CALIBRATION[FACTORYSET]==2||CALIBRATION[FACTORYSET]==3)//DTI
	{
		if(CALIBRATION[FACTORYSET]==2){CALIBRATION[PWM_FREQ_SEL]=2;CALIBRATION[CONTINUOUSDISPLAY]=4;}
		else if(CALIBRATION[FACTORYSET]==1) {CALIBRATION[PWM_FREQ_SEL]=1;CALIBRATION[CONTINUOUSDISPLAY]=1;}
		else {CALIBRATION[PWM_FREQ_SEL]=3;CALIBRATION[CONTINUOUSDISPLAY]=1;}
  
		CALIBRATION[TEMPDRIFT]=0;
		CALIBRATION[SUPPLYDRIFT]=0;
		CALIBRATION[MAXSPEED]=15000;
		CALIBRATION[CHANNEL1_FREQUENCY]=1;
 	    CALIBRATION[CHANNEL1_DUTYCYCLE]=0;
	    CALIBRATION[CHANNEL1_MINIMUM]=0;
	    CALIBRATION[CHANNEL1_MAXIMUM]=2000;
	    CALIBRATION[CHANNEL1_MINIMUMBAND]=0;
	    CALIBRATION[CHANNEL1_MAXIMUMBAND]=2000;
	    CALIBRATION[CHANNEL2_FREQUENCY]=1;
        CALIBRATION[CHANNEL2_DUTYCYCLE]=0;
        CALIBRATION[CHANNEL2_MINIMUM]=200;

        CALIBRATION[CHANNEL2_MAXIMUM]=1800;
        CALIBRATION[CHANNEL2_MINIMUMBAND]=0;
        CALIBRATION[CHANNEL2_MAXIMUMBAND]=2000;
	    CALIBRATION[FREQUENCY1]=0;
        CALIBRATION[DUTYCYCLE1]=0;
        CALIBRATION[SET_FREQUENCY1]=100;
		
		CALIBRATION[AIOFFSET1]=0;
		CALIBRATION[AIOFFSET2]=-7;
		CALIBRATION[AISCALE1]=1000;
		CALIBRATION[AISCALE2]=1010;
		CALIBRATION[AIRESPONSE1]=CALIBRATION[AIRESPONSE2]=100;
		CALIBRATION[AIFULLSCALE1]=CALIBRATION[AIFULLSCALE2]=1000;			
		CALIBRATION[SETA]=CALIBRATION[SETC]=CALIBRATION[ACTUALA]=CALIBRATION[ACTUALC]=0;
		CALIBRATION[SETB]=CALIBRATION[ACTUALB]=10000;
	}
	if(CALIBRATION[FACTORYSET]==3) //DSI/Lengthcounter/all
	{
			
		CALIBRATION[SPEEDMETHOD]=2;
		CALIBRATION[PULSES]=10;
		CALIBRATION[ENCPPR]=10;
		CALIBRATION[SAMPLINGINTERVAL]=500;
		CALIBRATION[FREQMULTIPLIER]=10000;
		CALIBRATION[SPDRESPONSE]=100;	
	}     
}
/************************************Factory set*****************************************/
void save_parameters(void)
{
    unsigned char i=0;
	
	for(i=14;i<21;i++)eeprom_write_word((&ee_setup+(i*2)),CALIBRATION[i]); 

    for(i=28;i<161;i++)eeprom_write_word((&ee_setup+(i*2)),CALIBRATION[i]);  

}
/***************************************************************************/	 

