#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_serial.h"
#include "osc_byteorder.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"

static char *_osc_serial_errstr[] = {
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

char *osc_serial_errstr(uint64_t err)
{
	err = (err & 0x000000ff00000000) >> 32;
	if(err < sizeof(_osc_serial_errstr) / sizeof(char *)){
		return _osc_serial_errstr[err];
	}
	return NULL;
}

int osc_serial_errorp(uint64_t state)
{
	return (state & OSC_SERIAL_ERROR_MASK) != 0;
}

uint64_t osc_serial_processByte(char b, uint64_t s)
{
	uint64_t macro = s & 0xff00000000000000;
	uint64_t state = s & 0x00ff000000000000;
	uint64_t micro = s & 0x0000ff0000000000;
	//uint64_t error = s & 0x000000ff00000000;
	uint32_t count = s & 0x00000000ffffffff;
	uint32_t ctrm4 = count % 4;

	switch(macro){
	case OSC_SERIAL_INIT:
		switch(b){
		case '#':
			return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID;
		case '/':
			return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS:
		default: 
			return OSC_SERIAL_ERROR_BADHEADER;
		}
	case OSC_SERIAL_BUNDLE_HEADER:
		switch(state){
		case OSC_SERIAL_BUNDLE_HEADER_ID:
			switch(count){
			case 0:
				if(b == 'b'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 1:
				if(b == 'u'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 2:
				if(b == 'n'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 3:
				if(b == 'd'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 4:
				if(b == 'l'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 5:
				if(b == 'e'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 6:
				if(b == '\0'){
					return s + 1;
				}else{
					return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER;
				}
			case 7:
				return OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_TIMETAG;
			}
		case OSC_SERIAL_BUNDLE_HEADER_TIMETAG:
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
				return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b) << 24);
			}
		}
	case OSC_SERIAL_MESSAGE:
		switch(state){
		case OSC_SERIAL_MESSAGE_SIZE:
			{
				int32_t c = count & 0xff;
				switch(c){
				case 0:
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b) << 16) | (c + 1);
				case 1:
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b) << 8) | (c + 1);
				case 2:
					// negative size check
					//count = ntoh32(count | b);
					count = count | b;
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (count - 2);
				default:
					//count = ntoh32(count);
					if(b == '/'){
						return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1);
					}else{
						return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_ERROR_NOLEADINGSLASH;
					}
				}
			}
		case OSC_SERIAL_MESSAGE_ADDRESS:
			switch(micro){
			case 0:
				switch(b){
				case '[':
					return (s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS) - 1;
				case '{':
					return (s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS) - 1;
				case '\0':
					return (s | OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING) - 1;
				default:
					return s - 1;
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA:
				switch(b){
				case ',':
					return s | OSC_SERIAL_ERROR_DOUBLECOMMA;
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS:
				switch(b){
				case '[':
				case ']':
				case '{':
					return s | OSC_SERIAL_ERROR_NESTEDBRACKETS;
				case '*':
				case '?':
					return s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS;
				case ',':
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA | (count - 1);
				case '}':
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1);
				default:
					return s - 1;
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH:
				switch(b){
				case '-':
					return s | OSC_SERIAL_ERROR_DOUBLEDASH;
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS:
				switch(b){
				case '{':
				case '}':
				case '[':
					return s | OSC_SERIAL_ERROR_NESTEDBRACKETS;
				case '*':
				case '?':
					return s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS;
				case '-':
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH | (count - 1);
				case ']':
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1);
				default:
					return s - 1;
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING:
				if(ctrm4 == 0){
					if(b == ','){
						return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_TYPETAGS | (count - 1);
					}else if(b == '\0'){
						return s | OSC_SERIAL_ERROR_BADALIGNMENT;
					}else{
						return s | OSC_SERIAL_ERROR_NOTYPETAGS;
					}
				}else{
					if(b == '\0'){
						return s - 1;
					}else{
						return s | OSC_SERIAL_ERROR_BADALIGNMENT;
					}
				}
			}
		case OSC_SERIAL_MESSAGE_TYPETAGS:
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
					return (s | OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING) - 1;
				default:
					return s | OSC_SERIAL_ERROR_UNSUPPORTEDTYPETAG;
				}
			case OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING:
				if(ctrm4 == 0){
					return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_DATA | (count - 1);
				}else{
					if(b == '\0'){
						return s - 1;
					}else{
						return OSC_SERIAL_ERROR_BADALIGNMENT;
					}
				}
			}
		case OSC_SERIAL_MESSAGE_DATA:
			if(count <= 0){
				return OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_SIZE;
			}else{
				return s - 1;
			}
		}

	default:
		;// state is fucked...
	}
}

/*
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
	uint64_t state = OSC_SERIAL_INIT;
	for(int i = 0; i < len; i++){
		printf("state: 0x%llx (count = %lld)\n", state, state & 0xffffffff);
		printf("processing byte %d (%c, 0x%x)\n", i, bundle[i], bundle[i]);
		state = process_byte(bundle[i], state);
		if(state & 0x000000ff00000000){
			printf("error:(byte %d): %s\n", i, osc_serial_errstr(state));
			return 0;
		}else{
			//printf("state: 0x%llx\n", state);
		}
	}
	return 0;
}
*/
