#define DELTA 1000

void pumpOn(pump *pmp){
	pmp->finished = 0;
	UART_Transmit(pmp->label);
	UART_TString(" is on!");
	switch(pmp->label){
		case 'A':
			DDRD |= 1<<PD3;
			break;
		case 'B':
			DDRD |= 1<<PD5;
			break;
		case 'C':
			DDRD |= 1<<PD6;
			break;
		case 'D':
			DDRB |= 1<<PB1;
			break;
		case 'E':
			DDRB |= 1<<PB2;
			break;
		case 'F':
			DDRB |= 1<<PB3;
			break;
		default:
			break;
	}
	CRLF();
}

void pumpOff(pump *pmp){
	pmp->finished = 1;
	UART_Transmit(pmp->label);
	UART_TString(" is off!");
	switch(pmp->label){
		case 'A':
			DDRD &= ~(1<<PD3);
			break;
		case 'B':
			DDRD &= ~(1<<PD5);
			break;
		case 'C':
			DDRD &= ~(1<<PD6); 
			break;
		case 'D':
			DDRB &= ~(1<<PB1);
			break;
		case 'E':
			DDRB &= ~(1<<PB2);
			break;
		case 'F':
			DDRB &= ~(1<<PB3);
			break;
		default:
			break;
	}
	CRLF();
}

void parallelDispense(request* req){
	
	println("Dispensing parallel pumps");
	
	/* Get the duration of the pump that will run the longest */
	float maxTime = 0;
	for(uint8_t i = 0; i < req->parPumps; i++){
		if(req->pumps[i].duration > maxTime)
			maxTime = req->pumps[i].duration;
	}
	maxTime *= 1000;
	/* Set total pump time duration and initial time */ 
	float totalDuration = mstime + (unsigned int)(maxTime);
	float initialTime = mstime;

	for(uint8_t i = 0; i < req->parPumps; i++)
		pumpOn(&(req->pumps[i])); // Turn the pumps on
	
	while(mstime < (totalDuration + DELTA)){
		for(uint8_t i = 0; i < req->parPumps; i++){
			if((mstime - initialTime) >= (float)((req->pumps[i].duration)*1000) ){
				if(req->pumps[i].finished == 0){
					pumpOff(&(req->pumps[i]));
				}
			}
			else{
				UART_Transmit(req->pumps[i].label);
				UART_TString(" is still running");
				CRLF();
			}
		}
	}
	

}

void sequentialDispense(request* req){
	
	println("Dispensing sequential pumps");
	
	uint8_t totalPumps = req->parPumps + req->seqPumps;
	for(uint8_t i = req->parPumps; i < totalPumps; i++){
		unsigned int totalDuration = mstime + (unsigned int)((req->pumps[i].duration)*1000);
		pumpOn(&(req->pumps[i]));
		while(mstime < totalDuration){ }
		pumpOff(&(req->pumps[i]));
	}	
}

/* if(temp > 47 && temp < 58){
UART_Transmit(temp);
CRLF();
temp = temp - 48;
unsigned int duration = mstime + 1000*temp;
//for(; OCR2B >= 100; OCR2B-=10){ _delay_ms(1); }
while(mstime < duration){
}
//for(; OCR2B <= 255; OCR2B++){ _delay_ms(1); }

}
*/