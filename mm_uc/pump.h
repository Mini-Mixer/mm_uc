/* pump and request structs */

typedef struct _pump {
	unsigned char label;
	float amount;
	float duration;
	uint8_t finished;
} pump;

/*  A second struct in addition to the pump struct is required 
	This is because of the limitations of static variables in C */
typedef struct _request{
	pump* pumps;
	uint8_t parPumps;
	uint8_t seqPumps;	
} request;