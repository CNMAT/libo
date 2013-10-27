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

static int osc_match_star(const char *pattern, const char *address);
static int osc_match_star_r(const char *pattern, const char *address);
static int osc_match_single_char(const char *pattern, const char *address);
static int osc_match_bracket(const char *pattern, const char *address);
static int osc_match_curly_brace(const char *pattern, const char *address);

static int osc_match_range(const char *pattern, const char *address);

#define OSC_MATCH_RANGE_NOMATCH 1

int osc_match(const char *pattern, const char *address, int *pattern_offset, int *address_offset)
{
	if(!strcmp(pattern, address)){
		*pattern_offset = strlen(pattern);
		*address_offset = strlen(address);
		return OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE;
	}

	const char *pattern_start = pattern;
	const char *address_start = address;

	*pattern_offset = 0;
	*address_offset = 0;

	struct state {uint16_t p,a;} stack[100];
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
				return OSC_MATCH_NOMATCH;
			}
		}
		switch(p){
		case '/':
			if(a == '/'){
				uint32_t pp = sp->p;
				uint32_t aa = sp->a;
				sp = stack;
				sp->p = pp + 1;
				sp->a = aa + 1;
			}else{
				return OSC_MATCH_NOMATCH;
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
				case OSC_MATCH_RANGE_NOMATCH:
					sp--;
					break;
				case 0:
					sp->a++;
					while(pattern[sp->p] != ']'){
						sp->p++;
					}
					sp->p++;
					break;
				default:
					return ret;
				}
			}
			break;
		case ']':
			return OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET;
		case '{':
			if(a == '\0' || a == '/'){
				sp--;
			}else{
				uint16_t rest = sp->p;
				while(pattern[rest] != '}'){
					rest++;
				}
				rest++;
				int cont = 0;
				sp->p++;
				uint16_t p1 = sp->p, p2 = sp->p;
				uint16_t aa = sp->a;
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
				//if(!cont){
				//sp--;
				//}
			}
			break;
		case '}':
			return OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE;
		case '*':
			{
				uint16_t pp = sp->p + 1;
				uint16_t aa = sp->a;
				sp--;
				while(address[aa] != '/' && address[aa] != '\0'){
					sp++;
					sp->p = pp;
					sp->a = aa;
					aa++;
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

static int osc_match_star(const char *pattern, const char *address)
{
	const char *address_start = address;
	const char *pattern_start = pattern;
	int num_stars = 0;
	if(*address == '\0') { return 0; }
	while(*address != '/' && *address != '\0'){
		address++;
	}
	while(*pattern != '/' && *pattern != '\0'){
		if(*pattern == '*'){
			num_stars++;
		}
		pattern++;
	}
	pattern--;
	address--;
	switch(num_stars){
	case 1:
		{
			const char *pp = pattern, *aa = address;
			while(*pp != '*'){
				if(!(osc_match_single_char(pp, aa))){
					return 0;
				}
				if(*pp == ']' || *pp == '}'){
					while(*pp != '[' && *pp != '{'){
						pp--;
					}
				}
				pp--;
				aa--;
			}
		}
		break;
	case 2:
#if (OSC_MATCH_ENABLE_2STARS == 1)
		{
			const char *pp = pattern, *aa = address;
			while(*pp != '*'){
				if(!(osc_match_single_char(pp, aa))){
					return 0;
				}
				if(*pp == ']' || *pp == '}'){
					while(*pp != '[' && *pp != '{'){
						pp--;
					}
				}
				pp--;
				aa--;
			}
			aa++; // we want to start one character forward to allow the star to match nothing
			const char *star2 = pp;
			const char *test = aa;
			int i = 0;
			while(test > address_start){
				pp = star2 - 1;
				aa = test - 1;
				i++;
				while(*pp != '*'){
					if(!osc_match_single_char(pp, aa)){
						break;
					}
					if(*pp == ']' || *pp == '}'){
						while(*pp != '[' && *pp != '{'){
							pp--;
						}
					}
					pp--;
					aa--;
				}
				if(pp == pattern_start){
					return 1;
				}
				test--;
			}
			return 0;
		}
		break;
#else
		return 0;
#endif
	default:
#if (OSC_MATCH_ENABLE_NSTARS == 1)
		return osc_match_star_r(pattern_start, address_start);
		break;
#else
		return 0;
#endif
	}
	return 1;
}

#if (OSC_MATCH_ENABLE_NSTARS == 1)
static int osc_match_star_r(const char *pattern, const char *address)
{
	if(*address == '/' || *address == '\0'){
		if(*pattern == '/' || *pattern == '\0' || (*pattern == '*' && ((*(pattern + 1) == '/') || *(pattern + 1) == '\0'))){
			return 1;
		}else{
			return 0;
		}
	}
	if(*pattern == '*'){
		if(osc_match_star_r(pattern + 1, address)){
			return 1;
		}else{
			return osc_match_star_r(pattern, address + 1);
		}
	}else{
		if(!osc_match_single_char(pattern, address)){
			return 0;
		}
		if(*pattern == '[' || *pattern == '{'){
			while(*pattern != ']' && *pattern != '}'){
				pattern++;
			}
		}
		return osc_match_star_r(pattern + 1, address + 1);
	}
}
#endif

static int osc_match_range(const char *pattern, const char *address)
{
	const char *p = pattern;
	// the first char in pattern should be the first char after the '['
	//p++;
	int val = 0;
	if(*p == '!'){
		p++;
		val = 1;
	}
	int matched = !val;
	while(*p != ']'){
		char c = *p++, c2;
		if(c == '\0' || c == '/'){
			return OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET;
		}
		if(*p == '-' && p[1] != '\0' && p[1] != '/' && p[1] != ']'){
			p++;
			c2 = *p++;
			if(c <= *address && *address <= c2){
				matched = val;
				break;
			}
		}
		if(c == *address){
			matched = val;
			break;
		}
	}
	if(matched == val){
		/*
		while(*p != ']'){
			p++;
		}
		*pattern = ++p;
		*/
	}
	return matched;
}

static int osc_match_curly_brace(const char *pattern, const char *address)
{
	pattern++;
	const char *ptr = pattern;
	while(*ptr != '}' && *ptr != '\0' && *ptr != '/'){
		while(*ptr != '}' && *ptr != '\0' && *ptr != '/' && *ptr != ','){
			ptr++;
		}
		int n = ptr - pattern;
		if(!strncmp(pattern, address, n)){
			return n;
		}else{
			ptr++;
			pattern = ptr;
		}
	}
	return 0;
}
