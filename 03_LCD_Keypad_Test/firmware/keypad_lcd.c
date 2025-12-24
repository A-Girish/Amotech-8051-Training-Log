#include <reg51.h>
#include <string.h>
// LCD Pin Mapping
#define lcdport P3
sbit rs = P3^2;
sbit en = P3^3;

// Keypad Pin Mapping
sbit col1 = P2^3;
sbit col2 = P2^2;
sbit col3 = P2^1;
sbit col4 = P2^0;
sbit row1 = P2^7;
sbit row2 = P2^6;
sbit row3 = P2^5;
sbit row4 = P2^4;

void delay(int del)
{
	int i,j;
	for(i=0;i<del;i++)
	for(j=0;j<1275;j++);
}

void lcd_enable(void)
{
	en = 1;
	delay(10);
	en = 0;
}

void lcd_cmd(unsigned char cmd)
{	
	rs = 0;
	lcdport = (lcdport & 0x0F)|(cmd & 0xF0);
	lcd_enable();
	lcdport = (lcdport & 0x0F)|((cmd << 4) & 0xF0);
	lcd_enable();
}

void lcddata(unsigned char dat)
{
	rs = 1;
	lcdport = (lcdport & 0x0F)|(dat & 0xF0);
	lcd_enable();
	lcdport = (lcdport & 0x0F)|((dat << 4) & 0xF0);
	lcd_enable();
}

void lcd_string(unsigned char *str)
{
	int i;
	for(i=0;str[i]!=0;i++)
	{
		lcddata(str[i]);
		//delay(1);
	}
}

void lcd_init(void)
{
	delay(20);
	lcd_cmd(0x02); // 4bit mode
	lcd_cmd(0x28); // initialization of lcd in 4 bit mode
	lcd_cmd(0x0E); // display on cursor off
	//lcd_cmd(0x06); // auto increment cursor
	lcd_cmd(0x01);	// clear display
	//lcd_cmd(0x80);
}

void keypad()
{
		col1 = 0;
		col2 = col3 = col4 = 1;
		if(row1 == 0){ lcddata('1'); while(!row1); }
		if(row2 == 0){ lcddata('4'); while(!row2); }
		if(row3 == 0){ lcddata('7'); while(!row3); }
		if(row4 == 0){ lcddata('*'); while(!row4); }

		col2 = 0;
		col1 = col3 = col4 = 1;
		if(row1 == 0){ lcddata('2'); while(!row1); }
		if(row2 == 0){ lcddata('5'); while(!row2); }
		if(row3 == 0){ lcddata('8'); while(!row3); }
		if(row4 == 0){ lcddata('0'); while(!row4); }
		
		col3 = 0;
		col1 = col2 = col4 = 1;
		if(row1 == 0){ lcddata('3'); while(!row1); }
		if(row2 == 0){ lcddata('6'); while(!row2); }
		if(row3 == 0){ lcddata('9'); while(!row3); }
		if(row4 == 0){ lcddata('#'); while(!row4); }
	
		col4 = 0;
		col1 = col2 = col3 = 1;
		if(row1 == 0){lcddata('A');while(!row1);}
		if(row2 == 0){lcddata('B');while(!row2);}
		if(row3 == 0){lcddata('C');while(!row3);}
		if(row4 == 0){lcddata('D');while(!row4);}
}

void main(void)
{
	lcd_init();
	
	lcd_cmd(0x80);
	lcd_string("  Hello World!");
	lcd_cmd(0xC0);
	lcd_string("  KEYPAD CHECK");
	delay(100);
	lcd_cmd(0x01);
	
	while(1)
	{
		keypad();
	}
}
