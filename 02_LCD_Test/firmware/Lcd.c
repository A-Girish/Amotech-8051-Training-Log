#include <reg51.h>

sbit d4 = P3^4;
sbit d5 = P3^5;
sbit d6 = P3^6;
sbit d7 = P3^7;
sbit en = P3^3;
sbit rs = P3^2;
sbit led = P1^0;

#define lcd_port P0
void delay(unsigned int delay)
{
	int i, j;
	for(i=0;i<delay;i++)
	for(j=0;j<112;j++);
}
void lcd_enable(void)
{
	en = 1;
	delay(2);
	en = 0;
}
void lcd_cmd(char cmd)
{
  lcd_port = (cmd & 0xF0)>>4;
	d7 = lcd_port & 0x08;
	d6 = lcd_port & 0x04;
	d5 = lcd_port & 0x02;
	d4 = lcd_port & 0x01;
	rs = 0;
	lcd_enable();
	
	lcd_port = (cmd & 0x0F);
	d7 = lcd_port & 0x08;
	d6 = lcd_port & 0x04;
	d5 = lcd_port & 0x02;
	d4 = lcd_port & 0x01;
	rs = 0;
	lcd_enable();
}	
void lcddata(char dat)
{
	delay(2);
  lcd_port = (dat & 0xF0)>>4;
	d7 = lcd_port & 0x08;
	d6 = lcd_port & 0x04;
	d5 = lcd_port & 0x02;
	d4 = lcd_port & 0x01;
	rs = 1;
	lcd_enable();
	
	lcd_port = (dat & 0x0F);
	d7 = lcd_port & 0x08;
	d6 = lcd_port & 0x04;
	d5 = lcd_port & 0x02;
	d4 = lcd_port & 0x01;
	rs = 1;
	lcd_enable();
}
//void lcd_string(char *str)
//{
//	int i;
//	for(i=0;str[i]!=0;i++)
//	{
//		lcddata(str[i]);
//		delay(100);
//	}
//}	

void lcd_string_xy(char row, char pos, char *str)
{
	int i;
	if(row == 0)
	{
		lcd_cmd((pos & 0x0F)| 0x80);
	}
	else if(row == 1)
	{
		lcd_cmd((pos & 0x0F)| 0xC0);
	}		
	for(i=0;str[i]!=0;i++)
	{
		lcddata(str[i]);
		delay(100);
	}
}
void lcd_Init(void)
{
	delay(20);
	lcd_cmd(0x01);		// clear display
	lcd_cmd(0x02);		// 4 bit mode
	lcd_cmd(0x28);		// lcd initialization in 4bit mode
	lcd_cmd(0x0C);		// Display ON Cursor OFF 	
	lcd_cmd(0x06);		// Auto Increment cursor 
}
void main(void)
{
	led = 0x00;
	lcd_Init();
	
	lcd_string_xy(0,0,"Girish  Kotalwar");
	lcd_cmd(0xC0);
	lcd_string_xy(1,5,"Latur");
	delay(1000);
	
	while(1);
}	