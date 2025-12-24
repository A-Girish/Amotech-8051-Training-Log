#include <reg51.h>

#define leds P0
sbit led = P1^0;

void delay(unsigned int del)
{
	int i,j;
	for(i=0;i<del;i++)
		for(j=0;j<112;j++);
}
void main(void)
{
	while(1){
		led = 0x00;
		leds = 0x00;
		delay(1000);
		leds = 0xFF;
		delay(1000);
	}
}