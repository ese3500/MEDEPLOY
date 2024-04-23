/*
 * ese3500fp.c
 *
 * Created: 4/11/2024 10:52:05 AM
 * Author : aaront05
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU/(BAUD_RATE*16UL)))-1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


volatile int x = 500;
volatile int y = 500;

void Initialize() {
	
	cli();
	// configure communication pin for forwards backwards
	DDRD |= (1<<DDRD4);
	DDRD |= (1<<DDRD2);
	DDRD |= (1<<DDRD3);
	DDRB &= ~(1<<DDRB0);
	DDRB |= (1<<DDRB2);
	
	//ADC
	PRR0 &= ~(1 << PRADC);

	// Vref = AVcc
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);

	// prescale ADC clock by 128
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);

	// PC0
	ADMUX |= (1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);

	// Autotriggering of ADC
	ADCSRA |= (1 << ADATE);

	// Free running mode
	ADCSRB &= ~(1 << ADTS0);
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB &= ~(1 << ADTS2);

	DIDR0 |= (1 << ADC0D);

	// Enable ADC
	ADCSRA |= (1 << ADEN);

	ADCSRA |= (1 << ADSC);

	//Timer 1
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	TCCR1B |= (1<<ICES1);
	
	TIFR1 |= (1<<ICF1);
	TIMSK1 |= (1<<ICIE1);
	TIMSK1 |= (1<<TOIE1);
	
	sei();
}

int main(void)
{
	Initialize();
    while (1) 
    {
		if (!(ADMUX & (1 << MUX0))) {
			y = ADC;
		} else {
			x = ADC;
		}
		if (y >= 900 && x > 300 && x < 700) {
			// forwards 100
			PORTD |= (1<<PORTD4);
			PORTD &= ~(1<<PORTD2);
			PORTD &= ~(1<<PORTD3);
		} else if (y <= 200 && x > 300 && x < 700) {
			// backwards 010
			PORTD |= (1<<PORTD2);
			PORTD &= ~(1<<PORTD4);
			PORTD &= ~(1<<PORTD3);
		} else if (x >= 900 && y > 300 && y < 700) {
			// left 110
			PORTD |= (1<<PORTD4);
			PORTD |= (1<<PORTD2);
			PORTD &= ~(1<<PORTD3);
		} else if (x <= 200 && y > 300 && y < 700) {
			// right 101
			PORTD &= ~(1<<PORTD2);
			PORTD |= (1<<PORTD4);
			PORTD |= (1<<PORTD3);
		} else {
			// off 000
			PORTD &= ~(1<<PORTD2);
			PORTD &= ~(1<<PORTD4);
			PORTD &= ~(1<<PORTD3);
		}
		ADMUX ^= (1 << MUX0);
		_delay_ms(100);
    }
}

ISR(TIMER1_CAPT_vect) {
	PORTB ^= (1<<PORTB2);
}
