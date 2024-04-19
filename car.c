/*
 * ese3500fp.c
 *
 * Created: 4/11/2024 10:52:05 AM
 * Author : aaront05
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util./delay.h>
#include <stdbool.h>
#include <stdlib.h>

void Initialize() {
	
	cli();
	// servo motor output pin for PWM
	DDRD |= (1<<DDD5);
	DDRB &= ~(1<<DDB0);
	
	// pre-scale timer 0 clock by 1024
	TCCR0B |= (1<<CS02);
	TCCR0B &= ~(1<<CS01);
	TCCR0B |= (1<<CS00);
	// set timer 0 to Phase Correct PWM mode
	TCCR0A |= (1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0A |= (1<<WGM02);
	TCCR0A |= (1<<COM0A0);
	TCCR0B |= (1<<WGM02);
	TCCR0A |= (1<<COM0B1);
	
	OCR0A = 157;
	OCR0B = 0;
	
	// set up motor pins
	// right
	DDRD |= (1<<DDRD2);
	DDRD |= (1<<DDRD3);
	
	// left
	DDRD |= (1<<DDRD7);
	DDRD |= (1<<DDRD1);
	
	//set up esp32 input pins
	DDRD &= ~(1<<DDRD4);
	DDRB &= ~(1<<PINB2);
	DDRB &= ~(1<<DDRB1);
	
	// prescale clock
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	// enable clock
	TCCR1B |= (1<<ICES1);
	
	// clear input capture flags
	TIFR1 |= (1<<ICF1);
	
	// enable input capture interrupt (rising edge initially)
	TIMSK1 |= (1<<ICIE1);
	
	
	sei();
}

int main(void)
{
	Initialize();
	while (1)
	{
		if ((PIND & (1<<PIND4)) && !(PINB & (1<<PINB2)) && !(PINB & (1<<PINB1))) {
			// forwards
			PORTD |= (1<<PORTD0);
			PORTD &= ~(1<<PORTD1);
			
			PORTD |= (1<<PORTD2);
			PORTD &= ~(1<<PORTD3);
			} else if ((PINB & (1<<PINB2)) && !(PIND & (1<<PIND4)) && !(PINB & (1<<PINB1))) {
			// backwards
			PORTD |= (1<<PORTD1);
			PORTD &= ~(1<<PORTD0);
			
			PORTD |= (1<<PORTD3);
			PORTD &= ~(1<<PORTD2);
			} else if ((PINB & (1<<PINB2)) && (PIND & (1<<PIND4)) && !(PINB & (1<<PINB1))) {
			// right
			PORTD &= ~(1<<PORTD1);
			PORTD &= ~(1<<PORTD0);
			
			PORTD |= (1<<PORTD2);
			PORTD &= ~(1<<PORTD3);
			}else if (!(PINB & (1<<PINB2)) && (PIND & (1<<PIND4)) && (PINB & (1<<PINB1))) {
			
			// left
			PORTD |= (1<<PORTD0);
			PORTD &= ~(1<<PORTD1);
			
			PORTD &= ~(1<<PORTD3);
			PORTD &= ~(1<<PORTD2);
			}else {
			// off
			PORTD &= ~(1<<PORTD1);
			PORTD &= ~(1<<PORTD0);
			
			PORTD &= ~(1<<PORTD3);
			PORTD &= ~(1<<PORTD2);
		}
	}
}

ISR(TIMER1_CAPT_vect) {
	if (TCCR1B & (1<<ICES1)) {
		DDRD |= (1<<DDRD5);
		OCR0B = 11;
		_delay_ms(5000);
		DDRD &= ~(1<<DDRD5);
	} else {
		DDRD |= (1<<DDRD5);
		OCR0B = 13;
		_delay_ms(2500);
		DDRD &= ~(1<<DDRD5);
	}
	// toggle rising/falling edge capture
	TCCR1B ^= (1<<ICES1);
}
