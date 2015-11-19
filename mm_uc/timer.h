#include <avr/interrupt.h>
volatile float mstime;

/* Interrupt based timer
	Credit: https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
*/

ISR (TIMER0_COMPA_vect){
	mstime+=1.6; 
}


void timer0_Init(void){
    TCCR0A |= (1 << WGM01);
    TIMSK0 |= (1 << OCIE0A);
    sei();         
    TCCR0B |= (1 << CS02);  
}


void timer_init(void){
	timer0_Init();
	
	// All PWM generators are set to not output on initial setup
	DDRD &= ~(1<<PD3); // A
	DDRD &= ~(1<<PD5); // B
	DDRD &= ~(1<<PD6); // C
	DDRB &= ~(1<<PB1); // D
	DDRB &= ~(1<<PB2); // E
	DDRB &= ~(1<<PB3); // F
	
	// Configure timer registers
	// Timer 0
	TCCR0A |= (1 << COM0A0);
	TCCR0B |= (1 << WGM02);
	
	// Timer 1
	
	// Set OC1x behavior for inverting mode
	TCCR1A |= (1<<COM1A0);
	TCCR1A |= (1<<COM1A1);
	TCCR1A |= (1<<COM1B0);
	TCCR1A |= (1<<COM1B1);
	
	// Configure the 16 bit timer to operate in the 8 bit Fast-PWM	mode
	TCCR1A |= (1<<WGM10);
	//TCCR1A &= ~(1<<WGM11);
	TCCR1B |= (1<<WGM12);
	//TCCR1B &= ~(1<<WGM13);

	// Set the clock source to the internal clock with a prescale divisor of 64
	TCCR1B |= (1<<CS10);
	TCCR1B |= (1<<CS11);
	//TCCR1B &= ~(1<<CS12);
	
	
	//Timer 2
	TCCR2A |= (1<<COM2A1);
	TCCR2A |= (1<<COM2A0);
	TCCR2A |= (1<<COM2B1);
	TCCR2A |= (1<<COM2B0);
	
	TCCR2A |= (1<<WGM21);
	TCCR2A |= (1<<WGM20);
	TCCR2B |= (1<<CS22);
}