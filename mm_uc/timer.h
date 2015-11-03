#include <avr/interrupt.h>
unsigned volatile int mstime;

/* Interrupt based timer
	Credit: https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
*/

ISR (TIMER0_COMPA_vect){
	mstime+=4; 
}

void timer0_Init(void){
    TCCR0A |= (1 << WGM01);
    OCR0A = 0xF9;
    TIMSK0 |= (1 << OCIE0A);
    sei();         
    TCCR0B |= (1 << CS02);  
}


