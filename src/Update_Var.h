/***************************************************************************
			   Filename     : Update_Var.h    
			   Author       : Manjula P
			   Last Modified: 05/05/2010                                     
			   Description  : Values are updated    
********************************Configuring digital inputs*************************************************/
/********************************Update Polynomial co efficients**************************************/
void UpdatePolynomial_Coeff(void)
{

   Actual_TensionA=(float)CALIBRATION[ACTUALA]*ACT_TENSIONA;
   if(CALIBRATION[ACTUALB]<0)Actual_TensionB=(float)CALIBRATION[ACTUALB]*ACT_TENSIONB*10.0;
   else Actual_TensionB=(float)CALIBRATION[ACTUALB]*ACT_TENSIONB;
   if(CALIBRATION[ACTUALC]<0)Actual_TensionC=(float)CALIBRATION[ACTUALC]*ACT_TENSIONC*10.0;
   else Actual_TensionC=(float)CALIBRATION[ACTUALC]*ACT_TENSIONC;

   Set_TensionA=(float)CALIBRATION[SETA]*SET_TENSIONA;
   if(CALIBRATION[SETB]<0)Set_TensionB=(float)CALIBRATION[SETB]*SET_TENSIONB*10.0; 
   else Set_TensionB=(float)CALIBRATION[SETB]*SET_TENSIONB;
   if(CALIBRATION[SETC]<0)Set_TensionC=(float)CALIBRATION[SETC]*SET_TENSIONC*10.0;
   else Set_TensionC=(float)CALIBRATION[SETC]*SET_TENSIONC;
return;

}
/*****************************************Update PID variables and drift constants******************************************************/
void Value_updation(void)
{
    unsigned char i=0;

    PGain=(float)CALIBRATION[CHANNEL1_MINIMUM]/10.0;

	ITime=(float)CALIBRATION[CHANNEL1_MINIMUMBAND]/10.0;

	PLmt=(float)CALIBRATION[CHANNEL1_MAXIMUM]/10.0;

	DeadBand=(float)CALIBRATION[CHANNEL2_DUTYCYCLE]/10.0;

	ILmt=(float)CALIBRATION[CHANNEL1_MAXIMUMBAND]/10.0;

	Neglmt=CALIBRATION[CHANNEL2_FREQUENCY]/10.0; 
	 
	Temp_Drift= (float)CALIBRATION[TEMPDRIFT]/10000.0; 

	Supply_Drift=(float)CALIBRATION[SUPPLYDRIFT]/10000.0;

	for(i=0;i<4;i++)AI_FILTER[i]=1.0-exp(-(AI_SCANTIME/(float)CALIBRATION[AIRESPONSE1+(i*7)]));

return;

}
/*************************************Updating necessary speed constants*************************************************/
void Update_Speedconstants(void)
{ 
    float spd_response=0.0;
	
	Speed_Constant=60.0/(float)CALIBRATION[ENCPPR];

	Freq_Scale= (float)CALIBRATION[FREQMULTIPLIER]/10000.0;

	Sampling_Freq=1000.0/(float)CALIBRATION[SAMPLINGINTERVAL];

    spd_response = (((float)CALIBRATION[SPDOUTPUT]/(float)CALIBRATION[MAXSPEED])*(float)CALIBRATION[SPDRESPONSE])+(float)CALIBRATION[MINCOUNT] ;
	
	Filter_Coeff=(float)CALIBRATION[SPDRESPONSE];/*1.0-exp(-(AI_SCANTIME/SPDRESPONSE));*/
	
return;

}
/**********************************************************************************************************************/
