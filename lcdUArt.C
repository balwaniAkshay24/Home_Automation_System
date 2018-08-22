//4 bit lcd
//baudrate=fosc/16(spbrg+1)	if brgh=1 & sync=0
#include <htc.h>
#include "tempdef.c"

void interrupt isr(void)
{
	if(TMR1IF)		
	{
		TMR1IF = 0 ; 
		TMR1H = TIMER1_RELOADH ;
		TMR1L = TIMER1_RELOADL ;

        delaycnt++ ;
		milisec++ ;
		if(milisec >= 1000)	{milisec = 0 ;}
	}
	if(RCIF)
	{
		Rxindex = RCREG ;
		RxFlag=1;
	}


}
void inithw(void)
{
        ADCON1 = 0X06 ;      //all digital
        OPTION =  0X00 ;
 		INTCON =  0X00 ;
		TRISA = 0X00 ;      //input mode
        TRISB = 0X00 ;          
        TRISC = 0X80 ;          
        TRISD = 0X00 ;     
        TRISE = 0X00 ;          
        T1CON = 0X01 ;
		RBPU =  0X00 ;
        TMR1H = TIMER1_RELOADH ;
        TMR1L = TIMER1_RELOADL ;
        PEIE = 1 ;
        TMR1IE = 1 ;
//if brgh=0 then baud=fosc/(16*(x+1)) where x=spbrg count
	SPBRG = 71 ;	//9600 baud
	CREN = 1 ; 		// receive enable ;
	BRGH = 1 ;
	SYNC = 0 ;
	SPEN = 1 ;
	TXIE = 0 ;
	RCIE = 1 ;	//1 enable receive interrupt ;
	TX9 = 0 ;
	RX9 = 0 ;
	TXEN = 1 ;
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
	LcdData	=	PORTD;
	LcdData &= 0xf0 ;
	LcdData |= PortData;
	PORTD=LcdData ;
	LCDEN = 0 ;
	if(flag)
	LCDRS = 0 ;  
	else
	  LCDRS = 1; //	LCDRS = 1 ;  
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
	send_nibble2lcd(((byte & 0xf0)), 1) ;
	delay(10) ;
   send_nibble2lcd(((byte & 0x0F)<<4), 1) ;
	delay(10);
	} 
void write_lcd_data(void)
	{
		unsigned char byte;
		byte=lcd_dat;
	send_nibble2lcd(((byte & 0xf0)), 0) ;
	send_nibble2lcd(((byte & 0x0F)<<4), 0) ; 
	}

void initlcd(void)
{
//	LCDRW=0;
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
        delay(1);
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

void initvar(void)
{
	RxFlag = 0 ;
	Rxcnt = 0 ;
	Rxindex=0;
	delaycnt=0;
 	milisec=0;
 	
}

void Transmit(UCHAR Byte)
{
	while(!TXIF) ;
	TXREG = Byte ;
	delay(10);
}
void main(void)
{
	initvar() ;
	inithw() ;
	delay(500);	        
	initlcd() ;
	delay(500);	        
	RxFlag=0;
			LCD_Clear();
			dispstr("Welcome  ",0,15,0x80);
		//	dispstr("welcome         ",0,15,0xc0);
			delay(1000);	        
			Transmit('A');
//			LCD_Clear();
			LCD_Cursor(1,1);
			while(1)
			{
				if(RxFlag)
				{
					Transmit(Rxindex);
					LCD_DisplayChar(Rxindex);
					RxFlag=0;
					
				}
				delay(1);	
			}	
	        
}  
