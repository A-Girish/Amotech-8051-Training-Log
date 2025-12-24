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

char pass[4],i=0;

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
	int cursor = 192, flag =0;
	lcd_cmd(1);
	lcd_string("Enter Passkey:");
	lcd_cmd(0xC0);
	
	while(i<4)
	{
		flag = cursor;
		
		col1 = 0;
		col2 = col3 = col4 = 1;
		
		if(row1 == 0)
		{
			lcddata('1');
			pass[i++] = '1';
			cursor++;
			while(!row1);
		}
		if(row2 == 0)
		{
			lcddata('4');
			pass[i++] = '4';
			cursor++;
			while(!row2);
		}
		if(row3 == 0)
		{
			lcddata('7');
			pass[i++] = '7';
			cursor++;
			while(!row3);
		}
		if(row4 == 0)
		{
			lcddata('*');
			pass[i++] = '*';
			cursor++;
			while(!row4);
		}	
		
		col2 = 0;
		col1 = col3 = col4 = 1;
		
		if(row1 == 0)
		{
			lcddata('2');
			pass[i++] = '2';
			cursor++;
			while(!row1);
		}
		if(row2 == 0)
		{
			lcddata('5');
			pass[i++] = '5';
			cursor++;
			while(!row2);
		}
		if(row3 == 0)
		{
			lcddata('8');
			pass[i++] = '8';
			cursor++;
			while(!row3);
		}
		if(row4 == 0)
		{
			lcddata('0');
			pass[i++] = '0';
			cursor++;
			while(!row4);
		}
		
		col3 = 0;
		col1 = col2 = col4 = 1;
		if(row1 == 0)
		{
			lcddata('3');
			pass[i++] = '3';
			cursor++;
			while(!row1);
		}
		if(row2 == 0)
		{
			lcddata('6');
			pass[i++] = '6';
			cursor++;
			while(!row2);
		}
		if(row3 == 0)
		{
			lcddata('9');
			pass[i++] = '9';
			cursor++;
			while(!row3);
		}
		if(row4 == 0)
		{
			lcddata('#');
			pass[i++] = '#';
			cursor++;
			while(!row4);
		}
		
		col4 = 0;
		col1 = col2 = col3 = 1;
		if(row1 == 0)
		{
			lcddata('A');
			pass[i++] = 'A';
			cursor++;
			while(!row1);
		}
		if(row2 == 0)
		{
			lcddata('B');
			pass[i++] = 'B';
			cursor++;
			while(!row2);
		}
		if(row3 == 0)
		{
			lcddata('C');
			pass[i++] = 'C';
			cursor++;
			while(!row3);
		}
		if(row4 == 0)
		{
			lcddata('D');
			pass[i++] = 'D';
			cursor++;
			while(!row4);
		}
		
		if(i>0)
		{
			if(flag!=cursor)
			delay(1);
			lcd_cmd(cursor-1);
			lcddata('*');
			
		}
	}
}

void accept()
{
     lcd_cmd(1);
     lcd_string("Welcome");
     lcd_cmd(192);
     lcd_string("Password Accept");
     delay(200);
}

void wrong()
{

    lcd_cmd(1);
    lcd_string("Wrong Passkey");
    lcd_cmd(192);
    lcd_string("PLZ Try Again");
    delay(200);
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
			i=0;
      keypad();
      if(strncmp(pass,"4201",4)==0)
      {
				accept();
				lcd_cmd(1);
				lcd_string("Access Granted ");
				delay(30);
		  }     
      else 
      {
         lcd_cmd(1);
         lcd_string("Access Denied");
         wrong();
         delay(30);
      }
    }
}