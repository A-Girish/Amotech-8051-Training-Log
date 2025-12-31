#include <reg51.h>
#define lcdport P3
sbit rs = P3^2;
sbit en = P3^3;

sbit col1 = P2^3;
sbit col2 = P2^2;
sbit col3 = P2^1;
sbit col4 = P2^0;
sbit row1 = P2^7;
sbit row2 = P2^6;
sbit row3 = P2^5;
sbit row4 = P2^4;

char op = 0;
unsigned long operand_A = 0;
unsigned long operand_B = 0; 
unsigned long current_value = 0; 
unsigned long max_value = 0; 
unsigned char mode = 0;
unsigned char mode_screen_shown = 0;
unsigned char entering_B = 0;

void delay(int d)
{
	int i,j; 
	for(i=0;i<d;i++)
		for(j=0;j<1275;j++);
}

void lcd_enable()
{	
	en=1;
	delay(2);
	en=0;
}

void lcd_cmd(unsigned char c)
	{
    rs=0;
    lcdport=(lcdport&0x0F)|(c&0xF0); 
		lcd_enable();
    lcdport=(lcdport&0x0F)|((c<<4)&0xF0); 
		lcd_enable();
	}

void lcddata(unsigned char d)
	{
    rs=1;
    lcdport=(lcdport&0x0F)|(d&0xF0); 
		lcd_enable();
    lcdport=(lcdport&0x0F)|((d<<4)&0xF0); 
		lcd_enable();
		
	}

void lcd_string(unsigned char *str)
{
	int i;
	for(i=0;str[i]!=0;i++)
	{
		lcddata(str[i]);
		delay(10);
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

void lcd_print_int(unsigned long value)
{
    char buf[11];
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

void mode_select(char key){
    if(!mode_screen_shown)
		{
      lcd_cmd(0x01); lcd_string("SELECT MODE");
      lcd_cmd(0xC0); lcd_string("A=8 B=16 C=32");
      mode_screen_shown = 1;
    }
    if(key == 'A')
		{
			mode=1;
			max_value=255;
			lcd_cmd(0x01);
			lcd_string("8 bit Mode");
			delay(200);
			lcd_cmd(0x01);
		}
    else if(key == 'B')
		{
			mode=2;
			max_value=65535;
			lcd_cmd(0x01);
			lcd_string("16 bit Mode");
			delay(200);
			lcd_cmd(0x01);
		}
    else if(key == 'C')
		{
			mode=3;
			max_value=4294967295UL;
			lcd_string("32 bit Mode");
			delay(200);
			lcd_cmd(0x01);
		}
    else return;

    mode_screen_shown=0;
}

void calculator(char key)
{
    unsigned char digit; 
		unsigned long result = 0;

    if(key >= '0' && key <= '9')
			{
        digit = key - '0';
			
        if(current_value > (max_value - digit) / 10)
				{
          lcd_cmd(0x01); 
					lcd_cmd(0x80); 
					lcd_string("OVERFLOW");
          current_value=0; 
					entering_B=0; 
					return;
        }
        current_value = current_value * 10 + digit;
        if(entering_B) 
				{
					lcd_cmd(0xC0); 
				}
				else 
        {
					lcd_cmd(0x80);
				}
				lcd_print_int(current_value);
			}		

    if((key == 'A'||key == 'B'||key == 'C'||key == 'D') && current_value)
			{
        operand_A = current_value; 
				current_value = 0; 
				op = key; 
				entering_B=1;
        lcd_cmd(0x01); 
				lcd_cmd(0x80);
        lcd_print_int(operand_A); 
				lcddata(' '); 
				lcddata(op);
        lcd_cmd(0xC0);
			}

    if(key == '#')
		{
        operand_B=current_value;
        if(op == 'A') result = operand_A + operand_B;
        if(op == 'B') result = operand_A - operand_B;
        if(op == 'C') result = operand_A * operand_B;
        if(op == 'D' && operand_B == 0)
				{
					lcd_cmd(0x01);
					lcd_string("DIV ERROR");
					return;
				}
        if(op == 'D') result = operand_A / operand_B;

        if(result > max_value)
				{
					lcd_cmd(0x01);
					lcd_string("OVERFLOW");
					current_value=0; 
					return;
				}

        lcd_cmd(0x01); 
				lcd_string("RESULT:");
        lcd_cmd(0xC0); 
				lcd_print_int(result);
        operand_A = operand_B = current_value = 0; 
				entering_B=0; 
				op=0;
    }

    if(key == '*')
			{
        lcd_cmd(0x01);
        operand_A = operand_B = current_value = 0;
        entering_B = 0; 
				op = 0; 
				mode = 0; 
				mode_screen_shown = 0;
			}
}

void main()
{
    char key ;
    lcd_init();
		lcd_cmd(0x80);
		lcd_string("    SIMPLE  ");
		lcd_cmd(0xC0);
		lcd_string("  CALCULATOR  ");
		delay(300);
		lcd_cmd(0x01);
		lcd_string("A = +  B = -");
		lcd_cmd(0xC0);
		lcd_string("C = x  D = / ");
		delay(300);
		lcd_cmd(0x01);
		lcd_string("# = Calculate");
		lcd_cmd(0xC0);
		lcd_string("* = Clear Screen");
		delay(300);
		lcd_cmd(0x01);
	
    while(1)
			{ 
        key = keypad_key();
        if(mode == 0)
				{
					mode_select(key);
				}
				else if(key)
				{
					calculator(key);
				}

			}
}
