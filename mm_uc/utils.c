#define DELTA 1000

void pumpOn(pump *pmp){
	pmp->finished = 0;
	UART_Transmit(pmp->label);
	UART_TString(" is on!");
	switch(pmp->label){
		case 'A':
			OCR2B = 100;
			break;
		case 'B':
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
			OCR2B = 255;
			break;
		case 'B':
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
		if(req->pumps[i].amount > maxTime)
			maxTime = req->pumps[i].amount;
	}
	maxTime *= 1000;
	/* Set total pump time duration and initial time */ 
	unsigned int totalDuration = mstime + (unsigned int)(maxTime);
	unsigned int initialTime = mstime;

	for(uint8_t i = 0; i < req->parPumps; i++)
		pumpOn(&(req->pumps[i])); // Turn the pumps on
	
	while(mstime < (totalDuration + DELTA)){
		for(uint8_t i = 0; i < req->parPumps; i++){
			//println("in this loop");
			if((mstime - initialTime) >= (unsigned int)((req->pumps[i].amount)*1000) ){
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
		unsigned int totalDuration = mstime + (unsigned int)((req->pumps[i].amount)*1000);
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