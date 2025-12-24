#include <reg51.h>


sbit en = P3^3;
sbit rs = P3^2;
sbit led = P1^0;

#define lcd_port P3
#define led_port P0

void delay(unsigned int delay)	// 1ms delay generated
{
	int i, j;
	for(i=0;i<delay;i++)
	for(j=0;j<1275;j++);
}
void lcd_enable(void)  // Generating pulse for enable pin
{
	en = 1;
	delay(2);
	en = 0;
}
void lcd_cmd(char cmd)
{
  lcd_port = (lcd_port & 0x0F)|(cmd & 0xF0);	// only affects the upper nibble and lower nibble remains untouched
	rs = 0;
	lcd_enable();
	
	lcd_port = (lcd_port & 0x0F)|((cmd << 4) & 0xF0);	// lower nibble remains same as upper nibble is replaced by lower nibble of cmd
	rs = 0;
	lcd_enable();
}	
void lcddata(char dat)
{
	delay(2);
  lcd_port = (lcd_port & 0x0F)|(dat & 0xF0);  // only affects the upper nibble and lower nibble remains untouched
	rs = 1;
	lcd_enable();
	
	lcd_port = (lcd_port & 0x0F)|((dat << 4) & 0xF0);	// lower nibble remains same as upper nibble is replaced by lower nibble of data
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

void lcd_string_xy(char row, char pos, char *str)	// sending string as per the rows and coloumns
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
		delay(10);
	}
}
void lcd_Init(void)	// initialization of lcd in 4bit mode and other commands
{
	delay(20);
	lcd_cmd(0x02);		// 4 bit mode
	lcd_cmd(0x28);		// lcd initialization in 4bit mode
	lcd_cmd(0x0C);		// Display ON Cursor OFF 	
	lcd_cmd(0x06);		// Auto Increment cursor 
	lcd_cmd(0x01);
}
void main(void)
{
	led_port = 0x00;
	led = 0x00;
	lcd_Init();
	
	lcd_string_xy(0,0,"Girish  Kotalwar");
	lcd_cmd(0xC0);
	lcd_string_xy(1,5,"Latur");
	delay(1000);
	
	while(1);
}	