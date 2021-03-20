/*
 * AVR_multimeter.c
 *
 * Created: 2021-03-08 14:52:24
 * Author : mahad
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

uint32_t wgnd;
int cntr;

ISR(INT0_vect){
	if(cntr > 25){
		PORTB ^= (1 << PORTB5);
		cntr = 0;
	}
	else{
		wgnd = wgnd << cntr;
		cntr++;
	}
}

ISR(INT1_vect){
	if(cntr > 25){
		cntr = 0;
		PORTB ^= (1 << PORTB5);
	}
	else{
		if(cntr == 0){	wgnd |= 1; }
		else if(wgnd == 0){ wgnd = wgnd | (1 << cntr); }
		else{ wgnd = wgnd | (wgnd << cntr); }
		cntr++;
	}
}

void begin_wiegand(int pin0, int pin1){
	DDRD = 0x00;
	
	DDRB |= 0B100000;
	PORTB |= 0B100000;

	PORTD = (1 << pin0) | (1 << pin1);
	EIMSK = (1 << INT1) | (1 << INT0);
	sei();	
}

uint16_t cardData(void){
	return (uint16_t) wgnd;
}

int main(void)
{
	begin_wiegand(2,3);
    while (1) 
    {
		_delay_ms(10);
    }
}

