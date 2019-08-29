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

#include "osc_error.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "osc.h"
#include "osc_mem.h"
#include "osc_bundle_s.h"
#include "osc_byteorder.h"

int osc_error_defaultHandler(void *context, t_osc_err errorcode, const char * const errorstr)
{
	return fprintf(stderr, "%s\n", errorstr);
}

static t_osc_error_handler _osc_error_handler = osc_error_defaultHandler;

int osc_error_handler(void *context,
					const char * const filename,
		      const char * const functionname,
		      int linenum,
		      t_osc_err errorcode,
		      const char * const moreinfo_fmt,
		      ...)
{

	if(_osc_error_handler){

		// printf("%s context %p \n\tfilename %s \n\tfunctionname %s \n\tlinenum %d \n\terrorcode %d \n\tmoreinfo_fmt %s \n", __func__, context, filename, functionname, linenum, (int)errorcode, moreinfo_fmt );
        
		int buflen = MAX_ERR_STRING_LEN;
		char buf[buflen];
		char *pos = buf;
		if(filename){
			pos += snprintf(pos, buflen, "%s:\n", filename);
		}
		if(functionname){
			pos += snprintf(pos, (buflen - (pos - buf)), "%s(): ", functionname);
		}
		if(linenum > 0){
			pos += snprintf(pos, (buflen - (pos - buf)), "%d: ", linenum);
		}
		if(errorcode){
			pos += snprintf(pos, (buflen - (pos - buf)), "%s: \n", osc_error_string(errorcode));
		}
		if(moreinfo_fmt){
			pos += snprintf(pos, (buflen - (pos - buf)), "%s", moreinfo_fmt);
		}
		va_list ap;
		va_start(ap, moreinfo_fmt);
		char newbuf[buflen];
		pos = newbuf;
		pos += vsnprintf(newbuf, (buflen - (pos - newbuf)), buf, ap);
		va_end(ap);
		return _osc_error_handler(context, errorcode, newbuf);
	}
	return 0;
}

void osc_error_setHandler(t_osc_error_handler eh)
{
	_osc_error_handler = eh;
}

char *osc_error_type(t_osc_err err)
{
	switch(err){
	case OSC_ERR_NONE:
		return "/none";
	case OSC_ERR_BUNDLETOOSMALL:
	case OSC_ERR_NOBUNDLEID:
	case OSC_ERR_MSGTOOSMALL:
	// case OSC_ERR_MSGTOOLARGE:
	// 	return "OSC message size is incorrect; message extends beyond the end of the bundle";
	case OSC_ERR_MALFORMEDADDRESS:
	case OSC_ERR_NOBUNDLE:
		return "/bundle";
	case OSC_ERR_OUTOFMEM:
	case OSC_ERR_NULLPTR:
		return "/memory";
	// case OSC_ERR_BADTYPETAG:
	// 	return "encountered a discrepancy between a typetag and a piece of data";
	// case OSC_ERR_MALFORMEDMSG:
	// 	return "malformed message";
	case OSC_ERR_INVAL:
		return "/address";
	case OSC_ERR_EXPR_FUNCTIONNOTFOUND:
	case OSC_ERR_PARSER_FUNCTIONNOTFOUND:
		return "/expr/function/unbound";
	case OSC_ERR_EXPR_ADDRESSUNBOUND:
		return "/expr/address/unbound";
	case OSC_ERR_EXPPARSE:
		return "/expr/parser";
	case OSC_ERR_PARSER:
		return "/parser";
	case OSC_ERR_EXPR_ARGCHK:
		return "/expr/function/arguments";
	case OSC_ERR_EXPR_EVAL:
		return "/expr/eval";
	case OSC_ERR_INVALIDCHARINADDRESS:
		return "/address";
	default:
		return "/";
	}
}

char *osc_error_string(t_osc_err err)
{
	switch(err){
	case OSC_ERR_NONE:
		return "no error";
	case OSC_ERR_BUNDLETOOSMALL:
#define xstr(s) str(s)
#define str(s) #s
		return "OSC bundle was too small (< "xstr(OSC_HEADER_SIZE)" bytes)";
#undef xstr
#undef str
	case OSC_ERR_NOBUNDLEID:
		return "no bundle id ("OSC_IDENTIFIER") found at beginning of bundle";
	case OSC_ERR_MSGTOOSMALL:
	// 	return "OSC message was too small (< 4 bytes)";
	// case OSC_ERR_MSGTOOLARGE:
	// 	return "OSC message size is incorrect; message extends beyond the end of the bundle";
	case OSC_ERR_MALFORMEDADDRESS:
		return "malformed OSC address (probably missing an initial '/')";
	case OSC_ERR_NOBUNDLE:
		return "received a NULL pointer where a bundle was expected";
	case OSC_ERR_OUTOFMEM:
		return "out of memory";
	case OSC_ERR_NULLPTR:
		return "got a NULL pointer unexpectedly";
	// case OSC_ERR_BADTYPETAG:
	// 	return "encountered a discrepancy between a typetag and a piece of data";
	// case OSC_ERR_MALFORMEDMSG:
	// 	return "malformed message";
	case OSC_ERR_INVAL:
		return "invalid address";
	case OSC_ERR_EXPR_FUNCTIONNOTFOUND:
	case OSC_ERR_PARSER_FUNCTIONNOTFOUND:
		return "function not found";
	case OSC_ERR_EXPR_ADDRESSUNBOUND:
		return "address is unbound";
	case OSC_ERR_EXPPARSE:
		return "error parsing expression";
	case OSC_ERR_PARSER:
		return "error parsing OSC bundle";
	case OSC_ERR_EXPR_ARGCHK:
		return "invalid arguments for function";
	case OSC_ERR_EXPR_EVAL:
		return "error evaluating expression";
	case OSC_ERR_INVALIDCHARINADDRESS:
		return "invalid character found in address";
	default:
		return "unrecognized error code";
	}
}

t_osc_err osc_error_bundleSanityCheck(int len, char *bundle)
{
	if(!bundle){
		return OSC_ERR_NOBUNDLE;
	}
	if(len < OSC_HEADER_SIZE){
		return OSC_ERR_BUNDLETOOSMALL;
	}
	if(osc_bundle_s_strcmpID(bundle)){
		return OSC_ERR_NOBUNDLEID;
	}
	char *ptr = bundle + OSC_HEADER_SIZE;
	int cumsize = OSC_HEADER_SIZE;
	while((ptr - bundle) < len){
		cumsize += 4; // msg size
		int size = ntoh32(*((uint32_t *)ptr));
		cumsize += size;
		if(cumsize > len){
			return OSC_ERR_MSGTOOLARGE;
		}
		int ret;
		if((ret = osc_error_msgSanityCheck(ptr))){
			return ret;
		}
		ptr += size + 4;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_error_msgSanityCheck(char *msg)
{
	int size = ntoh32(*((uint32_t *)msg));
	if(size < 4){
		return OSC_ERR_MSGTOOSMALL;
	}
	if(*(msg + 4) != '/'){
		return OSC_ERR_MALFORMEDADDRESS;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_error_validateAddress(char *address){
	// this should be done with a proper grammar
	if(!address){
		return OSC_ERR_NULLPTR;
	}
	if(*address != '/'){
		return OSC_ERR_MALFORMEDADDRESS;
	}
	if(strlen(address) < 2){
		return OSC_ERR_MALFORMEDADDRESS;
	}
	int len = strlen(address);
	for(int i = 0; i < len; i++){
		switch(address[i]){
		case '\0':
			return OSC_ERR_NONE;
		case '#':
		case ' ':
			return OSC_ERR_INVALIDCHARINADDRESS;
		default:
			;
		}
	}
	return OSC_ERR_NONE;
}
