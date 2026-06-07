/******************************************************************************
		   Filename     : scankeys.h     
		   Author       : Manjula P
		   Last Modified: 29/01/2010                                        
		   Description  : Key functions are defined
******************************************************************************
   This routine is used for incrementing/ decrementing the value of variables.
   If the value is greater than max value, max value is set to variable
   If the value is less than min value, min value is set to variable
   Based on the cursor position INC/DEC steps are adjusted by this routine 
**************************************************************************************/

int INC_DEC_VALUE(int key,  long int value)
{
    int max=32000, curmax=32000;
	int min=0;     
    long int absval=0; 	

    if(ENTER_KEYCOUNT==1){MAXVALUE=160;MIN=1;}
	max=MAXVALUE;min=MIN;
	for(MAX_CUR_POS=0,curmax=max; MAX_CUR_POS<5; ++MAX_CUR_POS,curmax/=10)
    { 
        if(curmax<10) break;        
    }  
    if(key==PLUS_KEY)
    {
        if(Increment)value = value + MULTIPLE+4; //Increment by 5 if plus key pressed continuously.
		else value = value + MULTIPLE; //Increment
        if(value > max)value= max;        
    }   
    else if(key==MINUS_KEY)  
    {    
        absval= fabs(value);
        if(((value- MULTIPLE)<= min || (absval- MULTIPLE)<MULTIPLE) && MULTIPLE>1)
        {            
            if((absval- MULTIPLE)< MULTIPLE && value!=MULTIPLE && value > MULTIPLE) value = 2 * MULTIPLE;
            else{ MULTIPLE= MULTIPLE/10; CURSOR_POSITION--; }                    
        }        
        if(Increment)value = value - MULTIPLE-4;         //Decrement by 5 if minus key pressed continuously
		else value = value - MULTIPLE; 
        if(value < min)
        {
            value= min ;                 
            CURSOR_POSITION = 0;   
            MULTIPLE = 1;
        }             
    }        
    /*if(value<0)SIGN=1;
	else SIGN=0;*/
  return value;
} 
    
/************** To Generate address of pages to be displayed ****************************************/  
            
void MSG_HANDLER()   
{   
     /* To Display Operator Menu */
    if(ENTER_KEYCOUNT==0||ENTER_KEYCOUNT==1)
	{
		split_numeric(DIGITS);
		DECIMAL_POINT=0;
	}    
	   /*Setup  menu page value display*/
    else if(ENTER_KEYCOUNT>1)split_numeric(fabs(CALIBRATION[SETUP_INDEX])); 		
     
	DISPLAY_VALUE();       
  return;
}
/*****************************************************************************************************  
   To Move Cursor automatically to next lower digit and goes off
after minimum position                                                  
******************************************************************************************************/

void CURSOR_MOVE()
{
    if(CURSOR==TRUE)
    {   
        ++CURSOR_CNT;
        /* called every 5s */
        if(CURSOR_CNT>=125) 
        {
            --CURSOR_POSITION;   
            MULTIPLE= MULTIPLE / 10;            
            
            if(CURSOR_POSITION<0)
            {    
                CURSOR_POSITION = 0;
                MULTIPLE = 1;           
                KEY_PRESSED = TRUE; 
				CURSOR=FALSE; 
				if(ENTER_KEYCOUNT==1)ENTER_KEYCOUNT=0;
				else if(ENTER_KEYCOUNT>1)ENTER_KEYCOUNT=2;                                             
            }
            CURSOR_CNT=0; 
        } 
    }
	else        //Display selected parameter continuously
	{
		 Opmenucount++;		 
		 //40 sec 
		 if(Opmenucount>=750)
		 {
		    DIGITS=OPMENU_PAGE=CALIBRATION[CONTINUOUSDISPLAY];
			ENTER_KEYCOUNT=2;
			Opmenucount=0;
	     }
		 else if(PREVCOUNT==1)
		 {
		 	SETUP_INDEX=DIGITS=OPMENU_PAGE=PREVPAGE;
			ENTER_KEYCOUNT=PREVENTERCOUNT;
			PREVCOUNT=0;
		 }

		 MSG_HANDLER();
     }

  return;
}
/********************************************************************************************************
     Keys pressed are sensed and associated functions are done         
*********************************************************************************************************/ 	   	                            
void SCAN_KEYS()
{
 
 unsigned char Refresh= TRUE;  
    
    KEY =PINB & 0x0F; 

	SETUP_INDEX=OPMENU_PAGE; //Calulate index
    if(ENTER_KEYCOUNT>1 && CALIBRATION[SETUP_INDEX]<0)SIGN=1;
	else SIGN=0;
    if(OPMENU_PAGE!=149)PrevDP=CALIBRATION[DPSELECT]; // to update values based on precision
   /*Define the max values*/
	if(ENTER_KEYCOUNT>1)
    {
       
	   if((OPMENU_PAGE>26 && OPMENU_PAGE<28) ){MAXVALUE=9999;MIN=-9999;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==11){MAXVALUE=9999;MIN=-9999;DECIMAL_POINT=CALIBRATION[DPSELECT];}
	   else if(OPMENU_PAGE==1){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==2){MAXVALUE=100;MIN=0;DECIMAL_POINT=1;}
       else if(OPMENU_PAGE==3){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==4){MAXVALUE=1000;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==5){MAXVALUE=7;MIN=2;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==6){MAXVALUE=100;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==7){MAXVALUE=7;MIN=2;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==8){MAXVALUE=1000;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==6||OPMENU_PAGE==125||OPMENU_PAGE==134||OPMENU_PAGE==141||OPMENU_PAGE==146){MAXVALUE=32000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE>151 && OPMENU_PAGE<158){MAXVALUE=32000;MIN=-9999;DECIMAL_POINT=0;}
       else if(OPMENU_PAGE==7){MAXVALUE=32000;MIN=0;if(FilteredFreq<=3000.0)DECIMAL_POINT=1; else  DECIMAL_POINT=2;}
       else if(OPMENU_PAGE==8){MAXVALUE=32000;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==9){MAXVALUE=32000;MIN=0;if(FilteredFreq<=3000.0)DECIMAL_POINT=0; else  DECIMAL_POINT=1;}
       else if(OPMENU_PAGE==10){MAXVALUE=32000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==28){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==29){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==30){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==31){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==32){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
       else if(OPMENU_PAGE==33){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==34){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==43){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==37){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
       else if(OPMENU_PAGE==38){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==39){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==40){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==41){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;} 
	   else if(OPMENU_PAGE==42){MAXVALUE=5000;MIN=0;DECIMAL_POINT=0;} 
	   else if(OPMENU_PAGE==12 ||(OPMENU_PAGE>45 && OPMENU_PAGE<66)||OPMENU_PAGE==148){MAXVALUE=1;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==13){MAXVALUE=9999;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==14||OPMENU_PAGE==149){MAXVALUE=2;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==17){MAXVALUE=9999;MIN=0;DECIMAL_POINT=CALIBRATION[DPSELECT];}
	   else if(OPMENU_PAGE==16){MAXVALUE=30000;MIN=0;DECIMAL_POINT=CALIBRATION[DPSELECT];}
	   else if(OPMENU_PAGE==116||OPMENU_PAGE==117||OPMENU_PAGE==150){MAXVALUE=9999;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==18){MAXVALUE=5;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==19){MAXVALUE=11;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==20){MAXVALUE=100;MIN=2;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==110||OPMENU_PAGE==151){MAXVALUE=5;MIN=0;DECIMAL_POINT=0;}	   
	   else if(OPMENU_PAGE>=21 && OPMENU_PAGE<=23){MAXVALUE=9999;MIN=-9999;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==111||OPMENU_PAGE==112||OPMENU_PAGE==120){MAXVALUE=3;MIN=0;DECIMAL_POINT=0;}
	   /*else if(OPMENU_PAGE>=30 && OPMENU_PAGE<=36){MAXVALUE=3000;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE>=37 && OPMENU_PAGE<=43){MAXVALUE=3000;MIN=-3000;DECIMAL_POINT=1;}*/
	   else if(OPMENU_PAGE==71 || OPMENU_PAGE==78||OPMENU_PAGE==85||OPMENU_PAGE==92||OPMENU_PAGE==75||OPMENU_PAGE==82||OPMENU_PAGE==89||OPMENU_PAGE==96||OPMENU_PAGE==123||OPMENU_PAGE==132||OPMENU_PAGE==126 ||OPMENU_PAGE==135){MAXVALUE=10000;MIN=-1000;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==72 || OPMENU_PAGE==79||OPMENU_PAGE==86||OPMENU_PAGE==93||OPMENU_PAGE==130||OPMENU_PAGE==131||OPMENU_PAGE==121||OPMENU_PAGE==122){MAXVALUE=11000;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==76 || OPMENU_PAGE==83||OPMENU_PAGE==90||OPMENU_PAGE==97||OPMENU_PAGE==158||OPMENU_PAGE==159){MAXVALUE=9999;MIN=-9999;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==73 || OPMENU_PAGE==80||OPMENU_PAGE==87||OPMENU_PAGE==94||OPMENU_PAGE==124||OPMENU_PAGE==133||OPMENU_PAGE==139||OPMENU_PAGE==142){MAXVALUE=9999;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==74 || OPMENU_PAGE==81||OPMENU_PAGE==88||OPMENU_PAGE==95){MAXVALUE=9999;MIN=0;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==119 || OPMENU_PAGE==128){MAXVALUE=15;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==129){MAXVALUE=4;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==136){MAXVALUE=2;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==137){MAXVALUE=32000;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==143){MAXVALUE=20000;MIN=1;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==145||OPMENU_PAGE==147){MAXVALUE=9999;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==144){MAXVALUE=32000;MIN=0;DECIMAL_POINT=0;}
	   else if(OPMENU_PAGE==138){MAXVALUE=9999;MIN=1;DECIMAL_POINT=1;}
	   else if(OPMENU_PAGE==116){MAXVALUE=32000;MIN=0;DECIMAL_POINT=0;}	
	   else if(OPMENU_PAGE==50){MAXVALUE=10000;MIN=0;DECIMAL_POINT=2;}		 	   
	   else {MAXVALUE=9999;MIN=0;DECIMAL_POINT=0;}
	}    
    else DECIMAL_POINT=0;

    if(KEY==NO_KEY)
    {   
        KEY_PRESSED= KEY_DEBOUNCE= FALSE; 
		CURSOR_MOVE();
		Increment=Delaycount=0;		
        return;  
    }
    
    if(KEY!= NO_KEY && KEY_PRESSED==FALSE)
    {        
        ++KEY_DEBOUNCE; 
        /* To avoid glitches */
        if(KEY_DEBOUNCE<5) return;     
        KEY_PRESSED  = TRUE;         
        KEY_OPRN_OVER= CURSOR_CNT=Opmenucount= FALSE;                                                                                                                     
    }  
	      	 
    if(KEY_PRESSED==TRUE && KEY_OPRN_OVER==FALSE)
    {                
        
       switch (KEY)
        {            
             case PLUS_KEY:         
                   
				    PREVCOUNT=0;
					/* Page scroll up in Operator Menu */   
                    if(ENTER_KEYCOUNT==0 ||ENTER_KEYCOUNT==1 )    
                    { 					    
					  if(ENTER_KEYCOUNT==0)++OPMENU_PAGE;
					  else if(ENTER_KEYCOUNT==1 && CURSOR ==TRUE)OPMENU_PAGE=INC_DEC_VALUE(KEY, OPMENU_PAGE);  //Enter the page number
					  if(PASS==CALIBRATION[PASSWORD])
					  {	
					    if(OPMENU_PAGE>=18 && OPMENU_PAGE<=69)OPMENU_PAGE=70;
						else if(OPMENU_PAGE>=84 && OPMENU_PAGE<=109)OPMENU_PAGE=110;
						else if(OPMENU_PAGE>=147 && OPMENU_PAGE<=150)OPMENU_PAGE=151;
						else if(OPMENU_PAGE>=152 && OPMENU_PAGE<=160)OPMENU_PAGE=151;
						else if(OPMENU_PAGE>MAX_PAGE)OPMENU_PAGE= MAX_PAGE;	                       
					  	
					  }
					  else
					  {
					  	if(OPMENU_PAGE>MAX_PAGE)OPMENU_PAGE= MAX_PAGE;
					  }						
					  DIGITS=OPMENU_PAGE;													
                      break;
                    } 													  	            
                    /* To Edit the values */                   
					if(ENTER_KEYCOUNT==3 && CURSOR ==TRUE)
                    {                                        
					    CALIBRATION[SETUP_INDEX]=INC_DEC_VALUE(KEY, CALIBRATION[SETUP_INDEX]); 					                           
                        break;
                    }
									                        
                    return;                         
                        
            case MINUS_KEY: 
			          
                    PREVCOUNT=0;
				    /* To scroll various Set up Menus in descending order */
                    if(ENTER_KEYCOUNT==0||ENTER_KEYCOUNT==1)     
                    {    
						if(ENTER_KEYCOUNT==0)--OPMENU_PAGE;
						else if(ENTER_KEYCOUNT==1 && CURSOR ==TRUE)OPMENU_PAGE=INC_DEC_VALUE(KEY, OPMENU_PAGE);//Enter the page number
						if(PASS==CALIBRATION[PASSWORD])
						{	
						    if(OPMENU_PAGE<=1)OPMENU_PAGE=1;
							else if(OPMENU_PAGE>=18 && OPMENU_PAGE<=69)OPMENU_PAGE=17;
							else if(OPMENU_PAGE>=84 && OPMENU_PAGE<=109)OPMENU_PAGE=83;
							else if(OPMENU_PAGE>=147 && OPMENU_PAGE<=150)OPMENU_PAGE=146;
							else if(OPMENU_PAGE>=152 && OPMENU_PAGE<=160)OPMENU_PAGE=151;
						}
						else
						{
							if(OPMENU_PAGE<1)OPMENU_PAGE= 1;
						}				
						DIGITS=OPMENU_PAGE;												
                        break;                                                    
                    } 
				  
                    /* To Edit the values*/ 
					if(ENTER_KEYCOUNT==3 && CURSOR == TRUE)
                    { 
						CALIBRATION[SETUP_INDEX]=INC_DEC_VALUE(KEY, CALIBRATION[SETUP_INDEX]); 	                     
                        break;
                    } 
					
                    return; 

           case SET_KEY:  
		     
		            PREVCOUNT=0;
		   			//ENTERKEY AND PLUSKEY, to goto corresponding page no                  
                    if(PASS==MPASSWORD||PASS==CALIBRATION[PASSWORD])SETPAGE=1;
					if(OPMENU_PAGE>0 && SETPAGE==1 && ENTER_KEYCOUNT==0){ENTER_KEYCOUNT=1;CURSOR = TRUE;}
					break;			               
                    return;                
           case ENTER_KEY:                                         
	               
					CURSOR_POSITION=0;
					MULTIPLE=1;	
					PREVCOUNT=0;	
	                /* Enter into page no enter menu  */ 
	                if(ENTER_KEYCOUNT==0)
	                { 					
                        ENTER_KEYCOUNT=2;
						CURSOR = FALSE;													                                                                
	                    break;                   
					}
					/*Edit the page number*/
					if(ENTER_KEYCOUNT==1)
                    { 		
						SETPAGE=0;					
						ENTER_KEYCOUNT=2;							
					    CURSOR = FALSE;		                                                                
	                    break; 			                          
                    }   
					                                        
	                if(ENTER_KEYCOUNT==2) 
	                {
					    ENTER_KEYCOUNT= 3; 	
						if(/*(OPMENU_PAGE<12) ||*/(OPMENU_PAGE>=21 && OPMENU_PAGE<=27)||/*(OPMENU_PAGE>=39 && OPMENU_PAGE<=43)||*/OPMENU_PAGE==46||OPMENU_PAGE==48||OPMENU_PAGE==70||OPMENU_PAGE==75||OPMENU_PAGE==76||OPMENU_PAGE==77||OPMENU_PAGE==82||OPMENU_PAGE==83||OPMENU_PAGE==84||OPMENU_PAGE==89||OPMENU_PAGE==90||OPMENU_PAGE==91||OPMENU_PAGE==96||OPMENU_PAGE==97||OPMENU_PAGE==118||OPMENU_PAGE==126||OPMENU_PAGE==127||OPMENU_PAGE==135||OPMENU_PAGE==150)CURSOR = FALSE;	
						else CURSOR=TRUE;
						break;                                                     
	                }					
			    	/*To save editing values*/
                    if(ENTER_KEYCOUNT==3)
                    {  	 
						CURSOR=FALSE;
						/* Select lock the parameter or unlock the parameter 0- Unlock,1- Lock*/
						if(PREVIOUSPAGE!=CALIBRATION[PARAMETERLOCK])
						{
							PREVIOUSPAGE=CALIBRATION[PARAMETERLOCK];
							if(CALIBRATION[PARAMETERLOCK]==0)MAX_PAGE=13;
							else MAX_PAGE=12;
                        }
						if(OPMENU_PAGE==13) //set Maximum page
						{
							if(CALIBRATION[ACCESSCODE]==MPASSWORD || CALIBRATION[ACCESSCODE]==CALIBRATION[PASSWORD]){MAX_PAGE=160;PASS=CALIBRATION[ACCESSCODE];CALIBRATION[ACCESSCODE]=0;ENTER_KEYCOUNT=0;DIGITS=OPMENU_PAGE;}
							else {MAX_PAGE=13;FAULT=3;OPMENU_PAGE=0;PASS=0;}
							
						}
						else 
						{
							ENTER_KEYCOUNT=2;
							Refresh=FALSE;
						}
										
						eeprom_write_word((&ee_setup+(SETUP_INDEX*2)),CALIBRATION[SETUP_INDEX]); 
						if(OPMENU_PAGE==18) eeprom_write_word((&ee_setup+38),CALIBRATION[CONTINUOUSDISPLAY]); 					
						break;                            
                    } 
					
                    return; 			      
              
			case CURSOR_KEY:                        
                    
                    /* Finds max cursor position,Avoids Cursor at Decimal points */
                    if(CURSOR == TRUE)
                    {   
                        PREVCOUNT=0;
						INC_DEC_VALUE(KEY,0);   
                        ++CURSOR_POSITION;   
                        MULTIPLE= MULTIPLE*10;                                                            
                        /* To fix CURSOR at 0 th location */
                        if(CURSOR_POSITION>MAX_CUR_POS)
                        {   
                            CURSOR_POSITION = 0; MULTIPLE = 1;
                        }				                           
                        break;
                    }
					else 
					{						
						if(PREVCOUNT==0)
						{
							SETUP_INDEX=PREVPAGE=OPMENU_PAGE;
							PREVENTERCOUNT=ENTER_KEYCOUNT;
							PREVCOUNT=1;
						}
						SETUP_INDEX=DIGITS=OPMENU_PAGE=2;
						ENTER_KEYCOUNT=2;					
					}					 
                    return;	 	                               
                                                                            
            case ESC_KEY:                                
                        
                    CURSOR_POSITION=0; MULTIPLE=1;
					CURSOR= FALSE;         
                    PREVCOUNT=0;
                    /*To Escape from pages of Menu to operator menu Page*/
                    if(ENTER_KEYCOUNT<=2) 
                    { 
					    ENTER_KEYCOUNT= 0;                      
						DIGITS=OPMENU_PAGE;	 						               
                        break;
                    }					              
                    /*To Escape from setup menu parameters to setup Menu Options*/
                    if(ENTER_KEYCOUNT>2) 
                    {                                        
                        ENTER_KEYCOUNT=2;                         									
                        break;
                    }
					         			                                                     
                    return;          
            default :break;return;                        
        }        
        if(Refresh) MSG_HANDLER();                
    }    
      
    KEY_OPRN_OVER= TRUE;  
    
    /* For PLUS/MINUS keys when keys are pressed continuously
       values are inc/dec. For other keys one function/pressing
       is allowed ,this is for delay
       */      
    if(KEY==PLUS_KEY || KEY==MINUS_KEY)
    {
        ++KEY_CONT_PRESS;
		Delaycount++;
		if(Delaycount>=75)Increment=1; //3.75sec
		else Increment=0;
        if(KEY_CONT_PRESS<6) return;//300msec
        else KEY_OPRN_OVER= KEY_CONT_PRESS= FALSE;  
    }
    else 
    {
        Increment=Delaycount=0;
		if(KEY!= NO_KEY) return;    
        else KEY_PRESSED= KEY_OPRN_OVER= FALSE;    
    }
	

  return;    
} 
/***************************************************************************************************/    
