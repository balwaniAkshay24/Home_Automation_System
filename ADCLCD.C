#include <htc.h>
#include "tempdef.c"
#include "KEY4x3_PIC.c"

void interrupt isr(void)
{
	if(TMR1IF)		
	{
		TMR1IF = 0 ; 
		TMR1H = TIMER1_RELOADH ;
		TMR1L = TIMER1_RELOADL ;
        delaycnt++ ;
		if(RxFlag && Rxcnt)	Rxcnt-- ;
		milisec++ ;
		if(milisec>=1000)
		{
			milisec=0;
			Second++;
		}

	}
	if(ADIF)
	{
        ADIF = 0 ;
		adcflag = 1 ;
		adcval = (((UINT)ADRESH) << 8) + ADRESL ;
		adcval &= 0x03FF ;
	}
	if(RCIF)
	{
		DataBuffer[Rxindex] = RCREG ;
		if(DataBuffer[Rxindex]=='*')
			recBit=1;
		if(recBit==1)
		{	
			Rxcnt = RXCNT ;
			RxFlag = 1 ;
			Rxindex++ ;
		}	
//		RCIF = 0 ;
	}

}
void inithw(void)
{
	        ADCON0 = 0;	//adon bit1
        ADCON1 = 0X08;//	3 analog input
//        ADFM=1;
		ADCON2=0b10001010;
 		INTCON =  0X00 ;
		TRISA = 0X3f ;      //0000 1011
        TRISB = 0X00 ;          
        TRISC = 0XAF ;          
        TRISD = 0X00 ;          
        TRISE = 0X07 ;          
        T1CON = 0X01 ;
        
	RBPU =  0X00 ;
		ADIF=0;
        TMR1H = TIMER1_RELOADH ;
        TMR1L = TIMER1_RELOADL ;
        PEIE = 1 ;
        TMR1IE = 1 ;

	//SPBRG = 63 ;	//1200 baud
	SPBRG = 71;	//1200 baud
	BRGH = 1;//0
	//BRG16=1;
	CREN = 1 ; 		// receive enable ;
	SYNC = 0 ;
	SPEN = 1 ;
	TXIE = 0 ;
	RCIE =1 ;	//1 enable receive interrupt ;

	TX9 = 0 ;
	RX9 = 0 ;
	TXEN = 1 ;
    ADIE = 1 ;
	PIR1 = 0 ;
	GIE = 1 ;		//globle int enable
}


void delay(UINT num)
{
	UINT time ;
	time = num + delaycnt ;
	while(time != delaycnt) ;
}
void send_nibble2lcd(unsigned char PortData, char flag)
{



	UCHAR LcdData;	
#ifdef UPPER
	//higher------------------
/*	LcdData	=	LCDP;
	LcdData &= 0x0f;
	LcdData |= PortData;
	LCDP=LcdData ;
	//------------------------
	
	*/
	if(PortData & 0x80)
LCDD7=1;
else LCDD7=0;
  
 if(PortData & 0x40)
 LCDD6=1;
else LCDD6=0;
    
if(PortData & 0x20)
LCDD5=1;
else LCDD5=0;

if(PortData & 0x10)
LCDD4=1;
else LCDD4=0;
#else
	//lower------------------
	/*
	LcdData	=	LCDP;
	LcdData &= 0xf0;
	LcdData |= PortData;
	LCDP=LcdData ;
	*/
#endif	
	//--------------------
	LCDEN = 0 ;//cmnd
	if(flag)
	LCDRS = 0 ; //CMD 
	else
	  LCDRS = 1; //	DATA  
	asm("nop") ; 
    LCDEN = 1 ;
	asm("nop") ; 
    LCDEN = 0 ;
	asm("nop") ; 
    LCDRS = 0 ;//      LCDRS = 0 ;
    delay(2) ; //10ms                    

}

void write_lcd_command(void) 
	{
		unsigned char byte;
		byte=lcd_dat;
#ifdef UPPER
	send_nibble2lcd(((byte & 0xf0)), 1) ;
	delay(10) ;
   send_nibble2lcd(((byte & 0x0F)<<4), 1) ;
	delay(10);
#else
	send_nibble2lcd(((byte & 0xf0)>>4), 1) ;
	delay(10) ;
   send_nibble2lcd(((byte & 0x0F)), 1) ;
	delay(10);
	
#endif
	} 

void write_lcd_data(void)
	{
		unsigned char byte;
		byte=lcd_dat;
#ifdef UPPER		
	send_nibble2lcd(((byte & 0xf0)), 0) ;
	send_nibble2lcd(((byte & 0x0F)<<4), 0) ; 
#else
	send_nibble2lcd(((byte & 0xf0)>>4), 0) ;
	send_nibble2lcd(((byte & 0x0F)), 0) ; 

#endif	
	}
void initlcd(void)
{
	//LCDRW=0;
	lcd_dat = 0x03 ; write_lcd_command() ;
	delay(16) ;
	lcd_dat = 0x03 ; write_lcd_command() ;
	delay(16) ;
	lcd_dat = 0x03 ; write_lcd_command() ;
	delay(16);
	lcd_dat = 0x02; write_lcd_command() ;
	delay(16); 
	lcd_dat = 0x28 ; write_lcd_command() ;
	delay(16) ;
	lcd_dat = 0x08 ; write_lcd_command() ;
	delay(16) ;
	lcd_dat = 0x0c ; write_lcd_command() ;
	delay(4) ;
	lcd_dat = 0x06 ; write_lcd_command() ;
	delay(4) ;
}

void LCD_CursorOn(void)
{
	lcd_dat=0x0d;
	write_lcd_command();
}

void LCD_CursorOff(void)
{
	lcd_dat=0x0c;
	write_lcd_command();
}


void display(UCHAR addr)
{
	UCHAR k  ;
	lcd_dat = addr ; 
	write_lcd_command() ;
	for(k = 0 ; k <= 15 ; k++ )
	{
		lcd_dat = str[k] ;
		write_lcd_data() ;
	}
	delay(1) ;	
}

void dispstr(const UCHAR *s, UCHAR stcol, UCHAR endcol,UCHAR addr ) 
{
	UCHAR i ;
	for(i = stcol ; i <= endcol ; i++ )
	{
		str[i] = *s++ ;
		if(!(str[i]))		str[i] = ' ' ;
	}
	display(addr);
}	

void dispnum(UINT num, UCHAR stcol, UCHAR endcol, UCHAR dp,UCHAR addr)
{
	UCHAR i ;
	for(i = (endcol+1) ;i != stcol ; i--)
	{
		if(dp == (i-1))
			str[i-1] = '.' ;
		else
		{	 
			str[i-1] = ((num % 10) + '0') ;
			num /= 10 	;
		}	
	}
	display(addr) ;	 
}

void disp_clear(UCHAR stcol, UCHAR endcol,UCHAR addr)
{

	UCHAR i ;
	for(i = stcol; i <= endcol ; i++ )
	{
		str[i] = ' ' ;
	}		
	display(addr) ;
}

void LCD_Clear(void)
{
    UCHAR i ;
     for(i = 0; i <= 15 ; i++ )
 	{
		str[i] = ' ' ;
	}		
        lcd_dat=0x01;
        write_lcd_command();        
}

void LCD_Cursor (char row, char column)
{
    switch (row) {
        case 1: lcd_dat=0x80+column-1; write_lcd_command (); break;
        case 2: lcd_dat=0xc0+column-1; write_lcd_command (); break;
        default: break;
    }
}

void LCD_DisplayChar (char a_char)
{
		lcd_dat=a_char;
        write_lcd_data();
}

void channel_Sel(UCHAR channel)
{
        adcflag=0;
        if(channel==0)
	   		ADCON0 = 0x01 ;
	   	else if(channel==1)
	   		ADCON0 = 0x05 ;
		else if(channel==2)
			ADCON0 = 0x09 ;    	
		else if(channel==3)
			ADCON0 = 0x0d ;    	
		else if(channel==4)
			ADCON0 = 0x11 ;    	
		else if(channel==5)
			ADCON0 = 0x15 ;    	
		else if(channel==6)
			ADCON0 = 0x19 ;    	
		else if(channel==7)
			ADCON0 = 0x1d ;    	
        delay(5);
        GODONE=1;
}

void initvar(void)
{
	adcflag=0;
	delaycnt=0;
 	Second=0;
    milisec=0;
    scan=0;
	Rxindex=0;
	RxFlag = 0 ;
	Rxcnt = 0 ;
	recBit=0;
}
void Transmit(UCHAR Byte)
{
	while(!TXIF) ;
	TXREG = Byte ;
	delay(100);
}
void TransStr(const UCHAR *s) 
{
	while(*s)
		Transmit(*s++) ;
}
void transNum(UINT num, UCHAR stcol, UCHAR endcol, UCHAR dp)
{
	UCHAR i ;
	for(i = (endcol+1) ;i != stcol ; i--)
	{
		if(dp == (i-1))
			str[i-1] = '.' ;
		else
		{	 
			str[i-1] = ((num % 10) + '0') ;
			num /= 10 	;
		}	
		
	}
	for(i = stcol ; i <= endcol ; i++ )
	{
		Transmit(str[i]) ;	 
	}
	
}

void getNo()
{
	unsigned char i;
	LCD_Clear();
	    dispstr("Enter Password: ", 0,15,0x80) ;
		i=0;
		LCD_Cursor(2,1);
		do
		{
			getKey1();
			if(Key != 0xff)
			{
				if(i < 4)
				{
					KeyBuff[i]	=Key+'0';
					LCD_DisplayChar(KeyBuff[i]);
					i++;
					delay(250);
				}	
			}	
	        delay(250);							
		}while(	!(Key==KEY_ENTER) );
}


void main(void)
{
	inithw() ;
	initlcd() ;
	initvar() ;
  	BUZ=1;
  	DC11=0;
  	DC12=0;
  	DC21=0;
  	DC22=0;
  	
	LCD_Clear();
    	dispstr(" GSM Based Home ", 0,15,0x80) ;
    	dispstr("Security System ", 0,15,0xC0) ;
	delay(2000);
    
	adcflag=0;
	scan=0;
	RxFlag=0;    
	Rxindex=0;
	Rxcnt=0;
	in=0;
	msg1=1;
	msg2=1;
	Pass[0]='1';
	Pass[1]='2';
	Pass[2]='3';
	Pass[3]='4';
	
	
	
while(1)
{
	Key=getKey1();
	
	if(Key==KEY_1)
	{
		delay(300);
		Key=0xff;
		getNo();
		if(KeyBuff[0]==Pass[0] && KeyBuff[1]==Pass[1] && KeyBuff[2]==Pass[2] && KeyBuff[3]==Pass[3])
		{
			LCD_Clear();
		    	dispstr("Valid Password  ", 0,15,0x80) ;
		    	in=!in;
			DC11=0;
			DC12=1;
			DC21=0;
			DC22=1;
			delay(1000);
			DC11=0;
			DC12=0;
			DC21=0;
			DC22=0;
			if(in)
			{
				dispstr("Person IN       ", 0,15,0xC0) ;
				delay(1000);
			}
			else
			{
				dispstr("Person OUT      ", 0,15,0xC0) ;
				delay(1000);
			}
			DC11=1;
			DC12=0;
			DC21=1;
			DC22=0;
			delay(1000);
			DC11=0;
			DC12=0;
			DC21=0;
			DC22=0;
			
		}
		else
		{
			LCD_Clear();
			dispstr("Invalid Password", 0,15,0x80) ;
			TransStr("*1");
			Transmit(0x0d);
			Transmit(0x0a);
			BUZ=0;
	  		delay(1000);
	  		BUZ=1;
		}
	}
	
	if(IR)
	{
		if(!in)
		{
			LCD_Clear();
			dispstr("Intruder Detected", 0,15,0x80) ;
			if(msg1)
			{
				msg1=0;
				TransStr("*2");
				Transmit(0x0d);
				Transmit(0x0a);
			}
			delay(500);
		}
		
	}
	else
	{
		msg1=1;
	}
	
	if(Second>=2)
	    {
    		if(scan==0)
		{
			scan=1;
			channel_Sel(0);
			while(!adcflag);
			GODONE=0;
			adcflag=0;
			Temp1=adcval/1.024;
			LCD_Clear();
			dispstr("LPG:            ",0,15,0x80);
			dispnum(Temp1,8,11,16,0x80);
			TransStr("*G");
			transNum(Temp1,1,4,0);
			Transmit(0x0d);
			Transmit(0x0a);
			if(Temp1>700)
			{
				dispstr("LPG Detected    ",0,15,0xC0);
				BUZ=0;
				delay(500);
				BUZ=1;
//				TransStr("*Gas Detected");
//				Transmit(0x0d);
//				Transmit(0x0a);
			}
			else
			{
				BUZ=1;
			}
		
		}

		else if(scan==1)
		{
			scan=0;
			channel_Sel(5);
			while(!adcflag);
			GODONE=0;
			adcflag=0;
			Temp2=adcval/1.024;
			LCD_Clear();
			dispstr("Smoke:          ",0,15,0x80);
			dispnum(Temp2,8,11,16,0x80);
			TransStr("*S");
			transNum(Temp2,1,4,0);
			Transmit(0x0d);
			Transmit(0x0a);
			if(Temp2>700)
			{
				dispstr("Smoke Detected  ",0,15,0xC0);
				BUZ=0;
				delay(500);
				BUZ=1;
//				TransStr("*Alcohol Detected");
//				Transmit(0x0d);
//				Transmit(0x0a);	
			}
			else
			{
				BUZ=1;
			}
		}
		
		Second=0;
	}
	
}
	
		
}

