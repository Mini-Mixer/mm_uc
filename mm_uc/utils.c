#define DELTA 1000

void pumpOn(pump *pmp){
	pmp->finished = 0;
	UART_Transmit(pmp->label);
	UART_TString(" is on!");
	switch(pmp->label){
		case 'A':
			DDRB |= 1<<PB3;
			break;
		case 'B':
			DDRB |= 1<<PB2;
			break;
		case 'C':
			DDRB |= 1<<PB1;
			break;
		case 'D':
			DDRD |= 1<<PD3;
			break;
		case 'E':
			DDRD |= 1<<PD5;
			break;
		case 'F':
			DDRD |= 1<<PD6;
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
			DDRB &= ~(1<<PB3);
			break;
		case 'B':
			DDRB &= ~(1<<PB2);
			break;
		case 'C':
			DDRB &= ~(1<<PB1); 
			break;
		case 'D':
			DDRD &= ~(1<<PD3);
			break;
		case 'E':
			DDRD &= ~(1<<PD5);
			break;
		case 'F':
			DDRD &= ~(1<<PD6);
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
		float totalDuration = mstime + (float)((req->pumps[i].duration)*1000);
		pumpOn(&(req->pumps[i]));
		while(mstime < totalDuration){ 
			/* Something can be ran here if necessary */ 
		}
		pumpOff(&(req->pumps[i]));
	}	
}
