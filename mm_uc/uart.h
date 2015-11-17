void UART_Transmit(unsigned char);
unsigned char UART_Receive(void);
void UART_Flush(void);


/* 
	UART_TString(char*)
	Transmits a string via UART
*/
void UART_TString(char* string){
	uint8_t index = 0;
	while(string[index]){
		UART_Transmit((unsigned char)string[index]);
		index++;
	}
}

/* 
	CRLF(void)
	Used for newlines (Carriage Return/Line Feed, reduces code 
*/
void CRLF(){
	UART_Transmit(0x0D);
	UART_Transmit(0x0A);
}

/* 
	println(char*)
	Utilizes both CRLF and UART_TString to transmit a string with a newline
	autmatically following after
*/
void println(char* string){
	UART_TString(string);
	CRLF();
}


/* 
 Basic UART functions provided by Atmel 
 http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf
*/


void UART_Init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void UART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char UART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}

void UART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

