/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009-12, The Regents of
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

#include "osc_typetag.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"

static char *osc_typetag_strings[] = {"unknown type: (decimal: 0)", "unknown type: (decimal: 1)", "unknown type: (decimal: 2)", "unknown type: (decimal: 3)", "unknown type: (decimal: 4)", "unknown type: (decimal: 5)", "unknown type: (decimal: 6)", "unknown type: (decimal: 7)", "unknown type: (decimal: 8)", "unknown type: (decimal: 9)", "unknown type: (decimal: 10)", "unknown type: (decimal: 11)", "unknown type: (decimal: 12)", "unknown type: (decimal: 13)", "unknown type: (decimal: 14)", "unknown type: (decimal: 15)", "unknown type: (decimal: 16)", "unknown type: (decimal: 17)", "unknown type: (decimal: 18)", "unknown type: (decimal: 19)", "unknown type: (decimal: 20)", "unknown type: (decimal: 21)", "unknown type: (decimal: 22)", "unknown type: (decimal: 23)", "unknown type: (decimal: 24)", "unknown type: (decimal: 25)", "unknown type: (decimal: 26)", "unknown type: (decimal: 27)", "unknown type: (decimal: 28)", "unknown type: (decimal: 29)", "unknown type: (decimal: 30)", "unknown type: (decimal: 31)", "unknown type: (decimal: 32)", "unknown type: '!'", "unknown type: '\"'", "unknown type: '#'", "unknown type: '$'", "unknown type: '\%'", "unknown type: '&'", "unknown type: '''", "unknown type: '('", "unknown type: ')'", "unknown type: '*'", "unknown type: '+'", "unknown type: ','", "unknown type: '-'", "bundle", "unknown type: '/'", "unknown type: '0'", "unknown type: '1'", "unknown type: '2'", "unknown type: '3'", "unknown type: '4'", "unknown type: '5'", "unknown type: '6'", "unknown type: '7'", "unknown type: '8'", "unknown type: '9'", "unknown type: ':'", "unknown type: ';'", "unknown type: '<'", "unknown type: '='", "unknown type: '>'", "unknown type: '?'", "unknown type: '@'", "unknown type: 'A'", "unknown type: 'B'", "unsigned int8", "unknown type: 'D'", "unknown type: 'E'", "false", "unknown type: 'G'", "unsigned int64", "unsigned int32", "unknown type: 'J'", "unknown type: 'K'", "unknown type: 'L'", "unknown type: 'M'", "null", "unknown type: 'O'", "unknown type: 'P'", "unknown type: 'Q'", "unknown type: 'R'", "unknown type: 'S'", "true", "unsigned int16", "unknown type: 'V'", "unknown type: 'W'", "unknown type: 'X'", "unknown type: 'Y'", "unknown type: 'Z'", "unknown type: '['", "unknown type: '\'", "unknown type: ']'", "unknown type: '^'", "unknown type: '_'", "unknown type: '`'", "unknown type: 'a'", "unknown type: 'b'", "int8", "float64", "unknown type: 'e'", "float32", "unknown type: 'g'", "int64", "int32", "unknown type: 'j'", "unknown type: 'k'", "unknown type: 'l'", "unknown type: 'm'", "unknown type: 'n'", "unknown type: 'o'", "unknown type: 'p'", "unknown type: 'q'", "unknown type: 'r'", "string", "unknown type: 't'", "int16", "unknown type: 'v'", "unknown type: 'w'", "unknown type: 'x'", "unknown type: 'y'", "unknown type: 'z'", "unknown type: '{'", "unknown type: '|'", "unknown type: '}'", "unknown type: '~'", "unknown type: (decimal: 127)"};

static char osc_typetag_typetags[] = {'N', 'T', 'F', 'c', 'C', 'u', 'U', 'i', 'I', 'h', 'H', 'f', 'd', 's'};

static char osc_typetag_indexes[] __attribute__((unused)) = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 0, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 12, 0, 11, 0, 9, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


char *osc_typetag_str(int8_t tt)
{
	return osc_typetag_strings[tt];
}

int osc_typetag_compare(char tt1, char tt2)
{
	if(tt1 == tt2){
		return 0;
	}else if(osc_typetag_indexes[(int)tt1] < osc_typetag_indexes[(int)tt2]){
		return -1;
	}else{
		return 1;
	}
}

char osc_typetag_getLargestType(int argc, t_osc_atom_array_u **argv)
{
	uint32_t largest_type = 0;

	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			switch(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[i], j))){
			case 'N':
				if(OSC_TYPES_NULL > largest_type){
					largest_type = OSC_TYPES_NULL;
				}
				break;
			case 'T':
			case 'F':
				if(OSC_TYPES_TRUE > largest_type){
					largest_type = OSC_TYPES_TRUE;
				}
				break;
			case 'c':
				if(OSC_TYPES_INT8 > largest_type){
					largest_type = OSC_TYPES_INT8;
				}
				break;
			case 'C':
				if(OSC_TYPES_UINT8 > largest_type){
					largest_type = OSC_TYPES_UINT8;
				}
				break;
			case 'u':
				if(OSC_TYPES_INT16 > largest_type){
					largest_type = OSC_TYPES_INT16;
				}
				break;
			case 'U':
				if(OSC_TYPES_UINT16 > largest_type){
					largest_type = OSC_TYPES_UINT16;
				}
				break;
			default:
			case 'i':
				if(OSC_TYPES_INT32 > largest_type){
					largest_type = OSC_TYPES_INT32;
				}
				break;
			case 'I':
				if(OSC_TYPES_UINT32 > largest_type){
					largest_type = OSC_TYPES_UINT32;
				}
				break;
			case 'h':
				if(OSC_TYPES_INT64 > largest_type){
					largest_type = OSC_TYPES_INT64;
				}
				break;
			case 'H':
				if(OSC_TYPES_UINT64 > largest_type){
					largest_type = OSC_TYPES_UINT64;
				}
				break;
			case 'f':
				if(OSC_TYPES_FLOAT32 > largest_type){
					largest_type = OSC_TYPES_FLOAT32;
				}
				break;
			case 'd':
				if(OSC_TYPES_FLOAT64 > largest_type){
					largest_type = OSC_TYPES_FLOAT64;
				}
				break;
			case 's':
				if(OSC_TYPES_STRING > largest_type){
					largest_type = OSC_TYPES_STRING;
				}
				break;
			}
		}
	}
	return osc_typetag_typetags[largest_type];
}

