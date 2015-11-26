#define F_CPU 16000000UL //16MHz
#define FOSC 16000000 // Clock Speed
#define BAUD 9600 // Serial Baud Rate
#define MYUBRR FOSC/16/(BAUD-1)
#define NULL 0 
#define MODEDELAY 1000
#define MAXPUMPS 6 // Mini-Mixer only supports 6 pumps
#define A2DRATIO 5.9 // Conversion factor between amount and duration

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "uart.h"
#include "timer.h"
#include "pump.h"
#include "utils.c"

void init(void){
	UART_Init(MYUBRR);
	timer_init();

	// Set the counter limit
	OCR2B = 200; //A 
	OCR0B = 200; //B
	OCR0A = 200; //C	
	OCR1A = 200; //D
	OCR1B = 200; //E
	OCR2A = 200; //F

	// Turn on LED on pin 19 (PB5)
	DDRB |= 1<<PB5;

}

request* processRequest(){
	pump* pumps = (pump*)malloc(sizeof(pump)*6);
	UART_TString("OK");
	unsigned char parPumps = UART_Receive();
	UART_TString("OK");
	unsigned char seqPumps = UART_Receive();
	parPumps -= 48; seqPumps -= 48;
	
	// Error checking
	if(parPumps + seqPumps > MAXPUMPS)
		return NULL;
					
	if(parPumps > MAXPUMPS || parPumps < 0)
		return NULL;
		
	if(seqPumps > MAXPUMPS || seqPumps < 0)
		return NULL;
	
	
	/* Handle request */
	
	// Process parallel pump information
	uint8_t i = 0;
	if(parPumps > 0){
		for(; i < parPumps; i++){
			UART_TString("OK");
			// Get label info
			pumps[i].label = UART_Receive();
			if(pumps[i].label < 65 || pumps[i].label > 70 )
				return NULL;
			UART_TString("OK");
			// Get amount info
			uint8_t ones = UART_Receive() - 48;
			if(ones < 0 || ones > 8 )
				return NULL;
			if(UART_Receive() != '.')
				return NULL;			
			uint8_t tens = UART_Receive() - 48;
			if(tens < 0 || tens > 9 )
				return NULL;			
			pumps[i].amount = (float)ones + (float)(tens)*0.1;
			if(pumps[i].amount < 0 || pumps[i].amount > 8)
				return NULL;
		}
	}
	// Process sequential pump information 
	uint8_t j = i + seqPumps;
	if(seqPumps > 0){
		for(; i < j; i++){
			UART_TString("OK");
			// Get label info
			pumps[i].label = UART_Receive();
			if(pumps[i].label < 65 || pumps[i].label > 70)
				return NULL;
			UART_TString("OK");
			// Get amount info
			uint8_t ones = UART_Receive() - 48;
			if(ones < 0 || ones > 8 )
				return NULL;
			if(UART_Receive() != '.')
				return NULL;			
			uint8_t tens = UART_Receive() - 48;
			if(tens < 0 || tens > 9 )
				return NULL;			
			pumps[i].amount = (float)ones + (float)(tens)*0.1;
			if(pumps[i].amount < 0 || pumps[i].amount > 8)
				return NULL;
		}
	}
	
	// Convert all amounts to durations
	for(uint8_t index = 0; index < j; index++)
		pumps[index].duration = pumps[index].amount * A2DRATIO;

	
	UART_TString("OK");
	
	/* Create and return the request */
	
	request* req = (request*)malloc(sizeof(request));
	req->pumps = pumps;
	req->parPumps = parPumps;
	req->seqPumps = seqPumps;
	return req;
}



/* 
	TODO: 
	* As more commands are added, replace if/else chain with switch/case
	* Tweak all delay values and scalar conversions to fit the requirements
	* Change UART transmitted messages as necessary 
*/

/* Entry point */
int main(void){
	init();
	request* req;
	/* Primary loop */
	// Infinite loop is justified in this situation
	for(;;){	
		req = NULL;
		println("Ready.");
		unsigned char temp = UART_Receive();
		if(temp == 'R'){
			if((req = processRequest()) == NULL){
				CRLF();
				println("ERROR: Failed to process request!");
				continue;
			}
			CRLF();
			if(req->parPumps > 0){
				parallelDispense(req);
				_delay_ms(MODEDELAY); // Delay between modes, will tweak
			}
			if(req->seqPumps > 0){
				sequentialDispense(req);
				_delay_ms(MODEDELAY); // Delay between modes, will tweak
			}
			println("Request completed");			
		}
		else
			println("Invalid request command");
		
		free(req->pumps); 
		free(req);		
	}
	return 0; // Should never reach this point
}



