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
	
	PORTB &= ~(1<<PORTB7); //built in button for reset
	
	//esp outputs
	DDRD |= (1<<DDD2); //lsb
	DDRD |= (1<<DDD3); //
	DDRD |= (1<<DDD4);//msb 
	//DDRD |= (1<<DDD3); //signal bit
	
	//wait high
	PORTD |= (1<<PORTD2);
	PORTD |= (1<<PORTD3);
	PORTD |= (1<<PORTD4);
	
	// Setup for ADC (10bit = 0-1023)
	// Clear power reduction bit for ADC
	PRR0 &= ~(1 << PRADC);

	// Select Vref = Internal 1.1V reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	// Set the ADC clock div by 128
	// 16M/128=125kHz
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);
	
	// Select ADC0
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	
	ADCSRA |= (1 << ADATE);   // Autotriggering of ADC
	
	// Free running mode ADTS[2:0] = 000
	ADCSRB &= ~(1 << ADTS0);
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB &= ~(1 << ADTS2);
	
	// Disable digital input buffer on ADC pin
	DIDR0 |= (1 << ADC0D);

	// Enable ADC
	ADCSRA |= (1 << ADEN);

	// Start conversion
	ADCSRA |= (1 << ADSC);

	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS12);

	
	// Set Timer 1 to Normal
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B &= ~(1 << WGM12);
	TCCR1B &= ~(1 << WGM13);

	
	UART_init(BAUD_PRESCALER); 
	
	sprintf(String, "init");
	UART_putstring(String);
		
	lcd_init();
	sei();
}

int main(void)
{
	Initialize();
	
	volatile int hr = 0;
	volatile unsigned int last_peak_time = 0;
	volatile unsigned int peak_time = 0;
	volatile unsigned int bpm = 60;
	int peak_detected = 0;

	LCD_setScreen(WHITE);
	
	/*
	while(1){
		
		uint8_t value = 49; //test
		PORTD = ((value & 0x07) << PORTD2);
		
		
		sprintf(String, "LSB, %d", PORTD);
		UART_putstring(String);
		
		_delay_ms(2000);
		PORTD = (((value >> 3) & 0x07) << PORTD2);
		
		
		sprintf(String, "MSB, %d", PORTD);
		UART_putstring(String);
		_delay_ms(2000);
	}
	*/
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
				
				while (1){
					hr = ADC;
					//sprintf(String, "ADC: %d", hr);
					//UART_putstring(String);
					
					
					if (hr < 512){
						//less than 2.5V, probably not reading any signal
						sprintf(String, "No Pulse Detected. \n");
						UART_putstring(String);
						uint8_t value = 0; //test
						PORTD = ((value & 0x07) << PORTD2);
						
						
						sprintf(String, "LSB, %d", PORTD);
						UART_putstring(String);
						
						_delay_ms(2000);
						PORTD = (((value >> 3) & 0x07) << PORTD2);
						
						
						sprintf(String, "MSB, %d", PORTD);
						UART_putstring(String);
						_delay_ms(2000);
						}else{
						// Peak detection logic
						if (hr > 700) {
							
							peak_detected = 1;
							peak_time = TCNT1;  // Read current Timer1 count
							//sprintf(String, "Peak detected, %d, %d", last_peak_time, peak_time);
							//UART_putstring(String);
							volatile unsigned int period = peak_time - last_peak_time;
							
							bpm = 30 + 6 / ((period * 64.0 / F_CPU));  // Calculate BPM
							sprintf(String, "bpm: %d", bpm);
							UART_putstring(String);
							
							last_peak_time = peak_time;
							
							
							//send 32 bit
							uint8_t value = 60; //test;
							if (bpm >= 120 || bpm <= 40){
								value = bpm;
							}
							PORTD = ((value & 0x07) << PORTD2);
							
							
							sprintf(String, "LSB, %d", PORTD);
							UART_putstring(String);
							
							_delay_ms(2000);
							PORTD = (((value >> 3) & 0x07) << PORTD2);
							
							
							sprintf(String, "MSB, %d", PORTD);
							UART_putstring(String);
							_delay_ms(2000);
							
							
			
						}
						
					}
					_delay_ms(200);  // Delay for stability, adjust as needed
				}
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
	//PORTD &= ~(0x7 << PORTD2);
	//PORTD |= (num & 0x7) << PORTD2;
	

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
