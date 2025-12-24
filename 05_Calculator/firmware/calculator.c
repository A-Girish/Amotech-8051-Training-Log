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

int num1 = 0, num2= 0;
bit second = 0;
char op;

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

char keypad_key(void)
{
    col1 = 0; col2 = col3 = col4 = 1;
    if(row1 == 0){ while(!row1); return '1'; }
    if(row2 == 0){ while(!row2); return '4'; }
    if(row3 == 0){ while(!row3); return '7'; }
    if(row4 == 0){ while(!row4); return '*'; }

    col2 = 0; col1 = col3 = col4 = 1;
    if(row1 == 0){ while(!row1); return '2'; }
    if(row2 == 0){ while(!row2); return '5'; }
    if(row3 == 0){ while(!row3); return '8'; }
    if(row4 == 0){ while(!row4); return '0'; }

    col3 = 0; col1 = col2 = col4 = 1;
    if(row1 == 0){ while(!row1); return '3'; }
    if(row2 == 0){ while(!row2); return '6'; }
    if(row3 == 0){ while(!row3); return '9'; }
    if(row4 == 0){ while(!row4); return '#'; }

    col4 = 0; col1 = col2 = col3 = 1;
    if(row1 == 0){ while(!row1); return 'A'; }
    if(row2 == 0){ while(!row2); return 'B'; }
    if(row3 == 0){ while(!row3); return 'C'; }
    if(row4 == 0){ while(!row4); return 'D'; }

    return 0;   // no key pressed
}

void lcd_print_int(int value)
{
    char buf[6];
    int i = 0;

    if(value == 0)
    {
        lcddata('0');
        return;
    }

    if(value < 0)
    {
        lcddata('-');
        value = -value;
    }

    while(value > 0)
    {
        buf[i++] = (value % 10) + '0';
        value = value / 10;
    }

    while(i--)
        lcddata(buf[i]);
}

void calculator(char key)
{
    if(key >= '0' && key <= '9')
    {
        if(second == 0)
            num1 = num1 * 10 + (key - '0');
        else
            num2 = num2 * 10 + (key - '0');
        lcddata(key);
    }

    if(key == 'A' || key == 'B' || key == 'C' || key == 'D')
    {
        op = key;
        second = 1;
        lcd_cmd(0xC0);
        lcddata(key);
    }

    if(key == '#')
    {
        int result = 0;

        if(op == 'A') result = num1 + num2;
        if(op == 'B') result = num1 - num2;
        if(op == 'C') result = num1 * num2;
        if(op == 'D') result = num1 / num2;

        lcd_cmd(1);
        lcd_string("Result:");
        lcd_cmd(0xC0);
        lcd_print_int(result);

        num1 = num2 = 0;
        second = 0;
    }
		
		if(key == '*')
		{
			lcd_cmd(0x01);
			num1 = num2 = 0;
			second = 0;
		}
		
}

void main(void)
{
	char key;
	
	lcd_init();
	lcd_cmd(0x80);
	lcd_string("    SIMPLE  ");
	lcd_cmd(0xC0);
	lcd_string("  CALCULATOR  ");
	delay(100);
	lcd_cmd(0x01);
	lcd_string("A = +  B = -");
	lcd_cmd(0xC0);
	lcd_string("C = x  D = / ");
	delay(500);
	lcd_cmd(0x01);
	lcd_string("# = Calculate");
	lcd_cmd(0xC0);
	lcd_string("* = Clear display");
	delay(500);
	lcd_cmd(0x01);
	
	
	while(1)
  {
        key = keypad_key();
        if(key)
            calculator(key);
  }
}
