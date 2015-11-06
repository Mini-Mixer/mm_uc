#define F_CPU 16000000UL
#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/(BAUD-1)
#define NULL 0
#define MODEDELAY 1000
#define MAXPUMPS 6

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "uart.h"
#include "timer.h"
#include "pump.h"
#include "utils.c"

void init(void){
	UART_Init(MYUBRR);
	timer0_Init();
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	TCCR2B = _BV(CS22);
	OCR2B = 255; 
	OCR2A = 255; 
	DDRD |= 1<<PD3; // OC2B
	DDRD |= 1<<PB3; // OC2B
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
	while(1){	
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
			
	}
	return 0;
}



