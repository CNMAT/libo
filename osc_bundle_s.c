/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009-ll, The Regents of
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
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_mem.h"
#include "osc_match.h"
#include "osc_timetag.h"

#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"

#include "osc_message_s.h"
#include "osc_bundle_s.r"
#include "osc_bundle_s.h"
#include "osc_bundle_iterator_s.h"
#include "osc_atom_s.h"
#include "osc_util.h"
#include "osc_array.h"

#define myprintf(st, args...)
//#define myprintf(st, args...)printf(st, ##args)

t_osc_bndl_s *osc_bundle_s_alloc(long len, char *ptr)
{
	t_osc_bndl_s *b = osc_mem_alloc(sizeof(t_osc_bndl_s));
	b->len = len;
	b->ptr = ptr;
	return b;
}

t_osc_bndl_s *osc_bundle_s_allocEmptyBundle(void)
{
	t_osc_bndl_s *b = osc_mem_alloc(sizeof(t_osc_bndl_s));
	osc_bundle_s_setBundleID_b(b);
	return b;
}

t_osc_err osc_bundle_s_deepCopy(t_osc_bndl_s **dest, t_osc_bndl_s *src)
{
	if(!src){
		return OSC_ERR_NOBUNDLE;
	}
	if(!(*dest)){
		*dest = osc_mem_alloc(sizeof(t_osc_bndl_s));
	}
	(*dest)->ptr = osc_mem_alloc(src->len);
	memcpy((*dest)->ptr, src->ptr, src->len);
	(*dest)->len = src->len;
	return OSC_ERR_NONE;
}

size_t osc_bundle_s_getStructSize(void)
{
	return sizeof(t_osc_bndl_s);
}

void osc_bundle_s_free(t_osc_bndl_s *bndl)
{
	if(!bndl){
		return;
	}
	osc_mem_free(bndl);
}

void osc_bundle_s_deepFree(t_osc_bndl_s *bndl)
{
	if(bndl){
		if(bndl->ptr){
			osc_mem_free(bndl->ptr);
		}
		osc_mem_free(bndl);
	}
}

char *osc_bundle_s_getPtr(t_osc_bndl_s *bndl)
{
	if(!bndl){
		return NULL;
	}
	return bndl->ptr;
}

long osc_bundle_s_getLen(t_osc_bndl_s *bndl)
{
	if(!bndl){
		return 0;
	}
	return bndl->len;
}

void osc_bundle_s_setLen(t_osc_bndl_s *bndl, long len)
{
	if(bndl){
		bndl->len = len;
	}
}

void osc_bundle_s_setPtr(t_osc_bndl_s *bndl, char *ptr)
{
	if(bndl){
		bndl->ptr = ptr;
	}
}

t_osc_err osc_bundle_s_getMsgCount(int len, char *buf, int *count)
{
	*count = 0;
	t_osc_err ret;
	if((ret = osc_error_bundleSanityCheck(len, buf))){
		return ret;
	}
	char *ptr = buf + OSC_HEADER_SIZE;
	while((ptr - buf) < len){
		int32_t size = ntoh32(*((int32_t *)ptr));
		*count += 1;
		ptr += size + 4;
	}
	return OSC_ERR_NONE;
}

t_osc_msg_s *osc_bundle_s_getFirstMsg(t_osc_bndl_s *bndl)
{
	if(bndl){
		char *ptr = osc_bundle_s_getPtr(bndl);
		long len = osc_bundle_s_getLen(bndl);
		if(ptr && len > OSC_HEADER_SIZE){
			t_osc_msg_s *m = osc_message_s_alloc();
			osc_message_s_wrap(m, ptr + OSC_HEADER_SIZE);
			return m;
		}
	}
	return NULL;
}

t_osc_err osc_bundle_s_getMessagesWithCallback(int len, char *buf, void (*f)(t_osc_msg_s*, void *), void *context)
{
	int ret;
	if((ret = osc_error_bundleSanityCheck(len, buf))){
		return ret;
	}
	t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, buf);
	while(osc_bndl_it_s_hasNext(it)){
		t_osc_msg_s *m = osc_bndl_it_s_next(it);
		f(m, context);
	}
	osc_bndl_it_s_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_addressIsBound(long len, char *buf, char *address, int fullmatch, int *res)
{
	*res = 0;
	//t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, buf);
	//while(osc_bndl_it_s_hasNext(it)){
	char *msg = buf + OSC_HEADER_SIZE;
	while(msg - buf < len){
		//t_osc_msg_s *m = osc_bndl_it_s_next(it);
		int po, ao;
		//int r = osc_match(address, osc_message_s_getAddress(m), &po, &ao);
		int32_t size = ntoh32(*((int32_t *)msg));
		char *msg_address = msg + 4;
		int r = osc_match(address, msg_address, &po, &ao);
		if(fullmatch){
			if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
				msg += 4 + size;
				continue;
			}
		}else{
			if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
				msg += 4 + size;
				continue;
			}
		}
		//if(osc_message_s_getArgCount(m) > 0){
		char *tt = msg_address + osc_util_getPaddedStringLen(address);
		if(tt - msg < size + 4 && tt[1] != 0){
			*res = 1;
			break;
		}else{
			msg += 4 + size;
		}
	}
//osc_bndl_it_s_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_addressExists(long len, char *buf, char *address, int fullmatch, int *res)
{
	*res = 0;
	//t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, buf);
	//while(osc_bndl_it_s_hasNext(it)){
	char *msg = buf + OSC_HEADER_SIZE;
	while(msg - buf < len){
		//t_osc_msg_s *m = osc_bndl_it_s_next(it);
		int po, ao;
		//int r = osc_match(address, osc_message_s_getAddress(m), &po, &ao);
		int r = osc_match(address, msg + 4, &po, &ao);
		if(fullmatch){
			if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
				msg += 4 + ntoh32(*((int32_t *)msg));
				continue;
			}
		}else{
			if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
				msg += 4 + ntoh32(*((int32_t *)msg));
				continue;
			}
		}
		*res = 1;
		break;
	}
	//osc_bndl_it_s_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_msg_ar_s *osc_bundle_s_lookupAddress(long len, char *buf, const char *address, int fullmatch)
{
	int matchbuflen = 0, n = 0;
	t_osc_msg_ar_s *ar = NULL;
	char *current_message = buf + OSC_HEADER_SIZE;
	//t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, buf);
	//while(osc_bndl_it_s_hasNext(it)){
	while((current_message - buf) < len){
		//t_osc_msg_s *current_message = osc_bndl_it_s_next(it);
		int32_t size = ntoh32(*((int32_t *)current_message));
		char *current_message_address = current_message + 4;
		int po, ao;
		//int r = osc_match(address, osc_message_s_getAddress(current_message), &po, &ao);
		int r = osc_match(address, current_message_address, &po, &ao);
		if(fullmatch){
			if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
				goto cont;
			}
		}else{
			if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
				goto cont;
			}
		}
		//osc_message_s_copy(matches + n++, current_message);
		if(n >= matchbuflen){
			//matches = osc_mem_resize(matches, (matchbuflen + 16) * sizeof(t_osc_msg_s *));
			if(!ar){
				ar = osc_message_array_s_alloc(16);
				if(!ar){
					return NULL;
				}
			}else{
				t_osc_err e = osc_array_resize(ar, matchbuflen + 16);
				if(e){
					return NULL;
				}
			}
			matchbuflen += 16;
		}
		t_osc_msg_s *p = osc_array_get(ar, n++);
		//osc_message_s_copy(&p, current_message);
		osc_message_s_wrap(p, current_message);
	cont:
		current_message += size + 4;
	}
	//osc_bndl_it_s_destroy(it);
//*nmatches = n;
//*m = matches;
	if(ar){
		osc_array_resize(ar, n);
	}
	//*osc_msg_s_array = ar;
	return ar;
}

t_osc_msg_ar_s *osc_bundle_s_lookupAddress_b(t_osc_bndl_s *bndl, const char *address, int fullmatch)
{
	return osc_bundle_s_lookupAddress(bndl->len, bndl->ptr, address, fullmatch);
}

char *osc_bundle_s_getFirstFullMatch(long len, char *ptr, char *address)
{
	char *msg = ptr + OSC_HEADER_SIZE;
	while(msg - ptr < len){
		int32_t size = ntoh32(*((int32_t *)msg));
		char *msg_address = msg + 4;
		int po, ao;
		int r = osc_match(address, msg_address, &po, &ao);
		if(r == (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
			return msg;
		}
		msg += size + 4;
	}
	return NULL;
}

t_osc_err osc_bundle_s_wrapMessage(long len, char *msg, long *bndllen, char **bndl, char *alloc)
{
	*alloc = 0;
	*bndllen = len + OSC_HEADER_SIZE + 4;
	if(!(*bndl)){
		*bndl = osc_mem_alloc(*bndllen);
		if(!(*bndl)){
			return OSC_ERR_OUTOFMEM;
		}
		*alloc = 1;
	}
	osc_bundle_s_setBundleID(*bndl);
	*((int32_t *)((*bndl) + OSC_HEADER_SIZE)) = hton32(len);
	memcpy((*bndl) + OSC_HEADER_SIZE + 4, msg, len);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_removeMessage(char *address, long *len, char *ptr, int fullmatch)
{
	char tmp[*len];
	memset(tmp, '\0', *len);
	char *p1 = tmp, *p2 = ptr + OSC_HEADER_SIZE, *p3 = ptr + OSC_HEADER_SIZE;
	memcpy(p1, ptr, OSC_HEADER_SIZE);
	p1 += OSC_HEADER_SIZE;
	while((p3 - ptr) < *len){
		int32_t size = ntoh32(*((int32_t *)p3));
		char *pattern = p3 + 4;
		int po = 0, ao = 0;
		int ret = osc_match(pattern, address, &po, &ao);
		if(ret){
			if(fullmatch){
				if(ret != (OSC_MATCH_PATTERN_COMPLETE | OSC_MATCH_ADDRESS_COMPLETE)){
					ret = 0;
				}
			}
		}
		if(ret){
			if(p2 != p3){
				memcpy(p1, p2, p3 - p2);
				p1 += p3 - p2;
			}
			p2 = p3 + size + 4;
		}
		p3 += size + 4;
	}
	if(p2 != p3){
		memcpy(p1, p2, p3 - p2);
		p1 += p3 - p2;
	}
	memcpy(ptr, tmp, p1 - tmp);
	*len = p1 - tmp;
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_replaceMessage(long *buflen,
                                      long *bufpos,
                                      char **bndl,
                                      t_osc_msg_s *oldmsg,
                                      t_osc_msg_s *newmsg)
{
	char *om = osc_message_s_getPtr(oldmsg);
	char *nm = osc_message_s_getPtr(newmsg);
	int32_t old_size = ntoh32(*((int32_t *)om));
	int32_t new_size = ntoh32(*((int32_t *)nm));
	long newbuflen = *bufpos - (old_size + 4) + (new_size + 4);
	if(newbuflen < 0){
		return OSC_ERR_INVAL;
	}
	char *copy = (char *)osc_mem_alloc(newbuflen);
	char *oldptr = NULL;
    char *newptr = NULL;
    
    oldptr = *bndl;
    newptr = copy;
    
	memcpy(newptr, oldptr, om - *bndl);
	newptr += om - *bndl;
	oldptr += om - *bndl;
    
	memcpy(newptr, nm, new_size + 4);
	newptr += new_size + 4;
	oldptr += old_size + 4;
    
	long r = *bufpos - (oldptr - *bndl);
	if(r){
		memcpy(newptr, oldptr, r);
	}
	if(newbuflen > *buflen){
		char *tmp = osc_mem_resize(*bndl, newbuflen);
		if(!tmp){
			return OSC_ERR_OUTOFMEM;
		}
		*bndl = tmp;
		*buflen = newbuflen;
	}
	*bufpos = newbuflen;
	memcpy(*bndl, copy, newbuflen);
    
    if(copy)
    {
        osc_mem_free(copy);
    }
    
	return OSC_ERR_NONE;
}


// if msg == NULL, an empty bundle will be created, or, if a bundle
// already exists, nothing will happen.
t_osc_err osc_bundle_s_appendMessage(long *len, char **bndl, t_osc_msg_s *msg){
	if(!msg){
		//printf("no freaking message\n");
		return OSC_ERR_NONE;
	}
	int32_t msglen = osc_message_s_getSize(msg);
	char *tmp = NULL;
	if(*bndl){
		tmp = (char *)osc_mem_resize(*bndl, *len + msglen + 4);
	}else{
		int size = msglen + 4;
		tmp = (char *)osc_mem_alloc(size + OSC_HEADER_SIZE);
		if(!tmp){
			return OSC_ERR_OUTOFMEM;
		}
		memset(tmp + OSC_IDENTIFIER_SIZE, '\0', OSC_TIMETAG_SIZEOF); // clear timetag
		osc_bundle_s_setBundleID(tmp);
		*len = OSC_HEADER_SIZE;
		*bndl = tmp;
	}
	if(!tmp){
		return OSC_ERR_OUTOFMEM;
	}

	memcpy(tmp + *len, osc_message_s_getPtr(msg), msglen + 4);
	*len = *len + msglen + 4;
	*bndl = tmp;
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_appendMessage_b(t_osc_bndl_s **bndl, t_osc_msg_s *msg){
	if(!(*bndl)){
		*bndl = osc_bundle_s_alloc(0, NULL);
	}
        int ret = osc_bundle_s_appendMessage(&((*bndl)->len), &((*bndl)->ptr), msg);
	return ret;
}

t_osc_err osc_bundle_s_setBundleID(char *buf)
{
	if(!buf){
		return OSC_ERR_NOBUNDLE;
	}
	strncpy(buf, OSC_IDENTIFIER, OSC_IDENTIFIER_SIZE);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_setBundleID_b(t_osc_bndl_s *bndl)
{
	if(!bndl){
		return OSC_ERR_NOBUNDLE;
	}
	if(!(bndl->ptr)){
		bndl->ptr = osc_mem_alloc(OSC_IDENTIFIER_SIZE);
		if(!(bndl->ptr)){
			return OSC_ERR_OUTOFMEM;
		}
	}else{
		if(bndl->len < OSC_HEADER_SIZE){
			bndl->ptr = (char *)osc_mem_resize(bndl->ptr, OSC_HEADER_SIZE);
			if(!bndl->ptr){
				return OSC_ERR_OUTOFMEM;
			}
		}
	}
	memset(bndl->ptr + OSC_IDENTIFIER_SIZE, '\0', OSC_HEADER_SIZE - OSC_IDENTIFIER_SIZE);
	bndl->len = OSC_HEADER_SIZE;
	strncpy(bndl->ptr, OSC_IDENTIFIER, OSC_IDENTIFIER_SIZE);
	return OSC_ERR_NONE;
}

int osc_bundle_s_strcmpID(char *buf)
{
	return strncmp(buf, OSC_ID, OSC_ID_SIZE);
}

t_osc_timetag osc_bundle_s_getTimetag(long len, char *buf)
{
	if(!buf || len < OSC_HEADER_SIZE){
		return OSC_TIMETAG_NULL;
	}
	return osc_timetag_decodeFromHeader(buf + OSC_IDENTIFIER_SIZE);
}

void osc_bundle_s_setTimetag(long len, char *buf, t_osc_timetag t)
{
	if(!buf || len < OSC_HEADER_SIZE){
		return;
	}
	osc_timetag_encodeForHeader(t, buf + OSC_IDENTIFIER_SIZE);
}

t_osc_err osc_bundle_s_flatten(t_osc_bndl_s **dest, 
			       t_osc_bndl_s *src, 
			       int maxlevel, 
			       char *sep, 
			       int remove_enclosing_address_if_empty)
{
	if(!(src)){
		return OSC_ERR_NOBUNDLE;
	}
	t_osc_bndl_u *flattened = NULL;
	t_osc_bndl_u *bu = osc_bundle_s_deserialize(osc_bundle_s_getLen(src), osc_bundle_s_getPtr(src));
	osc_bundle_u_flatten(&flattened, bu, maxlevel, sep, remove_enclosing_address_if_empty);
	t_osc_bndl_s *b = osc_bundle_u_serialize(flattened);
	if(bu){
		osc_bundle_u_free(bu);
	}
	if(flattened){
		osc_bundle_u_free(flattened);
	}
	*dest = b;
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_explode(t_osc_bndl_s **dest, t_osc_bndl_s *src, int maxlevel, char *sep)
{
	if(!(src)){
		return OSC_ERR_NOBUNDLE;
	}
	t_osc_bndl_u *ex = NULL;
	t_osc_bndl_u *bu = osc_bundle_s_deserialize(osc_bundle_s_getLen(src), osc_bundle_s_getPtr(src));
	t_osc_err ret = osc_bundle_u_explode(&ex, bu, maxlevel, sep);
	if(ret){
		return ret;
	}
	if(!(*dest)){
		*dest = osc_bundle_u_serialize(ex);
	}else{
		long len = osc_bundle_u_nserialize(NULL, 0, ex);
		char *ptr = osc_mem_alloc(len);
		osc_bundle_u_nserialize(ptr, len, ex);
		osc_bundle_s_setLen(*dest, len);
		osc_bundle_s_setPtr(*dest, ptr);
	}
	if(bu){
		osc_bundle_u_free(bu);
	}
	if(ex){
		osc_bundle_u_free(ex);
	}
	return OSC_ERR_NONE;
}

t_osc_bndl_u *osc_bundle_s_deserialize(long len, char *ptr)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, ptr);
	while(osc_bndl_it_s_hasNext(it)){
		t_osc_msg_s *m = osc_bndl_it_s_next(it);
		t_osc_msg_u *um = osc_message_s_deserialize(m);
		osc_bundle_u_addMsg(b, um);
	}
	osc_bndl_it_s_destroy(it);
	return b;
}

long osc_bundle_s_getFormattedSize(long len, char *bndl)
{
	return osc_bundle_s_nformat(NULL, 0, len, bndl, 0);
}

char *osc_bundle_s_format(long len, char *bndl)
{
	if(!bndl){
		return NULL;
	}
	long buflen = osc_bundle_s_nformat(NULL, 0, len, bndl, 0) + 1;
	char *buf = osc_mem_alloc(buflen);
	osc_bundle_s_nformat(buf, buflen, len, bndl, 0);
	return buf;
}

char *osc_bundle_s_pformat(t_osc_bundle_s *bndl)
{
	return osc_bundle_s_format(osc_bundle_s_getLen(bndl), osc_bundle_s_getPtr(bndl));
}

long osc_bundle_s_nformat(char *buf, long n, long bndllen, char *bndl, int nindent)
{
	if(!bndl){
		return 0;
	}
	int msgcount = 0;
	t_osc_err e = osc_bundle_s_getMsgCount(bndllen, bndl, &msgcount);
	if(e){
		return 0;
	}
	if(msgcount == 0){
		return 0;
	}
	t_osc_bndl_it_s *it = osc_bndl_it_s_get(bndllen, bndl);
	long offset = 0;
	if(!buf){
		while(osc_bndl_it_s_hasNext(it)){
			t_osc_msg_s *m = osc_bndl_it_s_next(it);
			offset += osc_message_s_nformat(NULL, 0, m, nindent);
			if(osc_bndl_it_s_hasNext(it)){
				offset += snprintf(NULL, 0, ",\n");
			}
		}
	}else{
		while(osc_bndl_it_s_hasNext(it)){
			t_osc_msg_s *m = osc_bndl_it_s_next(it);
			offset += osc_message_s_nformat(buf + offset, n - offset, m, nindent);
			if(osc_bndl_it_s_hasNext(it)){
				offset += snprintf(buf + offset, n - offset, ",\n");
			}
		}
	}
	osc_bndl_it_s_destroy(it);
	return offset;
}

long osc_bundle_s_nformatNestedBndl(char *buf, long n, long bndllen, char *bndl, int nindent)
{
	if(!bndl || nindent < 1){
		return 0;
	}
	long offset = 0;
	char tabs[nindent];
	for(int i = 0; i < nindent - 1; i++){
		tabs[i] = '\t';
	}
	tabs[nindent - 1] = '\0';
	offset += snprintf(buf, n, "{\n");
	if(!buf){
		offset += osc_bundle_s_nformat(NULL, 0, bndllen, bndl, nindent);
		offset += snprintf(NULL, 0, "\n%s}", tabs);
	}else{
		offset += osc_bundle_s_nformat(buf + offset, n - offset, bndllen, bndl, nindent);
		offset += snprintf(buf + offset, n - offset, "\n%s}", tabs);
	}
	return offset;
}

//t_osc_err osc_bundle_s_union(long len1, char *bndl1, long len2, char *bndl2, long *len_out, char **bndl_out)
t_osc_bndl_s *osc_bundle_s_union(t_osc_bndl_s *lhs, t_osc_bndl_s *rhs)
{
	if(!lhs || !rhs){
		return NULL;
	}
	long len1 = osc_bundle_s_getLen(lhs);
	char *bndl1 = osc_bundle_s_getPtr(lhs);
	long len2 = osc_bundle_s_getLen(rhs);
	char *bndl2 = osc_bundle_s_getPtr(rhs);

	long len_out = 0;
	char *bndl_out = NULL;
	t_osc_bndl_s *b = NULL;
	if((len1 == 0 && len2 == 0) || (len1 == OSC_HEADER_SIZE && len2 == OSC_HEADER_SIZE)){
		bndl_out = osc_mem_alloc(OSC_HEADER_SIZE);
		osc_bundle_s_setBundleID(bndl_out);
		len_out = OSC_HEADER_SIZE;
		goto out;
	}
	bndl_out = osc_mem_alloc(len1 + len2);
	memset(bndl_out, '\0', len1 + len2);

	if(bndl1){
		memcpy(bndl_out, bndl1, len1);
		len_out = len1;
	}else if(bndl2){
		memcpy(bndl_out, bndl2, len2);
		len_out = len2;
		goto out;
	}else{
		osc_bundle_s_setBundleID(bndl_out);
		len_out = OSC_HEADER_SIZE;
	}
	char *bndl = bndl_out;
	long len = len_out;
	//char *bndls[2] = {bndl1, bndl2};
	//long lens[2] = {len1, len2};
	//int i = 1;
	//for(i = 0; i < 2; i++){
		//t_osc_bndl_it_s *it = osc_bndl_it_s_get(lens[i], bndls[i]);
		//while(osc_bndl_it_s_hasNext(it)){
		//t_osc_msg_s *m = osc_bndl_it_s_next(it);
		char *m = bndl2 + OSC_HEADER_SIZE;
		while(m - bndl2 < len2){
			//char *address = osc_message_s_getAddress(m);
			int32_t size = ntoh32(*((int32_t *)m));
			char *address = m + 4;
			int res = 0;
			osc_bundle_s_addressExists(len, bndl, address, 1, &res);
			if(res == 0){
				//long l = osc_message_s_getSize(m) + 4;
				long l = size + 4;
				//memcpy(bndl + len, osc_message_s_getPtr(m), l);
				memcpy(bndl + len, m, l);
				len += l;
			}
			m += size + 4;
		}
		//osc_bndl_it_s_destroy(it);
		//}
	len_out = len;
 out:
	b = osc_bundle_s_alloc(len_out, bndl_out);
	return b;
}

t_osc_err osc_bundle_s_intersection(long len1, char *bndl1, long len2, char *bndl2, long *len_out, char **bndl_out)
{
	if(len1 == 0 || len2 == 0 || len1 == OSC_HEADER_SIZE || len2 == OSC_HEADER_SIZE){
		if(!(*bndl_out)){
			*bndl_out = osc_mem_alloc(OSC_HEADER_SIZE);
		}
		osc_bundle_s_setBundleID(*bndl_out);
		*len_out = OSC_HEADER_SIZE;
		return OSC_ERR_NONE;
	}
	if(!(*bndl_out)){
		*bndl_out = osc_mem_alloc(len1 + len2);
		memset(*bndl_out, '\0', len1 + len2);
	}
	char *bndl = *bndl_out;
	long len = OSC_HEADER_SIZE;
	if(bndl1){
		memcpy(*bndl_out, bndl1, OSC_HEADER_SIZE);
	}else if(bndl2){
		memcpy(*bndl_out, bndl2, OSC_HEADER_SIZE);
	}else{
		osc_bundle_s_setBundleID(*bndl_out);
	}

	t_osc_bndl_it_s *it = osc_bndl_it_s_get(len1, bndl1);
	while(osc_bndl_it_s_hasNext(it)){
		t_osc_msg_s *m = osc_bndl_it_s_next(it);
		char *address = osc_message_s_getAddress(m);
		int res = 0;
		osc_bundle_s_addressExists(len2, bndl2, address, 1, &res);
		if(res != 0){
			int res = 0;
			osc_bundle_s_addressExists(len, bndl, address, 1, &res);
			if(res == 0){
				long l = osc_message_s_getSize(m) + 4;
				memcpy(bndl + len, osc_message_s_getPtr(m), l);
				len += l;
			}
		}
	}
	osc_bndl_it_s_destroy(it);
	*len_out = len;
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_s_difference(long len1, char *bndl1, long len2, char *bndl2, long *len_out, char **bndl_out)
{
	if((len1 == 0 && len2 == 0) || (len1 == OSC_HEADER_SIZE && len2 == OSC_HEADER_SIZE)){
		if(!(*bndl_out)){
			*bndl_out = osc_mem_alloc(OSC_HEADER_SIZE);
		}
		osc_bundle_s_setBundleID(*bndl_out);
		*len_out = OSC_HEADER_SIZE;
		return OSC_ERR_NONE;
	}
	if(!(*bndl_out)){
		*bndl_out = osc_mem_alloc(len1 + len2);
		memset(*bndl_out, '\0', len1 + len2);
	}

	char *bndl = *bndl_out;
	long len = OSC_HEADER_SIZE;
	if(bndl1){
		memcpy(*bndl_out, bndl1, OSC_HEADER_SIZE);
	}else if(bndl2){
		memcpy(*bndl_out, bndl2, OSC_HEADER_SIZE);
	}else{
		osc_bundle_s_setBundleID(*bndl_out);
	}

	long lens[2] = {len1, len2};
	char *bndls[2] = {bndl1, bndl2};
	int i;
	for(i = 0; i < 2; i++){
		t_osc_bndl_it_s *it = osc_bndl_it_s_get(lens[i], bndls[i]);
		while(osc_bndl_it_s_hasNext(it)){
			t_osc_msg_s *m = osc_bndl_it_s_next(it);
			char *address = osc_message_s_getAddress(m);
			int res = 0;
			osc_bundle_s_addressExists(lens[(i + 1) % 2], bndls[(i + 1) % 2], address, 1, &res);
			if(res == 0){
				int res = 0;
				osc_bundle_s_addressExists(len, bndl, address, 1, &res);
				if(res == 0){
					long l = osc_message_s_getSize(m) + 4;
					memcpy(bndl + len, osc_message_s_getPtr(m), l);
					len += l;
				}
			}
		}
		osc_bndl_it_s_destroy(it);
	}

	*len_out = len;
	return OSC_ERR_NONE;
}

int osc_bundle_s_concat(long len1, char *bndl1, long len2, char *bndl2, char *bndl){
	if(!bndl1 && !bndl2){
		return 0;
	}
	if(!bndl1){
		memcpy(bndl, bndl2, len2);
		return len2;
	}
	if(!bndl2){
		memcpy(bndl, bndl1, len1);
		return len1;
	}
	memcpy(bndl, bndl1, len1);
	memcpy(bndl + len1, bndl2 + OSC_HEADER_SIZE, len2 - OSC_HEADER_SIZE);
	return len1 + len2 - OSC_HEADER_SIZE;
}

t_osc_bundle_array_s *osc_bundle_array_s_alloc(long len)
{
	return (t_osc_bundle_array_s *)osc_array_allocWithSize(len, sizeof(t_osc_bndl_s));
}

void osc_bundle_array_s_free(t_osc_bundle_array_s *ar)
{
	osc_array_free((t_osc_array *)ar);
}

void osc_bundle_array_s_clear(t_osc_bundle_array_s *ar)
{
	osc_array_clear((t_osc_array *)ar);
}

t_osc_bndl_s *osc_bundle_array_s_get(t_osc_bundle_array_s *ar, long idx)
{
	return (t_osc_bndl_s *)osc_array_get((t_osc_array *)ar, idx);
}

long osc_bundle_array_s_getLen(t_osc_bundle_array_s *ar)
{
	return osc_array_getLen((t_osc_array *)ar);
}

t_osc_bundle_array_s *osc_bundle_array_s_copy(t_osc_bundle_array_s *ar)
{
	return (t_osc_bundle_array_s *)osc_array_copy((t_osc_array *)ar);
}

t_osc_err osc_bundle_array_s_resize(t_osc_bundle_array_s *ar, long newlen)
{
	return osc_array_resize((t_osc_array *)ar, newlen);
}
