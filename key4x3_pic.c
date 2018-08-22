#include <htc.h>
#include "tempdef.c"

static volatile bit	 SCAN1	@ (unsigned)&PORTD*8+0 ;
static volatile bit	 SCAN2	@ (unsigned)&PORTD*8+1 ;
static volatile bit	 SCAN3	@ (unsigned)&PORTD*8+2 ;
static volatile bit	 SCAN4	@ (unsigned)&PORTD*8+3 ;
static volatile bit	 RETUR3	@ (unsigned)&PORTC*8+0 ;
static volatile bit	 RETUR2	@ (unsigned)&PORTC*8+1 ;
static volatile bit	 RETUR1	@ (unsigned)&PORTC*8+2 ;

char getKey1()
{
		SCAN1=1;
		SCAN2=1;
		SCAN3=1;
		SCAN4=1;		
		delay(1);				
		Key=0xFF;

	if(RETUR1 && RETUR2 && RETUR3)
	{
				SCAN1=0;
				delay(5);
				if(!RETUR1)
				{
					Key=KEY_3;
					return(Key);
				}
				else if(!RETUR2)
				{
					Key=KEY_2;
					return(Key);
				}
				else if(!RETUR3)
				{
					Key=KEY_1;
					return(Key);
				}
				SCAN1=1;
				SCAN2=0;
				delay(5);
				if(RETUR1==0)
				{
					Key=KEY_6;
					return(Key);
				}
					
				else if(RETUR2==0)
				{
					Key=KEY_5;
					return(Key);
				}
					
				else if(RETUR3==0)
				{
					Key=KEY_4;
					return(Key);
				}
				SCAN2=1;	
				SCAN3=0;
				delay(5);
				
				if(RETUR1==0)
				{
					Key=KEY_9;
					return(Key);
				}
					
				else if(RETUR2==0)
				{
					Key=KEY_8;
					return(Key);
				}
					
				else if(RETUR3==0)
				{
					Key=KEY_7;
					return(Key);
				}
				SCAN3=1;	
				SCAN4=0;
				delay(5);
				
				if(RETUR1==0)
				{
					Key=KEY_ENTER ;
					return(Key);
				}
					
				else if(RETUR2==0)
				{
					Key=KEY_0;
					return(Key);
				}
					
				else if(RETUR3==0)
				{
					Key=KEY_CANCEL;
					return(Key);
				}
				SCAN4=1;
//				delay(250);
	}
	return(0);
}
void getKey()
{
	char zx,vx;
	zx=0;
	Key=0xff;
	while(!(zx))// || TimeOutFlag))
	{
		Key=0xff;
		vx=getKey1();
		if(Key < 0xff)
		{
			KeyFlag=0;
			zx=1;
//			TimeOutFlag=0;
//	Timer=1;
//	Tout=2000;
		
		}
		
	}
}
