#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_byteorder.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"

/*
The state is represented as an unsigned 64-bit int with each byte interpreted as follows:

most significant byte---------------------------------------------------------------------->least significant byte
7	   |	6	   |	5	   |	4		3		2		1		 0
MACRO	   |	STATE	   |	MICRO	   |	ERROR		^--------------------COUNTER---------------------^
___________|_______________|_______________|_______________________________________________________________________
header	   |	id	   |	<none>	   |
header	   |    timetag    |	<none>     |
	   |		   |		   |
message	   |	size	   |	<none>	   |
	   |		   |		   |
message	   |	address	   |	<none>	   |
message	   |	address	   |	curly br.  |
message	   |	address	   |	square br. |
message	   |	address	   |	comma c.br.|
message	   |	address	   |	dash sq.br.|
message    |	address	   |	null pad   |
	   |		   |		   |
message	   |	typetags   |	<none>	   |
message	   |	typetags   |	null pad   |
	   |   		   |		   |
message	   |	data       |	<none>	   |

The macro states represent the different structural parts of a bundle, namely the HEADER, or the MESSAGE section.

The HEADER macro state has two states representing the different parts of header:  id and timetag
The MESSAGE macro state has four states representing the different parts of a message: size, address, typetags, and data

The message address state has a number of micro states for recording whether or not we're inside brackets, or null padding.
The message typetag state has one micro state to signify that we're in the null padding section.

The counter allows us to keep track of 4-byte alignment and to make sure that the message is the same length as the length field.
When going through the header, the counter is simply incremented since we know how long the two header fields are.
For the size of a message, we use the lowest byte as a counter to record where we are in the size field, while we pack the
size bytes into the upper 3 bytes of the counter.  When we reach the last byte of the size, we put it in the lowest
byte of the counter and then count down as we pass through the rest of the message.  When we get to 0, we've either 
gone off the end of the buffer or started a new message.
 */

#define OSC_STATE_INIT 0ull

// macro states
#define OSC_STATE_HEADER 0x0100000000000000ull
#define OSC_STATE_MESSAGE 0x0200000000000000ull

// header states
#define OSC_STATE_HEADER_ID 0x0001000000000000ull
#define OSC_STATE_HEADER_TIMETAG 0x0002000000000000ull

// message states
#define OSC_STATE_MESSAGE_SIZE 0x0001000000000000ull
#define OSC_STATE_MESSAGE_ADDRESS 0x0002000000000000ull
#define OSC_STATE_MESSAGE_TYPETAGS 0x0004000000000000ull
#define OSC_STATE_MESSAGE_DATA 0x0008000000000000ull

// address micro states
#define OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS 0x0000010000000000ull
#define OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA 0x0000020000000000ull
#define OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS 0x0000040000000000ull
#define OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH 0x0000080000000000ull
#define OSC_STATE_MESSAGE_ADDRESS_NULLPADDING 0x0000100000000000ull

// typetags micro states
#define OSC_STATE_MESSAGE_TYPETAGS_NULLPADDING 0x0000010000000000ull

// error states
#define OSC_STATE_ERROR_BADHEADER 0x0000000100000000ull
#define OSC_STATE_ERROR_NEGATIVESIZE 0x0000000200000000ull
#define OSC_STATE_ERROR_NOLEADINGSLASH 0x0000000300000000ull
#define OSC_STATE_ERROR_NESTEDBRACKETS 0x0000000400000000ull
#define OSC_STATE_ERROR_WILDCARDINSIDEBRACKETS 0x0000000500000000ull
#define OSC_STATE_ERROR_BADALIGNMENT 0x0000000600000000ull
#define OSC_STATE_ERROR_UNSUPPORTEDTYPETAG 0x0000000700000000ull
#define OSC_STATE_ERROR_NOTYPETAGS 0x0000000800000000ull
#define OSC_STATE_ERROR_BUFFEROVERRUN 0x0000000900000000ull
#define OSC_STATE_ERROR_DOUBLEDASH 0x0000000a00000000ull
#define OSC_STATE_ERROR_DOUBLECOMMA 0x0000000b00000000ull

char *_osc_state_errstr[] = {
	"",
	"bad header",
	"size of message is negative",
	"address does not begin with a slash",
	"nested brackets",
	"wildcard inside brackets",
	"data not aligned on 4 byte boundary",
	"unsupported typetag",
	"no typetags (or typetags begin without a comma)",
	"buffer overrun", // can we actually detect this?
	"two dashes in a row within square brackets",
	"two commas in a row within curly brackets"
	};

char *osc_state_errstr(uint64_t err)
{
	err = (err & 0x000000ff00000000) >> 32;
	if(err < sizeof(_osc_state_errstr) / sizeof(char *)){
		return _osc_state_errstr[err];
	}
	return NULL;
}

uint64_t process_byte(char b, uint64_t s)
{
	uint64_t macro = s & 0xff00000000000000;
	uint64_t state = s & 0x00ff000000000000;
	uint64_t micro = s & 0x0000ff0000000000;
	uint64_t error = s & 0x000000ff00000000;
	uint32_t count = s & 0x00000000ffffffff;
	uint32_t ctrm4 = count % 4;

	switch(macro){
	case OSC_STATE_INIT:
		if(b == '#'){
			return OSC_STATE_HEADER | OSC_STATE_HEADER_ID;
		}else{
			return OSC_STATE_ERROR_BADHEADER;
		}
	case OSC_STATE_HEADER:
		switch(state){
		case OSC_STATE_HEADER_ID:
			switch(count){
			case 0:
				if(b == 'b'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 1:
				if(b == 'u'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 2:
				if(b == 'n'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 3:
				if(b == 'd'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 4:
				if(b == 'l'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 5:
				if(b == 'e'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 6:
				if(b == '\0'){
					return s + 1;
				}else{
					return OSC_STATE_HEADER | OSC_STATE_HEADER_ID | OSC_STATE_ERROR_BADHEADER;
				}
			case 7:
				return OSC_STATE_HEADER | OSC_STATE_HEADER_TIMETAG;
			}
		case OSC_STATE_HEADER_TIMETAG:
			switch(count){
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				return s + 1;
			case 7:
				// negative size check
				return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_SIZE | (((int32_t)b) << 24);
			}
		}
	case OSC_STATE_MESSAGE:
		switch(state){
		case OSC_STATE_MESSAGE_SIZE:
			{
				int32_t c = count & 0xff;
				switch(c){
				case 0:
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_SIZE | (((int32_t)b) << 16) | (c + 1);
				case 1:
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_SIZE | (((int32_t)b) << 8) | (c + 1);
				case 2:
					// negative size check
					//count = ntoh32(count | b);
					count = count | b;
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_SIZE | (count - 2);
				default:
					//count = ntoh32(count);
					if(b == '/'){
						return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | (count - 1);
					}else{
						return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | OSC_STATE_ERROR_NOLEADINGSLASH;
					}
				}
			}
		case OSC_STATE_MESSAGE_ADDRESS:
			switch(micro){
			case 0:
				switch(b){
				case '[':
					return (s | OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS) - 1;
				case '{':
					return (s | OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS) - 1;
				case '\0':
					return (s | OSC_STATE_MESSAGE_ADDRESS_NULLPADDING) - 1;
				default:
					return s - 1;
				}
			case OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA:
				switch(b){
				case ',':
					return s | OSC_STATE_ERROR_DOUBLECOMMA;
				}
				// intentional fallthrough to the next case
			case OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS:
				switch(b){
				case '[':
				case ']':
				case '{':
					return s | OSC_STATE_ERROR_NESTEDBRACKETS;
				case '*':
				case '?':
					return s | OSC_STATE_ERROR_WILDCARDINSIDEBRACKETS;
				case ',':
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | OSC_STATE_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA | (count - 1);
				case '}':
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | (count - 1);
				default:
					return s - 1;
				}
			case OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH:
				switch(b){
				case '-':
					return s | OSC_STATE_ERROR_DOUBLEDASH;
				}
				// intentional fallthrough to the next case
			case OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS:
				switch(b){
				case '{':
				case '}':
				case '[':
					return s | OSC_STATE_ERROR_NESTEDBRACKETS;
				case '*':
				case '?':
					return s | OSC_STATE_ERROR_WILDCARDINSIDEBRACKETS;
				case '-':
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | OSC_STATE_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH | (count - 1);
				case ']':
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_ADDRESS | (count - 1);
				default:
					return s - 1;
				}
			case OSC_STATE_MESSAGE_ADDRESS_NULLPADDING:
				if(ctrm4 == 0){
					if(b == ','){
						return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_TYPETAGS | (count - 1);
					}else if(b == '\0'){
						return s | OSC_STATE_ERROR_BADALIGNMENT;
					}else{
						return s | OSC_STATE_ERROR_NOTYPETAGS;
					}
				}else{
					if(b == '\0'){
						return s - 1;
					}else{
						return s | OSC_STATE_ERROR_BADALIGNMENT;
					}
				}
			}
		case OSC_STATE_MESSAGE_TYPETAGS:
			switch(micro){
			case 0:
				switch(b){
				case 'c':
				case 'C':
				case 'u':
				case 'U':
				case 'i':
				case 'I':
				case 'h':
				case 'H':
				case 'f':
				case 'd':
				case 's':
				case 'T':
				case 'F':
				case 'N':
				case OSC_TIMETAG_TYPETAG:
				case OSC_BUNDLE_TYPETAG:
					return s - 1;
				case '\0':
					return (s | OSC_STATE_MESSAGE_TYPETAGS_NULLPADDING) - 1;
				default:
					return s | OSC_STATE_ERROR_UNSUPPORTEDTYPETAG;
				}
			case OSC_STATE_MESSAGE_TYPETAGS_NULLPADDING:
				if(ctrm4 == 0){
					return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_DATA | (count - 1);
				}else{
					if(b == '\0'){
						return s - 1;
					}else{
						return OSC_STATE_ERROR_BADALIGNMENT;
					}
				}
			}
		case OSC_STATE_MESSAGE_DATA:
			if(count <= 0){
				return OSC_STATE_MESSAGE | OSC_STATE_MESSAGE_SIZE;
			}else{
				return s - 1;
			}
		}

	default:
		;// state is fucked...
	}
}

int main(int argc, char **argv)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_msg_u *m = osc_message_u_alloc();
	osc_message_u_setAddress(m, "/fo{}");
	osc_message_u_appendInt32(m, 20);
	osc_message_u_appendFloat(m, 44.1);
	osc_message_u_appendInt32(m, 666);
	osc_bundle_u_addMsg(b, m);

	m = osc_message_u_alloc();
	osc_message_u_setAddress(m, "/bar?");
	osc_message_u_appendFloat(m, 66.6);
	osc_bundle_u_addMsg(b, m);
	long len = 0;
	char *bundle = NULL;
	osc_bundle_u_serialize(b, &len, &bundle);
	uint64_t state = OSC_STATE_INIT;
	for(int i = 0; i < len; i++){
		printf("state: 0x%llx (count = %lld)\n", state, state & 0xffffffff);
		printf("processing byte %d (%c, 0x%x)\n", i, bundle[i], bundle[i]);
		state = process_byte(bundle[i], state);
		if(state & 0x000000ff00000000){
			printf("error:(byte %d): %s\n", i, osc_state_errstr(state));
			return 0;
		}else{
			//printf("state: 0x%llx\n", state);
		}
	}
	return 0;
}
