/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009, The Regents of
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
#include "osc_match.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

static int osc_match_range(const char *pattern, const char *address);

#define OSC_MATCH_RANGE_NOMATCH 1

static const char const *_osc_match_errstr[] = 
	{
		"no error",
		"unmatched left square bracket",
		"unmatched right square bracket",
		"unmatched left curly brace",
		"unmatched right curly brace",
		"pattern does not begin with a slash",
		"address does not begin with a slash",
		"invalid character range",
	};

const char const *osc_match_errstr(unsigned long e)
{
	e >>= 8;
	if(e < sizeof(_osc_match_errstr) / sizeof(char *)){
		return _osc_match_errstr[e];
	}
	return NULL;
}

//#define OSC_MATCH_LOGSTATE
#ifdef OSC_MATCH_LOGSTATE
#define OSC_MATCH_PRINTSTATE(p, a, po, ao) osc_match_printState(p, a, po, ao);
static void osc_match_printState(const char *pattern, const char *address, int po, int ao)
{
	for(int i = 0; i < strlen(pattern); i++){
		if(i == po){
			printf("v ");
		}else{
			printf("  ");
		}
	}
	printf("\n");
	for(int i = 0; i < strlen(pattern); i++){
		printf("%c ", pattern[i]);
	}
	printf("\n");
	for(int i = 0; i < strlen(address); i++){
		printf("%c ", address[i]);
	}
	printf("\n");
	for(int i = 0; i < strlen(address); i++){
		if(i == ao){
			printf("^ ");
		}else{
			printf("  ");
		}
	}
	printf("\n");
}
#else
#define OSC_MATCH_PRINTSTATE(p, a, po, ao) ;
#endif

int osc_match(const char *pattern, const char *address, int *pattern_offset, int *address_offset)
{
	if(*pattern != '/'){
		return OSC_MATCH_ERROR_PATTERN_NO_LEADING_SLASH;
	}
	if(*address != '/'){
		return OSC_MATCH_ERROR_ADDRESS_NO_LEADING_SLASH;
	}
	/*
	if(!strcmp(pattern, address)){
		*pattern_offset = strlen(pattern);
		*address_offset = strlen(address);
		return OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE;
	}
	*/
	const char *pattern_start = pattern;
	const char *address_start = address;

	*pattern_offset = 0;
	*address_offset = 0;

	struct state {int p,a;} stack[100];
	struct state *sp = stack;
	sp->p = sp->a = 0;

	while(1){
		if(sp < stack){
			return OSC_MATCH_NOMATCH;
		}

		char p = pattern[sp->p];
		char a = address[sp->a];

		*pattern_offset = sp->p;
		*address_offset = sp->a;

		if(a == '\0'){
			if(p == '\0'){
				return OSC_MATCH_PATTERN_COMPLETE | OSC_MATCH_ADDRESS_COMPLETE;
			}else if(p == '/'){
				return OSC_MATCH_ADDRESS_COMPLETE;
			}else{
				sp--;
				continue;
			}
		}
		OSC_MATCH_PRINTSTATE(pattern, address, sp->p, sp->a);

		switch(p){
		case '/':
			if(a == '/'){
				int pp = sp->p;
				int aa = sp->a;
				sp = stack;
				sp->p = pp + 1;
				sp->a = aa + 1;
			}else{
				sp--;
			}
			break;
		case '\0':
			if(a == '\0'){
				return OSC_MATCH_PATTERN_COMPLETE | OSC_MATCH_ADDRESS_COMPLETE;
			}else if(a == '/'){
				return OSC_MATCH_PATTERN_COMPLETE;
			}else{
				sp--;
			}
			break;
		case '?':
			if(a == '\0' || a == '/'){
				sp--;
			}else{
				sp->p++;
				sp->a++;
			}
			break;
		case '[':
			if(a == '\0' || a == '/'){
				sp--;
			}else{
				int ret;
				switch((ret = osc_match_range(pattern + sp->p, address + sp->a))){
				case 0:
					sp--;
					break;
				case 1:
					sp->a++;
					// if the first char inside the opening square bracket is 
					// a closing square bracket, it's treated as a normal char,
					// so skip over it
					if(pattern[sp->p + 1] == ']'){
						sp->p += 2;
					}
					while(1){
						if(pattern[sp->p] == ']'){
							break;
						}else if(pattern[sp->p] == '/' || pattern[sp->p] == '\0'){
							return OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET;
						}
						sp->p++;
					}
					sp->p++;
					break;
				default:
					return ret;
				}
			}
			break;
		case '{':
			if(a == '\0' || a == '/'){
				sp--;
			}else{
				int rest = sp->p;
				while(pattern[rest] != '}'){
					if(pattern[rest] == '/' || pattern[rest] == '\0'){
						return OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE;
					}
					rest++;
				}
				rest++;
				int cont = 0;
				sp->p++;
				int p1 = sp->p, p2 = sp->p;
				int aa = sp->a;
				sp--;
				while(pattern[p2] != '/' && pattern[p2] != '\0'){
					if(pattern[p2] == '/'){
						return OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE;
					}else if(pattern[p2] == ',' || pattern[p2] == '}'){
						if(!strncmp(pattern + p1, address + aa, p2 - p1)){
							cont = 1;
							sp++;
							sp->p = rest;
							sp->a = aa + (p2 - p1);
						}
						p1 = p2 + 1;
					}
					if(pattern[p2] == '}'){
						break;
					}
					p2++;
				}
			}
			break;
		case '}':
			return OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE;
		case ']':
			return OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET;
		case '*':
			{
				while(pattern[sp->p + 1] == '*'){
					// avoid all kinds of extra backtracking with multiple stars
					sp->p++;
				}
				if(pattern[sp->p + 1] == '\0' || pattern[sp->p + 1] == '/'){
					sp->p++;
					while(address[sp->a] != '/' && address[sp->a] != '\0'){
						sp->a++;
					}
				}else{
					int pp = sp->p + 1;
					int aa = sp->a;
					sp--;
					while(address[aa] != '/' && address[aa] != '\0'){
						sp++;
						sp->p = pp;
						sp->a = aa;
						aa++;
					}
				}
			}
			break;
		default:
			if(p == a){
				sp->p++;
				sp->a++;
			}else{
				sp--;
			}
			break;
		}
	}
	// unreachable
}

static inline int osc_match_range(const char *pattern, const char *address)
{
	const char *p = pattern;
	p++;
	int val = 1;
	if(*p == '!'){
		p++;
		val = 0;
	}
	int matched = !val;

	// we're on the first character inside the square brackets
	// if it's a - or a ], it gets treated as a normal character
	if(*p == ']'){
		if(*p == *address){
			return val;
		}
	}
	while(*p != ']'){
		char c = *p++, c2;
		if(c == '\0' || c == '/'){
			return OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET;
		}
		if(*p == '-'){
			if(p[1] != '\0' && p[1] != '/' && p[1] != ']'){
				p++;
				c2 = *p++;
				if(c2 <= c){
					return OSC_MATCH_ERROR_INVALID_CHARACTER_RANGE;
				}
				if(c <= *address && *address <= c2){
					matched = val;
					break;
				}
			}
		}else if(c == *address){
			matched = val;
			break;
		}
	}
	return matched;
}
