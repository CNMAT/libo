#include "../osc_match.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>

#define __OSC_PROFILE__
#include "../osc_profile.h"

#ifndef OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET
#define OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET 0x100
#endif
#ifndef OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET
#define OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET 0x200
#endif
#ifndef OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE
#define OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE 0x300
#endif
#ifndef OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE
#define OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE 0x400
#endif
#ifndef OSC_MATCH_ERROR_PATTERN_NO_LEADING_SLASH
#define OSC_MATCH_ERROR_PATTERN_NO_LEADING_SLASH 0x500
#endif
#ifndef OSC_MATCH_ERROR_ADDRESS_NO_LEADING_SLASH
#define OSC_MATCH_ERROR_ADDRESS_NO_LEADING_SLASH 0x600
#endif
#ifndef OSC_MATCH_ERROR_INVALID_CHARACTER_RANGE
#define OSC_MATCH_ERROR_INVALID_CHARACTER_RANGE 0x700
#endif
#ifndef OSC_MATCH_ERROR_BACKTRACK_LIMIT_EXCEEDED
#define OSC_MATCH_ERROR_BACKTRACK_LIMIT_EXCEEDED 0x800
#endif

#define BULLSHIT
#ifdef BULLSHIT
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
		"backtrack limit exceeded"
	};

const char const *osc_match_errstr(unsigned long e)
{
	e >>= 8;
	if(e < sizeof(_osc_match_errstr) / sizeof(char *)){
		return _osc_match_errstr[e];
	}
	return NULL;
}
#endif

static const char *theWholePattern;	/* Just for warning messages */

#define OSCWarning(st, ...) oscwarning_cb(st, __VA_ARGS__);
static int osc_match_test_errno = 0;
typedef int Boolean;
#define TRUE 1
#define FALSE 0
Boolean PatternMatch (const char *  pattern, const char * test);
static Boolean MatchBrackets (const char *pattern, const char *test);
static Boolean MatchList (const char *pattern, const char *test);

typedef struct _test{
	char *pattern;
	char *address;
	int return_code;
	int pmatch_len;
	int amatch_len;
} t_test;

#define FULLMATCH OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE

#define E_WRONG_RET_VAL 1
#define E_P_TOO_LONG 2
#define E_P_TOO_SHORT 4
#define E_A_TOO_LONG 8
#define E_A_TOO_SHORT 16

t_test testar[] = {
	// full matches
	{"/wessel", "/wessel", FULLMATCH, 7, 7},
	{"/wesse?", "/wessel", FULLMATCH, 7, 7},
	{"/we?sel", "/wessel", FULLMATCH, 7, 7},
	{"/?essel", "/wessel", FULLMATCH, 7, 7},
	{"/?e?sel", "/wessel", FULLMATCH, 7, 7},
	{"/??????", "/wessel", FULLMATCH, 7, 7},
	{"/[wv]essel", "/wessel", FULLMATCH, 10, 7},
	{"/[a-z]essel", "/wessel", FULLMATCH, 11, 7},
	{"/we[as]sel", "/wessel", FULLMATCH, 10, 7},
	{"/[wv]e[as]sel", "/wessel", FULLMATCH, 13, 7},
	{"/[!a-uxyz]essel", "/wessel", FULLMATCH, 15, 7},
	{"/{w,v}essel", "/wessel", FULLMATCH, 11, 7},
	{"/we{a,s}sel", "/wessel", FULLMATCH, 11, 7},
	{"/{w,v}e{a,s}sel", "/wessel", FULLMATCH, 15, 7},
	{"/{wes,wea}sel", "/wessel", FULLMATCH, 13, 7},
	{"/{wessel,weasel,vessel}", "/wessel", FULLMATCH, 23, 7},
	{"/*", "/wessel", FULLMATCH, 2, 7},
	{"/w*", "/wessel", FULLMATCH, 3, 7},
	{"/*sel", "/wessel", FULLMATCH, 5, 7},
	{"/*ss*", "/wessel", FULLMATCH, 5, 7},
	{"/*ss*l", "/wessel", FULLMATCH, 6, 7},
	{"/*e?*l", "/wessel", FULLMATCH, 6, 7},
	{"/*e{a,aa,s,ss}*l", "/wessel", FULLMATCH, 16, 7},
	{"/*e{a,aa,s}*l", "/wessel", FULLMATCH, 13, 7},
	{"/w[abcde-]ssel", "/w-ssel", FULLMATCH, 14, 7},
	{"/w[-abcde]ssel", "/w-ssel", FULLMATCH, 14, 7},
	{"/w[]abcde]ssel", "/w]ssel", FULLMATCH, 14, 7}, // this is standard in many regexps, but not in PatternMatch
	{"/***l", "/wessel", FULLMATCH, 5, 7}, // tests to make sure extra *** are reduced down to 1
	{"/[a-zA-Z][a-zA-Z][a-zA-Z][a-zA-Z][a-zA-Z][a-zA-Z]", "/wessel", FULLMATCH, 49, 7},
	{"/*e*l", "/wessel", FULLMATCH, 5, 7},
	{"/*?*l", "/wessel", FULLMATCH, 5, 7},
	// full pattern, partial address
	{"/david", "/david/l/wessel", OSC_MATCH_PATTERN_COMPLETE, 6, 6},
	{"/david/?", "/david/l/wessel", OSC_MATCH_PATTERN_COMPLETE, 8, 8},
	{"/*/*", "/david/l/wessel", OSC_MATCH_PATTERN_COMPLETE, 4, 8},
	{"/*/?", "/david/l/wessel", OSC_MATCH_PATTERN_COMPLETE, 4, 8},
	// partial pattern, full address
	{"/david/l/wessel", "/david", OSC_MATCH_ADDRESS_COMPLETE, 6, 6},
	{"/*/l/wessel", "/david", OSC_MATCH_ADDRESS_COMPLETE, 2, 6},
	{"/*/l/wessel", "/david/l", OSC_MATCH_ADDRESS_COMPLETE, 4, 8},
	// failures
	{"/wesse", "/wessel", 0, -1, -1},
	{"/wess?el", "/wessel", 0, -1, -1},
	{"/???????", "/wessel", 0, -1, -1},
	{"/?????", "/wessel", 0, -1, -1},
	{"/*e*{a,aa,s,ss}l", "/wessel", 0, -1, -1}, // PatternMatch() succeeds which is a bug
	{"/*e*{a,aa,s}l", "/wessel", 0, -1, -1}, // PatternMatch() succeeds which is a bug
	{"/david/??", "/david/l/wessel", 0, -1, -1},
	{"/david/l/", "/david/l/wessel", 0, -1, -1},
<<<<<<< HEAD
	{"/david/a", "/david/l/wessel", 0, -1, -1},
	{"/david/[!a-z]", "/david/l/wessel", 0, -1, -1},
	{"/david{l,/,w}", "/david/l/wessel", 0, -1, -1},
	{"/*l/wessel", "/david/l", 0, -1, -1},
	// errors
	{"wessel", "/wessel", OSC_MATCH_ERROR_PATTERN_NO_LEADING_SLASH, -1, -1},
	{"/wessel", "wessel", OSC_MATCH_ERROR_ADDRESS_NO_LEADING_SLASH, -1, -1},
	{"/w{essel", "/wessel", OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE, -1, -1},
	{"/wesse}l", "/wessel", OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE, -1, -1},
	{"/w[essel", "/wessel", OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET, -1, -1},
	{"/wess]el", "/wessel", OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET, -1, -1}, 
	{"/w[ab--cde]ssel", "/w-ssel", OSC_MATCH_ERROR_INVALID_CHARACTER_RANGE, -1, -1}, // b > -
};

void osc_match_test_printFail(char *fmt, int i1, int i2)
{
	long len = snprintf(NULL, 0, fmt, i1, i2);
	char buf[len + 1];
	snprintf(buf, len, fmt, i1, i2);
	char buf2[len + 1];
	memset(buf2, '!', len);
	buf2[len] = '\0';
	printf("%s\n", buf2);
	printf("%s\n", buf);
	printf("%s\n", buf2);
}

int main(int argc, char **argv)
{
	int numtests = sizeof(testar) / sizeof(t_test);
	int num_osc_match_fails = 0;
	int num_osckit_match_fails = 0;
	for(int i = 0; i < numtests; i++){
		t_test t = testar[i];

		int osc_match_po = 0, osc_match_ao = 0;
		int osc_match_ret;
		const int nnn = 10000;
		OSC_PROFILE_TIMER_START(t1);
		for(int i = 0; i < nnn; i++){
			osc_match_ret = osc_match(t.pattern, t.address, &osc_match_po, &osc_match_ao);
		}
		OSC_PROFILE_TIMER_STOP(t1);
		OSC_PROFILE_TIMER_PRINTF(t1);

		char pattern_parts[128][128];
		char address_parts[128][128];
		int num_pattern_parts = 0;
		int num_address_parts = 0;
		char *pp1 = t.pattern;
		char *pp2 = t.pattern + 1;
		char *aa1 = t.address;
		char *aa2 = t.address + 1;
		int j = 0;
		while(*pp2 != '\0'){
			while(*pp2 != '/' && *pp2 != '\0'){
				pp2++;
			}
			if(pp2 != pp1){
				memset(pattern_parts[j], '\0', sizeof(pattern_parts[j]));
				strncpy(pattern_parts[j], pp1, pp2 - pp1);
			}
			pp1 = pp2;
			if(*pp2 != '\0'){
				pp2++;
			}
			j++;
		}
		num_pattern_parts = j;
		j = 0;
		while(*aa2 != '\0'){
			while(*aa2 != '/' && *aa2 != '\0'){
				aa2++;
			}
			if(aa2 != aa1){
				memset(address_parts[j], '\0', sizeof(address_parts[j]));
				strncpy(address_parts[j], aa1, aa2 - aa1);
			}
			aa1 = aa2;
			if(*aa2 != '\0'){
				aa2++;
			}
			j++;
		}
		num_address_parts = j;
		int osckit_match_ret = 1;
		int level = 0;
		int osckit_po = 0;
		int osckit_ao = 0;
		osc_match_test_errno = 0;
		OSC_PROFILE_TIMER_START(t2);
		for(int i = 0; i < nnn; i++){
			osckit_match_ret = 1;
			osckit_po = 0;
			osckit_ao = 0;
			osc_match_test_errno = 0;
			for(level = 0; level < num_pattern_parts && level < num_address_parts; level++){
				if(!(osckit_match_ret *= PatternMatch(pattern_parts[level], address_parts[level]))){
					break;
				}
				osckit_po += strlen(pattern_parts[level]);
				osckit_ao += strlen(address_parts[level]);
			}
		}
		OSC_PROFILE_TIMER_STOP(t2);
		OSC_PROFILE_TIMER_PRINTF(t2);
		osckit_match_ret |= osc_match_test_errno;
		if(osckit_match_ret & 0xff){
			osckit_match_ret = 0;
			if(osckit_po == strlen(t.pattern)){
				osckit_match_ret |= OSC_MATCH_PATTERN_COMPLETE;
			}
			if(osckit_ao == strlen(t.address)){
				osckit_match_ret |= OSC_MATCH_ADDRESS_COMPLETE;
			}
		}

		struct test_result {
			char *func_name;
			int ret;
			int po;
			int ao;
			int *num_failures;
		} test_results[] = {
			{
				"osc_match",
				osc_match_ret,
				osc_match_po,
				osc_match_ao,
				&num_osc_match_fails
			},
			{
				"PatternMatch",
				osckit_match_ret,
				osckit_po,
				osckit_ao,
				&num_osckit_match_fails
			}
		};

		for(int r = 0; r < 2; r++){
			printf("\n");
			struct test_result res = test_results[r];
			printf("testing %s(%s, %s)\n", res.func_name, t.pattern, t.address);
			if(res.ret != t.return_code){
				(*(res.num_failures))++;
				osc_match_test_printFail("!!!FAIL: expected return value of %d but got %d\n", t.return_code, res.ret);
			}else if(t.pmatch_len >= 0 && res.po != t.pmatch_len){
				(*(res.num_failures))++;
				osc_match_test_printFail("!!!FAIL: expected pattern to match %d chars, but it matched %d\n", t.pmatch_len, res.po);
			}else if(t.amatch_len >= 0 && res.ao != t.amatch_len){
				(*(res.num_failures))++;
				osc_match_test_printFail("!!!FAIL: expected address to match %d chars, but it matched %d\n", t.amatch_len, res.ao);
			}else if(t.return_code == res.ret && ((t.pmatch_len >= 0 && res.po == t.pmatch_len) || (t.pmatch_len < 0)) && ((t.amatch_len >= 0 && res.ao == t.amatch_len) || (t.amatch_len < 0))){
				switch(t.return_code){
				case 0:
					printf("PASS: %s failed to match %s as expected\n", t.pattern, t.address);
					break;
				case 1:
					{
						char buf[res.po + 1];
						snprintf(buf, res.po + 1, "%s", t.pattern);
						printf("PASS:             ");
						for(int ii = 0; ii < res.po; ii++){
							printf(" ");
						}
						printf("v\n");
						printf("pattern partial : %s\n", t.pattern);
						printf("address complete: %s\n", t.address);
					}
					break;
				case 2:
					{
						char buf[res.po + 1];
						snprintf(buf, res.po + 1, "%s", t.pattern);
						printf("PASS:\n");
						printf("pattern complete: %s\n", t.pattern);
						printf("address partial : %s\n", t.address);
						printf("                  ");
						for(int ii = 0; ii < res.ao; ii++){
							printf(" ");
						}
						printf("^\n");
					}
					break;
				case 3:
					printf("PASS: %s fully matched %s\n", t.pattern, t.address);
					break;
				default:
					printf("PASS: encountered an error (%d:\"%s\") as expected while attempting to match %s against %s\n", res.ret, osc_match_errstr(res.ret), t.pattern, t.address);

				}
			}else{
				printf("ERROR: encountered an unexpected result while attempting to match %s against %s\n", t.pattern, t.address);
				printf("expected return value %d, pattern offset %d, address offset %d\n", t.return_code, t.pmatch_len, t.amatch_len);
				printf("but got  return value %d, pattern offset %d, address offset %d\n", res.ret, res.po, res.ao);
			}

			//printf("\n");

			// print results for PatternMatch()

		}
		printf("____________________________________________________________________________________________________\n\n");
	}
	printf("osc_match() passed %d of %d tests\n", numtests - num_osc_match_fails, numtests);
	printf("PatternMatch() passed %d of %d tests\n", numtests - num_osckit_match_fails, numtests);
	return 0;
}

void oscwarning_cb(const char *st, ...)
{
	char *spurious = "Spurious ";
	char *unterminated = "Unterminated ";
	int spurious_len = strlen(spurious);
	int unterminated_len = strlen(unterminated);
	char c;
	if(!strncmp(st, spurious, spurious_len)){
		// get the char
		va_list args;
		va_start(args, st);
		c = va_arg(args, int);
	}else if(!strncmp(st, unterminated, unterminated_len)){
		c = st[unterminated_len];
	}else{
		printf("%s: unexpected warning: %s\n", __func__, st);
		return;
	}
	switch(c){
	case '[':
		osc_match_test_errno = OSC_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET;
		break;
	case ']':
		osc_match_test_errno = OSC_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET;
		break;
	case '{':
		osc_match_test_errno = OSC_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE;
		break;
	case '}':
		osc_match_test_errno = OSC_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE;
		break;
	}
}

Boolean PatternMatch (const char *  pattern, const char * test) {
	theWholePattern = pattern;
  
	if (pattern == 0 || pattern[0] == 0) {
		return test[0] == 0;
	} 
  
	if (test[0] == 0) {
		if (pattern[0] == '*')
			return PatternMatch (pattern+1,test);
		else
			return FALSE;
	}

	switch (pattern[0]) {
	case 0      : return test[0] == 0;
	case '?'    : return PatternMatch (pattern + 1, test + 1);
	case '*'    : 
		if (PatternMatch (pattern+1, test)) {
			return TRUE;
		} else {
			return PatternMatch (pattern, test+1);
		}
	case ']'    :
	case '}'    :
		OSCWarning("Spurious %c in pattern \".../%s/...\"",pattern[0], theWholePattern);
		return FALSE;
	case '['    :
		return MatchBrackets (pattern,test);
	case '{'    :
		return MatchList (pattern,test);
	case '\\'   :  
		if (pattern[1] == 0) {
			return test[0] == 0;
		} else if (pattern[1] == test[0]) {
			return PatternMatch (pattern+2,test+1);
		} else {
			return FALSE;
		}
	default     :
		if (pattern[0] == test[0]) {
			return PatternMatch (pattern+1,test+1);
		} else {
			return FALSE;
		}
	}
}


/* we know that pattern[0] == '[' and test[0] != 0 */

static Boolean MatchBrackets (const char *pattern, const char *test) {
	Boolean result;
	Boolean negated = FALSE;
	const char *p = pattern;

	if (pattern[1] == 0) {
		OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
		return FALSE;
	}

	if (pattern[1] == '!') {
		negated = TRUE;
		p++;
	}

	while (*p != ']') {
		if (*p == 0) {
			OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
			return FALSE;
		}
		if (p[1] == '-' && p[2] != 0) {
			if (test[0] >= p[0] && test[0] <= p[2]) {
				result = !negated;
				goto advance;
			}
		}
		if (p[0] == test[0]) {
			result = !negated;
			goto advance;
		}
		p++;
	}

	result = negated;

 advance:

	if (!result)
		return FALSE;

	while (*p != ']') {
		if (*p == 0) {
			OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
			return FALSE;
		}
		p++;
	}

	return PatternMatch (p+1,test+1);
}

static Boolean MatchList (const char *pattern, const char *test) {

	const char *restOfPattern, *tp = test;


	for(restOfPattern = pattern; *restOfPattern != '}'; restOfPattern++) {
		if (*restOfPattern == 0) {
			OSCWarning("Unterminated { in pattern \".../%s/...\"", theWholePattern);
			return FALSE;
		}
	}

	restOfPattern++; /* skip close curly brace */


	pattern++; /* skip open curly brace */

	while (1) {
   
		if (*pattern == ',') {
			if (PatternMatch (restOfPattern, tp)) {
				return TRUE;
			} else {
				tp = test;
				++pattern;
			}
		} else if (*pattern == '}') {
			return PatternMatch (restOfPattern, tp);
		} else if (*pattern == *tp) {
			++pattern;
			++tp;
		} else {
			tp = test;
			while (*pattern != ',' && *pattern != '}') {
				pattern++;
			}
			if (*pattern == ',') {
				pattern++;
			}
		}
	}
}



