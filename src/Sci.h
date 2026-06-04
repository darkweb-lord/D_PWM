/******************************************************************************
			   Filename     : sci.h     
			   Author       : Manjula P
			   Last Modified: 29/01/2010                                         
			   Description  : Serial Communication routines for PC Interface                       
******************************************************************************/
/*********SERIAL COMMUNICATION RECEIEVED DATA CHECK ROUTINE*****/

int sci_errchk(int MAX_NO, int *RX_PARAMETERS)
{
	unsigned int RETXCODE = RX_PARAMETERS[0];
	int i, NoError=0;
	
	for(i=1;i< MAX_NO;i++)
 	{
 		RETXCODE = RETXCODE ^ RX_PARAMETERS[i];
 	}
 	if(RETXCODE == RX_PARAMETERS[MAX_NO]) {UDR0= 0x12; NoError=1;} /* No error */
 	else {UDR0= 0x13; NoError=0;} /* Parity error occurs */	
 	
 			
 	return NoError;
} 
/*****************************************************************************************/
void Data_Download( int *CODE,int Size)
{
	unsigned char  i,j;
	unsigned int LSB,MSB;

		for(i=0,j=0;j<=Size;j++)
		{
			if(READ_CHAR[i]>0x30)LSB =(READ_CHAR[i++]-0x30)*100;
			else {LSB=0;i++;}
			if(READ_CHAR[i]>0x30)LSB+=(READ_CHAR[i++]-0x30)*10;
			else LSB+=(READ_CHAR[i++]-0x30)*0;
			if(READ_CHAR[i]>0x30)LSB+=(READ_CHAR[i++]-0x30);
			else{ LSB+=0;i++;}
			if(READ_CHAR[i]>0x30)MSB =(READ_CHAR[i++]-0x30)*100;
			else {MSB=0;i++;}
			if(READ_CHAR[i]>0x30)MSB+=(READ_CHAR[i++]-0x30)*10;
			else MSB+=(READ_CHAR[i++]-0x30)*0;
			if(READ_CHAR[i]>0x30)MSB+=(READ_CHAR[i++]-0x30);
			else {MSB+=0;i++;}
			CODE[j]= LSB + MSB * 256;			
		}
		
	return;
}	
									
/********************SERIAL TRANSMISSION FROM CONTROLLER TO PC***************************/

void sci_tx(int MAX_NO, int *TX_PARAMETERS)
{   
	unsigned char  i;
/* START CODE & VALUE OF THE MENUS incl. NO OF PARAMETERS IN 5 DIGIT FORM IS TRANSMITTED*/             		
	    
		/*1500 - 2400 baudrates */
		/* 500 - 9600 baudrates */
		for(i=0;i<MAX_NO+2;i++) 
  		{
         	UDR0  = TX_PARAMETERS[i] & 255;nested_delay(1,500);
            UDR0  = TX_PARAMETERS[i] >> 8; nested_delay(1,500);
		}                    
    	
/* STOP CODE IS TRANSMITTED*/ 	    	
     	UDR0  = 0x0010 ;
return;            		
}
 
/************* SERIAL COMMUNICATION MAIN ROUTINE ******************************************/
void sci_main( void )
{    
  	int CODE[200];
	unsigned char  i=0;	
	int index=0;  	
  	
/*** start code is continously read from the PC & the corresponding menu is read/written***/  
    if(DATA_RECEIVED==0)
    {
	
		if(READ_CODE >= 0x0050 && READ_CODE <= 0x0057) /* Upload */
		{	    		 
			/* parameters are read from the ctrller by the PC, based on the START CODE*/
		    CODE[0] = READ_CODE;
		    switch(READ_CODE)
		    {
		   	   
			    /*Operator menu parameters are read from the ctrller */
                case 0x0050:  CODE[1] =10; 																  
                              for(i=1;i<CODE[1];i++) CODE[i+2]=CALIBRATION[i];                              
                              break;				
                 /*Setup menu menu parameters are read from the ctrller */
                case 0x0054:  CODE[1] =150; 																  
                              for(i=0;i<CODE[1];i++) CODE[i+2]=CALIBRATION[i+14];                              
                              break;    									  
				default:      break; 				    											      				
						    							  														    							  
    		} 
			  			
	    	sci_tx(CODE[1],CODE);	    			
	    } 	    
	    else if(READ_CODE>= 0x0060 && READ_CODE<= 0x0070) /* Download */
	    {             
	        UDR0  = 0x11;      /* START CODE Ack is send */
			RX_BUSY=1; DATA_RECEIVED=0; x=0;
	    	DNLD_CODE= READ_CODE;
		}    
		
	} 	
	else 
	{
		switch(DNLD_CODE) 
	  	{
				/* Calibration PID Parameters */
				case 0x60:
					Data_Download(CODE,32);
					if(sci_errchk(32,CODE)==1)
					{						
						for(i=0;i<CODE[0];i++) 
						{
						    index= i+CODE[1];
							if(CODE[i+2]>9999)CALIBRATION[index]  = (CODE[i+2]-10000)* -1; 							
							else CALIBRATION[index] = CODE[i+2]; 
						    eeprom_write_word((&ee_setup+(index*2)),CALIBRATION[index]);
						} 						
					}
					break;          

                 default:break; 	
		}
				   
								
	    DNLD_CODE= 0; DATA_RECEIVED=0; RX_BUSY=0;      
	} 	
	
return;
					                                    
}
/************SERIAL RECEPTION WITH TIMEOUT CHECK **************************************************/

void sci_rx(unsigned int timeoutdelay)
{
 	unsigned int timein=1,timeout=0,RCVRDY = 0;
	
	RCVRDY = UCSR0A & 0x80;
	while(RCVRDY!=0x0080)
	{                        
		RCVRDY = UCSR0A & 0x80;
		timeout++;
		if(timeout>timeoutdelay){timein=0;break;}							
	} 
		 
	READ_CODE = UDR0; 	
	if(timein==1) sci_main();   
	
return;
}		 				
/***************************************************************************************************/  

void SCI_DOWNLOAD(void)  
{
    unsigned int RCVRDY = 0;

    if(RX_BUSY==1)  /* Serial Data Receive   Download */
	{		
		STEPS++;		
		RCVRDY= UCSR0A & 0x80;
		if(RCVRDY== 0x0080)   /* Every 10ms a byte is send from PC */
		{
			STEPS=0;
			READ_CHAR[x]= UDR0;							
			if(READ_CHAR[x]==0x0010 || x>220)
			{
			    RX_BUSY=0; 
				DATA_RECEIVED=1;
		    }
			else if(READ_CHAR[x]>=0x0060 && READ_CHAR[x]<=0x0070)  x=0;  
			else x++; 				
		}
		else if(STEPS>3) {STEPS= 0; RX_BUSY= 0; DNLD_CODE= 0;}
		/* if the interval is morethan 2.5 sec download is aborted */						
	} 
	
	SCI_COUNT++; 
 
 	return;
 
} 
/*******************************************************************************************/


		

