/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2014, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_serial.h"
#include "osc_byteorder.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"

//#define OSC_SERIAL_DEBUG
#ifdef OSC_SERIAL_DEBUG
#define OSC_SERIAL_RETURN(s) {						\
		char buf[256];						\
		if(osc_serial_errorp(s)){				\
			printf("%d: ERROR: %s\n", __LINE__, osc_serial_errstr(s)); \
		}else{							\
			osc_serial_statestr(s, sizeof(buf), buf);	\
			printf("%d: %s\n", __LINE__, buf);		\
		}							\
		return s;						\
	}
#else
#define OSC_SERIAL_RETURN(s) return s;
#endif

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
	"two commas in a row within curly brackets",
	"extraneous closing curly brace",
	"unmatched open curly brace",
	"extraneous closing square bracket",
	"unmatched open square bracket"
	};

static char *_osc_serial_macroStr[] = {
	"bundle header",
	"bundle message",
	"naked message"
};

static char *_osc_serial_headerStateStr[] = {
	"id",
	"timetag"
};

static char *_osc_serial_messageStateStr[] = {
	"size",
	"address",
	"typetags",
	"data"
};

static char *_osc_serial_addressMicroStr[] = {
	"inside curly braces",
	"inside curly braces: comma",
	"inside square brackets",
	"inside square brackets: dash",
	"null padding"
};

static char *_osc_serial_typetagMicroStr[] = {
	"null padding"
};

#define OSC_SERIAL_NULLSTATE ""

char *osc_serial_macroStr(uint64_t s)
{
	uint32_t state = (uint32_t)log2f((float)((s & 0xff00000000000000) >> 56));
	if(state <= sizeof(_osc_serial_macroStr) / sizeof(char *)){
		return _osc_serial_macroStr[state];
	}else{
		return OSC_SERIAL_NULLSTATE;
	}
}

char *osc_serial_stateStr(uint64_t s)
{
	uint64_t macro = (uint64_t)(s & 0xff00000000000000);
	uint32_t state = (uint32_t)log2f((float)((s & 0x00ff000000000000) >> 48));
	switch(macro){
	case OSC_SERIAL_BUNDLE_HEADER:
		if(state < sizeof(_osc_serial_headerStateStr) / sizeof(char *)){
			return _osc_serial_headerStateStr[state];
		}else{
			return OSC_SERIAL_NULLSTATE;
		}
		break;
	case OSC_SERIAL_BUNDLE_MESSAGE:
	case OSC_SERIAL_MESSAGE:
		if(state < sizeof(_osc_serial_messageStateStr) / sizeof(char *)){
			return _osc_serial_messageStateStr[state];
		}else{
			return OSC_SERIAL_NULLSTATE;
		}
		break;
	default:
		return OSC_SERIAL_NULLSTATE;
	}
}

char *osc_serial_microStr(uint64_t s)
{
	uint64_t macro = (uint64_t)(s & 0xff00000000000000);
	uint64_t state = (uint64_t)(s & 0x00ff000000000000);
	uint32_t micro = (uint32_t)log2f((float)((s & 0x00ff000000000000) >> 40));
	switch(macro){
	case OSC_SERIAL_BUNDLE_MESSAGE:
	case OSC_SERIAL_MESSAGE:
		switch(state){
		case OSC_SERIAL_MESSAGE_ADDRESS:
			if(micro < sizeof(_osc_serial_addressMicroStr) / sizeof(char *)){
				return _osc_serial_addressMicroStr[micro];
			}else{
				return OSC_SERIAL_NULLSTATE;
			}
			break;
		case OSC_SERIAL_MESSAGE_TYPETAGS:
			if(micro < sizeof(_osc_serial_typetagMicroStr) / sizeof(char *)){
				return _osc_serial_typetagMicroStr[micro];
			}else{
				return OSC_SERIAL_NULLSTATE;
			}
			break;
		default:
			return OSC_SERIAL_NULLSTATE;
		}
		break;
	default:
		return OSC_SERIAL_NULLSTATE;
	}
}

uint32_t osc_serial_counter(uint64_t s)
{
	return (uint32_t)(s & 0x00000000ffffffff);
}

void osc_serial_statestr(uint64_t s, int slen, char *string)
{
	snprintf(string, slen, "%20s : %20s : %20s : %u", osc_serial_macroStr(s), osc_serial_stateStr(s), osc_serial_microStr(s), osc_serial_counter(s));
}

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
			OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID);
		case '/':
			OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS);
		default: 
			OSC_SERIAL_RETURN(OSC_SERIAL_ERROR_BADHEADER);
		}
	case OSC_SERIAL_BUNDLE_HEADER:
		switch(state){
		case OSC_SERIAL_BUNDLE_HEADER_ID:
			switch(count){
			case 0:
				if(b == 'b'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 1:
				if(b == 'u'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 2:
				if(b == 'n'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 3:
				if(b == 'd'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 4:
				if(b == 'l'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 5:
				if(b == 'e'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 6:
				if(b == '\0'){
					OSC_SERIAL_RETURN(s + 1);
				}else{
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_ID | OSC_SERIAL_ERROR_BADHEADER);
				}
			case 7:
				OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_HEADER | OSC_SERIAL_BUNDLE_HEADER_TIMETAG);
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
				OSC_SERIAL_RETURN(s + 1);
			case 7:
				// do negative size check
				OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b & 0xff) << 24));
			}
		}
	case OSC_SERIAL_BUNDLE_MESSAGE:
		switch(state){
		case OSC_SERIAL_MESSAGE_SIZE:
			{
				int32_t c = count & 0xff;
				switch(c){
				case 0:
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b & 0xff) << 16) | (c + 1));
				case 1:
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (((int32_t)b & 0xff) << 8) | (c + 1));
				case 2:
					// negative size check
					//count = ntoh32(count | b);
					count = count | b;
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_SIZE | (count - 2));
				default:
					//count = ntoh32(count);
					if(b == '/'){
						OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1));
					}else{
						OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_ERROR_NOLEADINGSLASH);
					}
				}
			}
		case OSC_SERIAL_MESSAGE_ADDRESS:
			switch(micro){
			case 0:
				switch(b){
				case '[':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS) - 1);
				case '{':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS) - 1);
				case '\0':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING) - 1);
				case ']':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_UNMATCHEDRIGHTSQUAREBRACKET);
				case '}':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_UNMATCHEDRIGHTCURLYBRACE);
				default:
					OSC_SERIAL_RETURN(s - 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA:
				switch(b){
				case ',':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_DOUBLECOMMA);
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS:
				switch(b){
				case '[':
				case ']':
				case '{':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NESTEDBRACKETS);
				case '*':
				case '?':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS);
				case ',':
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA | (count - 1));
				case '}':
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1));
				case '\0':
					OSC_SERIAL_RETURN(OSC_SERIAL_ERROR_UNMATCHEDLEFTCURLYBRACE);
				default:
					OSC_SERIAL_RETURN(s - 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH:
				switch(b){
				case '-':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_DOUBLEDASH);
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS:
				switch(b){
				case '{':
				case '}':
				case '[':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NESTEDBRACKETS);
				case '*':
				case '?':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS);
				case '-':
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH | (count - 1));
				case ']':
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count - 1));
				case '\0':
					OSC_SERIAL_RETURN(OSC_SERIAL_ERROR_UNMATCHEDLEFTSQUAREBRACKET);
				default:
					OSC_SERIAL_RETURN(s - 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING:
				if(ctrm4 == 0){
					if(b == ','){
						OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_TYPETAGS | (count - 1));
					}else if(b == '\0'){
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_BADALIGNMENT);
					}else{
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NOTYPETAGS);
					}
				}else{
					if(b == '\0'){
						OSC_SERIAL_RETURN(s - 1);
					}else{
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_BADALIGNMENT);
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
					OSC_SERIAL_RETURN(s - 1);
				case '\0':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING) - 1);
				default:
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_UNSUPPORTEDTYPETAG);
				}
			case OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING:
				if(ctrm4 == 0){
					OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_DATA | (count - 1));
				}else{
					if(b == '\0'){
						OSC_SERIAL_RETURN(s - 1);
					}else{
						OSC_SERIAL_RETURN(OSC_SERIAL_ERROR_BADALIGNMENT);
					}
				}
			}
		case OSC_SERIAL_MESSAGE_DATA:
			if(count <= 0){
				OSC_SERIAL_RETURN(OSC_SERIAL_BUNDLE_MESSAGE | OSC_SERIAL_MESSAGE_SIZE);
			}else{
				OSC_SERIAL_RETURN(s - 1);
			}
		}
	case OSC_SERIAL_MESSAGE: // naked message not contained in a bundle
		switch(state){
		case OSC_SERIAL_MESSAGE_ADDRESS:
			switch(micro){
			case 0:
				switch(b){
				case '[':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS) + 1);
				case '{':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS) + 1);
				case '\0':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING) + 1);
				default:
					OSC_SERIAL_RETURN(s + 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA:
				switch(b){
				case ',':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_DOUBLECOMMA);
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS:
				switch(b){
				case '[':
				case ']':
				case '{':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NESTEDBRACKETS);
				case '*':
				case '?':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS);
				case ',':
					OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDECURLYBRACKETS_COMMA | (count + 1));
				case '}':
					OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count + 1));
				default:
					OSC_SERIAL_RETURN(s + 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH:
				switch(b){
				case '-':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_DOUBLEDASH);
				}
				// intentional fallthrough to the next case
			case OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS:
				switch(b){
				case '{':
				case '}':
				case '[':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NESTEDBRACKETS);
				case '*':
				case '?':
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_WILDCARDINSIDEBRACKETS);
				case '-':
					OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | OSC_SERIAL_MESSAGE_ADDRESS_INSIDESQUAREBRACKETS_DASH | (count + 1));
				case ']':
					OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_ADDRESS | (count + 1));
				default:
					OSC_SERIAL_RETURN(s + 1);
				}
			case OSC_SERIAL_MESSAGE_ADDRESS_NULLPADDING:
				if(ctrm4 == 3){
					if(b == ','){
						OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_TYPETAGS | (count + 1));
					}else if(b == '\0'){
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_BADALIGNMENT);
					}else{
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_NOTYPETAGS);
					}
				}else{
					if(b == '\0'){
						OSC_SERIAL_RETURN(s + 1);
					}else{
						OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_BADALIGNMENT);
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
					OSC_SERIAL_RETURN(s + 1);
				case '\0':
					OSC_SERIAL_RETURN((s | OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING) + 1);
				default:
					OSC_SERIAL_RETURN(s | OSC_SERIAL_ERROR_UNSUPPORTEDTYPETAG);
				}
			case OSC_SERIAL_MESSAGE_TYPETAGS_NULLPADDING:
				if(ctrm4 == 3){
					OSC_SERIAL_RETURN(OSC_SERIAL_MESSAGE | OSC_SERIAL_MESSAGE_DATA | (count + 1));
				}else{
					if(b == '\0'){
						OSC_SERIAL_RETURN(s + 1);
					}else{
						OSC_SERIAL_RETURN(OSC_SERIAL_ERROR_BADALIGNMENT);
					}
				}
			}
		case OSC_SERIAL_MESSAGE_DATA:
			OSC_SERIAL_RETURN(s + 1);
		}

	default:
		;// state is fucked...
	}
	OSC_SERIAL_RETURN(0);
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
			OSC_SERIAL_RETURN(0);
		}else{
			//printf("state: 0x%llx\n", state);
		}
	}
	OSC_SERIAL_RETURN(0);
}
*/
