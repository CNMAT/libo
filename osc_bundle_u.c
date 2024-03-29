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
#include "osc_byteorder.h"
#include "osc_match.h"
#include "osc_message_u.h"
#include "osc_message_u.r"
#include "osc_message_iterator_u.h"
#include "osc_atom_u.h"
#include "osc_atom_u.r"
#include "osc_bundle_u.h"
#include "osc_bundle_u.r"
#include "osc_bundle_iterator_u.h"
#include "osc_array.h"

t_osc_bndl_u *osc_bundle_u_alloc(void)
{
	t_osc_bndl_u *b = osc_mem_alloc(sizeof(t_osc_bndl_u));
	memset(b, '\0', sizeof(t_osc_bndl_u));
	return b;
}

size_t osc_bundle_u_getStructSize(void)
{
	return sizeof(t_osc_bndl_u);
}

void osc_bundle_u_free(t_osc_bndl_u *bndl)
{
	if(!bndl){
		return;
	}
	t_osc_msg_u *m = bndl->msghead;
	while(m){
		t_osc_msg_u *next = m->next;
		osc_message_u_free(m);
		m = next;
	}
	osc_mem_free(bndl);
}

void osc_bundle_u_clear(t_osc_bndl_u *bndl)
{
	if(!bndl){
		return;
	}
	bndl->msgcount = 0;
	t_osc_msg_u *m = bndl->msghead;
	while(m){
		t_osc_msg_u *next = m->next;
		osc_message_u_free(m);
		m = next;
	}
	bndl->msghead = NULL;
	bndl->msgtail = NULL;
}

t_osc_err osc_bundle_u_copy(t_osc_bndl_u **dest, t_osc_bndl_u *src)
{
	t_osc_bndl_s *bs = osc_bundle_u_serialize(src);
	*dest = osc_bundle_s_deserialize(osc_bundle_s_getLen(bs), osc_bundle_s_getPtr(bs));
	osc_bundle_s_deepFree(bs);
	return OSC_ERR_NONE;
}
/*
void osc_bundle_u_setTimetag(t_osc_bndl_u *bndl, t_osc_timetag t)
{
	if(!bndl){
		return;
	}
	bndl->timetag = t;
}

t_osc_timetag osc_bundle_u_getTimetag(t_osc_bndl_u *bndl)
{
	if(bndl){
		return bndl->timetag;
	}
	return 0;
}
*/
int osc_bundle_u_getMsgCount(t_osc_bndl_u *bndl)
{
	if(bndl){
		return bndl->msgcount;
	}
	return 0;
}

t_osc_msg_u *osc_bundle_u_getFirstMsg(t_osc_bndl_u *bndl)
{
	if(bndl){
		return bndl->msghead;
	}
	return NULL;
}

t_osc_err osc_bundle_u_getMessagesWithCallback(t_osc_bndl_u *bndl, void (*f)(t_osc_msg_u*, void *), void *context)
{
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
	while(osc_bndl_it_u_hasNext(it)){
		t_osc_msg_u *m = osc_bndl_it_u_next(it);
		f(m, context);
	}
	osc_bndl_it_u_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addressExists(t_osc_bndl_u *bndl, char *address, int fullmatch, int *res)
{
	*res = 0;
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
	while(osc_bndl_it_u_hasNext(it)){
		t_osc_msg_u *m = osc_bndl_it_u_next(it);
		int po, ao;
		char *a = osc_message_u_getAddress(m);
		if(!a){
			continue;
		}
		int r = osc_match(address, osc_message_u_getAddress(m), &po, &ao);
		if(fullmatch){
			if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
				continue;
			}
		}else{
			if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
				continue;
			}
		}
		*res = 1;
		break;
	}
	osc_bndl_it_u_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addressIsBound(t_osc_bndl_u *bndl, char *address, int fullmatch, int *res)
{
    *res = 0;
    t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
    while(osc_bndl_it_u_hasNext(it)){
        t_osc_msg_u *m = osc_bndl_it_u_next(it);
        int po, ao;
        char *a = osc_message_u_getAddress(m);
        if(!a){
            continue;
        }
        int r = osc_match(address, osc_message_u_getAddress(m), &po, &ao);
        if(fullmatch){
            if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
                continue;
            }
        }else{
            if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
                continue;
            }
        }
        *res = (osc_message_u_getArgCount(m) > 0);
        break;
    }
    osc_bndl_it_u_destroy(it);
    return OSC_ERR_NONE;
}

t_osc_msg_u *osc_bundle_u_getFirstFullMatch(t_osc_bndl_u *bndl, char *address)
{
    t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
    while(osc_bndl_it_u_hasNext(it)){
        t_osc_msg_u *m = osc_bndl_it_u_next(it);
        int po, ao;
        char *a = osc_message_u_getAddress(m);
        if(!a){
            continue;
        }

        if( osc_match(address, osc_message_u_getAddress(m), &po, &ao) ==
           (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE))
        {
            osc_bndl_it_u_destroy(it);
            return m;
        }
    }
    osc_bndl_it_u_destroy(it);
    return NULL;
}

t_osc_msg_ar_u *osc_bundle_u_lookupAddress(t_osc_bndl_u *bndl, const char *address, int fullmatch)
{
	int matchbuflen = 16, n = 0;
	//t_osc_msg_u **matches = osc_mem_alloc(matchbuflen * sizeof(t_osc_msg_u *));
	t_osc_msg_ar_u *ar = osc_message_array_u_alloc(matchbuflen);
	osc_array_clear(ar);
	//memset(matches, '\0', matchbuflen * sizeof(t_osc_msg_u *));
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
	while(osc_bndl_it_u_hasNext(it)){
		if(n >= matchbuflen){
			//matches = osc_mem_resize(matches, (matchbuflen + 16) * sizeof(t_osc_msg_u *));
			t_osc_err e = osc_array_resize(ar, matchbuflen + 16);
			if(e){
				return NULL;
			}
			matchbuflen += 16;
		}
		t_osc_msg_u *current_message = osc_bndl_it_u_next(it);
		int po, ao;
		int r = osc_match(address, osc_message_u_getAddress(current_message), &po, &ao);
		if(fullmatch){
			if(r != (OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE)){
				continue;
			}
		}else{
			if(r == 0 || (((r & OSC_MATCH_PATTERN_COMPLETE) == 0) && address[po] != '/')){
				continue;
			}
		}
		//osc_message_u_copy(matches + n++, current_message);
		t_osc_msg_u *p = osc_array_get(ar, n++);
		osc_message_u_deepCopy(&p, current_message);
	}
	osc_bndl_it_u_destroy(it);
	osc_array_resize(ar, n);
	return ar;
}

static t_osc_err osc_bundle_u_addMsg_impl(t_osc_bndl_u *bndl, t_osc_msg_u *msg, int remove_dups)
{
	if(!bndl || !msg){
		return OSC_ERR_NONE;
	}
	if(remove_dups == 1)
    {
		char *address = osc_message_u_getAddress(msg);
		if(address){
			t_osc_msg_u *m = bndl->msghead;
			while(m){
				t_osc_msg_u *next = m->next;
                const char * const messageaddr = osc_message_u_getAddress(m);
				if(messageaddr && !strcmp(address, messageaddr)){
					osc_bundle_u_removeMsg(bndl, m);
					osc_message_u_free(m);
					bndl->msgcount--;
				}
				m = next;
			}
		}
	}
    else if(remove_dups == 2)
    {
        char *address = osc_message_u_getAddress(msg);
        if(address){
            t_osc_msg_u *m = bndl->msghead;
            while(m){
                t_osc_msg_u *next = m->next;
                if(!strcmp(address, osc_message_u_getAddress(m))){
                    // later maybe try to maintain memmory location if the value is a bundle?
                    // in that case we might not clear first, but copy the new value into the bundle at this message
                    t_osc_msg_u * next = m->next;
                    t_osc_msg_u * prev = m->prev;
                    
                    osc_message_u_clearArgs(m);
                    osc_message_u_deepCopy(&m, msg);
                    
                    m->next = next;
                    m->prev = prev;
                    
                    osc_message_u_free(msg);
                    msg = NULL;
                    return OSC_ERR_NONE;
                }
                m = next;
            }
        }
    }
    
	bndl->msgcount++;
	if(!(bndl->msghead)){
		bndl->msghead = msg;
		bndl->msgtail = NULL;
		msg->prev = msg->next = NULL;
		return OSC_ERR_NONE;
	}
	if(bndl->msghead && !(bndl->msgtail)){
		bndl->msghead->next = msg;
		msg->prev = bndl->msghead;
		msg->next = NULL;
		bndl->msgtail = msg;
		return OSC_ERR_NONE;
	}
	msg->prev = bndl->msgtail;
	msg->next = NULL;
	bndl->msgtail->next = msg;
	bndl->msgtail = msg;
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addMsg(t_osc_bndl_u *bndl, t_osc_msg_u *msg)
{
	return osc_bundle_u_addMsg_impl(bndl, msg, 0);
}

t_osc_err osc_bundle_u_addMsgWithoutDups(t_osc_bndl_u *bndl, t_osc_msg_u *msg)
{
	return osc_bundle_u_addMsg_impl(bndl, msg, 1);
}

t_osc_err osc_bundle_u_replaceMessage(t_osc_bndl_u *bndl, t_osc_msg_u *msg)
{
    return osc_bundle_u_addMsg_impl(bndl, msg, 2);
    
}

t_osc_err osc_bundle_u_addMsgCopy(t_osc_bndl_u *bndl, t_osc_msg_u *msg)
{
	if(bndl && msg){
		t_osc_msg_u *copy = NULL;
		osc_message_u_deepCopy(&copy, msg);
		osc_bundle_u_addMsg(bndl, copy);
		return OSC_ERR_NONE;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addMsgWithoutDupsCopy(t_osc_bndl_u *bndl, t_osc_msg_u *msg)
{
	if(bndl && msg){
		t_osc_msg_u *copy = NULL;
		osc_message_u_deepCopy(&copy, msg);
		osc_bundle_u_addMsgWithoutDups(bndl, copy);
		return OSC_ERR_NONE;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addMsgArrayCopy(t_osc_bndl_u *bndl, t_osc_msg_ar_u *ar)
{
	if(!bndl){
		return OSC_ERR_NOBUNDLE;
	}
	if(!ar){
		return OSC_ERR_NONE;
	}
	int len = osc_message_array_u_getLen(ar);
	if(len == 0){
		return OSC_ERR_NONE;
	}
	int i;
	for(i = 0; i < len; i++){
		osc_bundle_u_addMsgCopy(bndl, osc_message_array_u_get(ar, i));
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_addMsgList(t_osc_bndl_u *bndl, t_osc_msg_u *msgs)
{
	if(!bndl){
		return OSC_ERR_NOBUNDLE;
	}
	int n = 0;
	t_osc_msg_u *last = msgs;
	bndl->msghead = msgs;
	while(msgs){
		n++;
		last = msgs;
		msgs = osc_message_u_next(msgs);
	}
	bndl->msgcount = n;
	bndl->msgtail = last;
	return 0;
}

t_osc_err osc_bundle_u_removeMsg(t_osc_bndl_u *bndl, t_osc_msg_u *m)
{
	if(!bndl || !m){
		return OSC_ERR_NONE;
	}
	if(m->next){
		m->next->prev = m->prev;
	}
	if(m->prev){
		m->prev->next = m->next;
	}
	if(m == bndl->msghead){
		bndl->msghead = m->next;
	}
	if(m == bndl->msgtail){
		bndl->msgtail = m->prev;
	}
	return OSC_ERR_NONE;
}

void osc_bundle_u_removeMessage(t_osc_bndl_u * bndl, const char *address)
{
    if(address){
        t_osc_msg_u *m = bndl->msghead;
        while(m){
            t_osc_msg_u *next = m->next;
            if(!strcmp(address, osc_message_u_getAddress(m))){
                osc_bundle_u_removeMsg(bndl, m);
                osc_message_u_free(m);
                bndl->msgcount--;
            }
            m = next;
        }
    }
}

static t_osc_err osc_bundle_u_flatten_impl(t_osc_bndl_u **dest,
					   t_osc_bndl_u *src,
					   int maxlevel,
					   int level,
					   char *prefix,
					   char *sep,
					   int remove_enclosing_address_if_empty)
{
	if(!sep){
		sep = "";
	}
	if(!(*dest)){
		*dest = osc_bundle_u_alloc();
	}
	t_osc_bndl_it_u *bit = osc_bndl_it_u_get(src);
	while(osc_bndl_it_u_hasNext(bit)){
		t_osc_msg_u *m = osc_bndl_it_u_next(bit);
		t_osc_msg_u *mcopy = NULL;
		osc_message_u_deepCopy(&mcopy, m);
		if((level < maxlevel) || (maxlevel <= 0)){
			t_osc_msg_it_u *mit = osc_msg_it_u_get(mcopy);
			while(osc_msg_it_u_hasNext(mit)){
				t_osc_atom_u *a = osc_msg_it_u_next(mit);
				if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
					osc_message_u_removeAtom(mcopy, a);
					t_osc_bndl_u *bu = osc_atom_u_getBndl(a);
					/*
					t_osc_bndl_u *bu = NULL;
					osc_bundle_s_deserialize(osc_bundle_s_getLen(b),
								 osc_bundle_s_getPtr(b),
								 &bu);
					*/
					t_osc_err e;
					if(prefix){
						int prefixlen = strlen(prefix) + strlen(osc_message_u_getAddress(mcopy)) + strlen(sep);
						char pfx[prefixlen + 1];
						sprintf(pfx, "%s%s%s", prefix, sep, osc_message_u_getAddress(mcopy));
						e = osc_bundle_u_flatten_impl(dest,
									      bu,
									      maxlevel,
									      level + 1,
									      pfx,
									      sep,
									      remove_enclosing_address_if_empty);
					}else{
						e = osc_bundle_u_flatten_impl(dest,
									      bu,
									      maxlevel,
									      level + 1,
									      osc_message_u_getAddress(mcopy),
									      sep,
									      remove_enclosing_address_if_empty);
					}
					if(e){
						return e;
					}
					osc_atom_u_free(a);
				}
			}
			osc_msg_it_u_destroy(mit);
		}
		if(!remove_enclosing_address_if_empty || osc_message_u_getArgCount(mcopy) > 0){
			if(prefix){
				int newaddresslen = strlen(prefix) + strlen(osc_message_u_getAddress(mcopy)) + strlen(sep);
				char newaddress[newaddresslen + 1];
				sprintf(newaddress, "%s%s%s", prefix, sep, osc_message_u_getAddress(mcopy));
				osc_message_u_setAddress(mcopy, newaddress);
			}
			osc_bundle_u_addMsgWithoutDups(*dest, mcopy);
		}else{
			osc_message_u_free(mcopy);
		}
	}
	osc_bndl_it_u_destroy(bit);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_flatten(t_osc_bndl_u **dest,
			       t_osc_bndl_u *src,
			       int maxlevel,
			       char *sep,
			       int remove_enclosing_address_if_empty)
{
	return osc_bundle_u_flatten_impl(dest, src, maxlevel, 0, NULL, sep ? sep : "", remove_enclosing_address_if_empty);
}

t_osc_err osc_bundle_u_explode(t_osc_bndl_u **dest,
			       t_osc_bndl_u *src,
			       int maxlevel,
			       char *sep)
{
	if(!src){
		return OSC_ERR_NOBUNDLE;
	}
	if(!(*dest)){
		*dest = osc_bundle_u_alloc();
	}
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(src);
	while(osc_bndl_it_u_hasNext(it)){
		t_osc_msg_u *m = osc_bndl_it_u_next(it);
		t_osc_err ret = osc_message_u_explode(*dest, m, maxlevel, sep);
		if(ret){
			return ret;
		}
	}
	osc_bndl_it_u_destroy(it);
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_union(t_osc_bndl_u *bndl1, t_osc_bndl_u *bndl2, t_osc_bndl_u **bndl_out)
{
	if(!(*bndl_out)){
		*bndl_out = osc_bundle_u_alloc();
	}
	int argc1 = osc_bundle_u_getMsgCount(bndl1);
	int argc2 = osc_bundle_u_getMsgCount(bndl2);
	if(argc1 == 0 && argc2 == 0){
		return OSC_ERR_NONE;
	}
	t_osc_bndl_u *bndls[2] = {bndl1, bndl2};
	for(int i = 0; i < 2; i++){
		t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndls[i]);
		while(osc_bndl_it_u_hasNext(it)){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			char *address = osc_message_u_getAddress(m);
			int res = 0;
			osc_bundle_u_addressExists(*bndl_out, address, 1, &res);
			if(res == 0){
				t_osc_msg_u *mcopy = NULL;
				osc_message_u_deepCopy(&mcopy, m);
				osc_bundle_u_addMsg(*bndl_out, mcopy);
			}
		}
		osc_bndl_it_u_destroy(it);
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_bundle_u_intersection(t_osc_bndl_u *bndl1, t_osc_bndl_u *bndl2, t_osc_bndl_u **bndl_out)
{
	if(!(*bndl_out)){
		*bndl_out = osc_bundle_u_alloc();
	}
	int argc1 = osc_bundle_u_getMsgCount(bndl1);
	int argc2 = osc_bundle_u_getMsgCount(bndl2);
	if(argc1 == 0 || argc2 == 0){
		return OSC_ERR_NONE;
	}
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl1);
	while(osc_bndl_it_u_hasNext(it)){
		t_osc_msg_u *m = osc_bndl_it_u_next(it);
		char *address = osc_message_u_getAddress(m);
		int res = 0;
		osc_bundle_u_addressExists(bndl2, address, 1, &res);
		if(res != 0){
			int res = 0;
			osc_bundle_u_addressExists(*bndl_out, address, 1, &res);
			if(res == 0){
				t_osc_msg_u *mcopy = NULL;
				osc_message_u_deepCopy(&mcopy, m);
				osc_bundle_u_addMsg(*bndl_out, mcopy);
			}
		}
	}
	osc_bndl_it_u_destroy(it);
	return OSC_ERR_NONE;
}

long osc_bundle_u_getSerializedSize(t_osc_bndl_u *b)
{
	return osc_bundle_u_nserialize(NULL, 0, b);
}

t_osc_bndl_s *osc_bundle_u_serialize(t_osc_bndl_u *b)
{
	size_t n = osc_bundle_u_nserialize(NULL, 0, b);
	if(!n){
		return NULL;
	}
	char *buf = osc_mem_alloc(n);
	osc_bundle_u_nserialize(buf, n, b);
	t_osc_bndl_s *bs = osc_bundle_s_alloc(n, buf);
	return bs;
}

size_t osc_bundle_u_nserialize(char *buf, size_t n, t_osc_bndl_u *b)
{
	size_t _n = 0;
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(b);
	if(!buf){
		_n += OSC_HEADER_SIZE;
		while(osc_bndl_it_u_hasNext(it)){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			_n += osc_message_u_nserialize(NULL, 0, m);
		}
	}else{
		if(n < OSC_HEADER_SIZE){
			return 0;
		}
		memcpy(buf, OSC_EMPTY_HEADER, OSC_HEADER_SIZE);
		_n += OSC_HEADER_SIZE;
		while(osc_bndl_it_u_hasNext(it) && _n < n){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			_n += osc_message_u_nserialize(buf + _n, n - _n, m);
		}
	}
	osc_bndl_it_u_destroy(it);
	return _n;
}

long osc_bundle_u_getFormattedSize(t_osc_bndl_u *bndl)
{
	return osc_bundle_u_nformat(NULL, 0, bndl, 0);
}

char *osc_bundle_u_format(t_osc_bndl_u *bndl)
{
	if(!bndl){
		return NULL;
	}
	long buflen = osc_bundle_u_nformat(NULL, 0, bndl, 0) + 1;
	char *buf = osc_mem_alloc(buflen);
	osc_bundle_u_nformat(buf, buflen, bndl, 0);
	return buf;
}

long osc_bundle_u_nformat(char *buf, long n, t_osc_bndl_u *bndl, int nindent)
{
	if(!bndl || osc_bundle_u_getMsgCount(bndl) == 0){
		return 0;
	}
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(bndl);
	long offset = 0;
	if(!buf){
		while(osc_bndl_it_u_hasNext(it)){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			offset += osc_message_u_nformat(NULL, 0, m, nindent);
			if(osc_bndl_it_u_hasNext(it)){
				offset += snprintf(NULL, 0, ",\n");
			}
		}
	}else{
		while(osc_bndl_it_u_hasNext(it) && offset < n){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			offset += osc_message_u_nformat(buf + offset, n - offset, m, nindent);
			if(osc_bndl_it_u_hasNext(it)){
				offset += snprintf(buf + offset, n - offset, ",\n");
			}
		}
	}
	osc_bndl_it_u_destroy(it);
	return offset;
}

long osc_bundle_u_nformatNestedBndl(char *buf, long n, t_osc_bndl_u *bndl, int nindent)
{
	if(!bndl || nindent < 1){
		return 0;
	}
	long offset = 0;
	int nspaces = (nindent - 1) * OSC_SUBBUNDLE_INDENT;
	char tabs[nspaces + 1];
	for(int i = 0; i < nspaces; i++){
		tabs[i] = ' ';
	}
	tabs[nspaces] = '\0';
	if(!buf){
		offset += snprintf(NULL, 0, "{\n");
		offset += osc_bundle_u_nformat(NULL, 0, bndl, nindent);
		offset += snprintf(NULL, 0, "\n%s}", tabs);
	}else{
		offset += snprintf(buf + offset, n - offset, "{\n");
		offset += osc_bundle_u_nformat(buf + offset, n - offset, bndl, nindent);
		offset += snprintf(buf + offset, n - offset, "\n%s}", tabs);

	}
	return offset;
}

t_osc_bundle_array_u *osc_bundle_array_u_alloc(long len)
{
	return osc_array_allocWithSize(len, sizeof(t_osc_bndl_u));
}

void osc_bundle_array_u_free(t_osc_bundle_array_u *ar)
{
	osc_array_free((t_osc_array *)ar);
}

void osc_bundle_array_u_clear(t_osc_bundle_array_u *ar)
{
	osc_array_clear((t_osc_array *)ar);
}

t_osc_bndl_u *osc_bundle_array_u_get(t_osc_bundle_array_u *ar, long idx)
{
	return (t_osc_bndl_u *)osc_array_get((t_osc_array *)ar, idx);
}

long osc_bundle_array_u_getLen(t_osc_bundle_array_u *ar)
{
	return osc_array_getLen((t_osc_array *)ar);
}

t_osc_bundle_array_u *osc_bundle_array_u_copy(t_osc_bundle_array_u *ar)
{
	return (t_osc_bundle_array_u *)osc_array_copy((t_osc_array *)ar);
}

t_osc_err osc_bundle_array_u_resize(t_osc_bundle_array_u *ar, long newlen)
{
	return osc_array_resize((t_osc_array *)ar, newlen);
}
