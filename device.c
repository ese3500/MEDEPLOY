#define F_CPU 16000000UL
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util./delay.h>
#include <stdbool.h>
#include <stdlib.h>

#include <avr/io.h>
#include "lib\ST7735.h"
#include "lib\LCD_GFX.h"

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include "uart.h"
char String[25];


int num = 1;
int promptnum = 1;

void Initialize()
{
	cli();
	PORTB &= ~(1<<PORTB4); //
	PORTD &= ~(1<<PORTD5); 
	PORTD &= ~(1<<PORTD7); //select button
	
	//esp outputs
	DDRD |= (1<<DDD2); //lsb
	DDRD |= (1<<DDD3); //
	DDRD |= (1<<DDD4);//msb 
	//DDRD |= (1<<DDD3); //signal bit
	
	//start high
	PORTD |= (1<<PORTD2);
	PORTD |= (1<<PORTD3);
	PORTD |= (1<<PORTD4);
	
	
	UART_init(BAUD_PRESCALER); 
	
	sprintf(String, "init");
	UART_putstring(String);
		
	lcd_init();
	sei();
}

int main(void)
{
	Initialize();
	
	LCD_setScreen(WHITE);
	
	prompt(0);
	options(0);
	
	while (1)
	{
		if (PIND & (1<<PIND7)) {
			int promptinput = promptnum;
			if (promptinput == 3){
				_delay_ms(200);
				LCD_setScreen(WHITE);
				char* s = "Thank you for answering.";
				LCD_drawString(10, 18, s, BLUE, WHITE);
				
				char* s1 = "Help is on the way!";
				LCD_drawString(10, 30, s1, BLUE, WHITE);
				
				break;
			}
			prompt(promptinput);
			
			send(num % 5);
			sprintf(String, "input 1: %d", num % 5);
			UART_putstring(String);		
			promptnum += 1;
			
		}
		if (PINB & (1<<PINB4)) {
			int input = num % 5;
			options(input);
			num +=1;
		}
		
	}
}

void options(int num) {
	LCD_drawLine(0, 75, LCD_WIDTH, 75, WHITE);
	if (num == 0) {
		LCD_drawLine(20,75,35,75,BLACK);
		} else if (num == 1) {
		LCD_drawLine(45,75,60,75,BLACK);
		} else if (num == 2) {
		LCD_drawLine(70,75,85,75,BLACK);
		} else if (num == 3) {
		LCD_drawLine(95,75,110,75,BLACK);
		} else if (num == 4) {
		LCD_drawLine(120,75,135,75,BLACK);
	}
	char* n1 = "1";
	LCD_drawString(25, 64, n1, BLUE, WHITE);
	char* n2 = "2";
	LCD_drawString(50, 64, n2, BLUE, WHITE);
	char* n3 = "3";
	LCD_drawString(75, 64, n3, BLUE, WHITE);
	char* n4 = "4";
	LCD_drawString(100, 64, n4, BLUE, WHITE);
	char* n5 = "5";
	LCD_drawString(125, 64, n5, BLUE, WHITE);
}

void prompt(int num) {
	LCD_drawBlock(0,0,LCD_WIDTH, 60, WHITE);
	if (num == 0) {
		char* s = "Rate your pain";
		LCD_drawString(40, 18, s, BLUE, WHITE);
		
		char* s1 = "(least) 1 - 5 (most)";
		LCD_drawString(21, 30, s1, BLUE, WHITE);
		} else if (num == 1) {
		char* s = "Help needed?";
		LCD_drawString(40, 18, s, BLUE, WHITE);
		
		char* s1 = "(none) 1 - 5 (fast)";
		LCD_drawString(21, 30, s1, BLUE, WHITE);
		}
		
		
	
}
void send(int num){
	PORTD &= ~(0x7 << PORTD2);
	PORTD |= (num & 0x7) << PORTD2;
	

	if (num == 1){ //001
		sprintf(String, "1");
		
		PORTD |= (1<<PORTD2);
		PORTD &= ~(1<<PORTD3);
		PORTD &= ~(1<<PORTD4);
	}else if (num == 2){ //010
		sprintf(String, "2");
		PORTD &= ~(1<<PORTD2);
		PORTD |= (1<<PORTD3);
		PORTD &= ~(1<<PORTD4);
	}
	else if (num == 3){ //011
		sprintf(String, "3");
		PORTD |= (1<<PORTD2);
		PORTD |= (1<<PORTD3);
		PORTD &= ~(1<<PORTD4);
	}
	else if (num == 4){ //100
		sprintf(String, "4");
		PORTD &= ~(1<<PORTD2);
		PORTD &= ~(1<<PORTD3);
		PORTD |= (1<<PORTD4);
	}
	else if (num == 0){ //101
		sprintf(String, "1");
		PORTD |= (1<<PORTD2);
		PORTD &= ~(1<<PORTD3);
		PORTD |= (1<<PORTD4);
	}else{
		//start low
		sprintf(String, "else");
		PORTD &= ~(1<<PORTD2);
		PORTD &= ~(1<<PORTD3);
		PORTD &= ~(1<<PORTD4);
	}
	UART_putstring(String);
	
	_delay_ms(20);
	//wait high
	PORTD |= (1<<PORTD2);
	PORTD |= (1<<PORTD3);
	PORTD |= (1<<PORTD4);
}
