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
#include "osc_message_s.r"
#include "osc_message_s.h"
#include "osc_message_iterator_s.h"
#include "osc_message_u.h"
#include "osc_array.h"
#include "osc_mem.h"
#include "osc_util.h"

t_osc_msg_s *osc_message_s_alloc(void)
{
	t_osc_msg_s *m = (t_osc_msg_s *)osc_mem_alloc(sizeof(t_osc_msg_s));
	osc_message_s_initMsg(m);
	return m;
}

size_t osc_message_s_getStructSize(void)
{
	return sizeof(t_osc_msg_s);
}

void osc_message_s_free(t_osc_msg_s *m)
{
	if(!m){
		return;
	}
	if(m->data_offset_cache){
		osc_mem_free(m->data_offset_cache);
	}
	if(m->data_size_cache){
		osc_mem_free(m->data_size_cache);
	}
	osc_mem_free(m);
}

void osc_message_s_deepFree(t_osc_msg_s *m)
{
	if(!m){
		return;
	}
	if(m->size){
		osc_mem_free(m->size);
	}
	osc_message_s_free(m);
}

void osc_message_s_initMsg(t_osc_msg_s *m)
{
	if(!m){
		return;
	}
	memset(m, '\0', sizeof(t_osc_msg_s));
}

void osc_message_s_copy(t_osc_msg_s **dest, t_osc_msg_s *src)
{
	if(!(*dest)){
		*dest = osc_message_s_alloc();
	}
	**dest = *src;
}

t_osc_err osc_message_s_wrap(t_osc_msg_s *m, char *bytes)
{
	if(!m || !bytes){
		return OSC_ERR_NULLPTR;
	}
	int32_t len = ntoh32(*((int32_t *)bytes));
	m->size = bytes;
	char *address = m->address = bytes + 4;
	/*
	if(!(m->address)){
		return OSC_ERR_MALFORMEDADDRESS;
	}
	if(m->address[0] != '/'){
		return OSC_ERR_MALFORMEDADDRESS;
	}
	*/
	/*
	m->typetags = bytes + 4 + strlen(bytes + 4) + 1;
	while((m->typetags - bytes) % 4){
		m->typetags++;
	}
	*/
	char *tt = m->typetags = address + osc_util_getPaddedStringLen(address);
	if((tt - bytes) > len){
		m->typetags = NULL;
		// this isn't really cool--if there is no data, there should at least be a 
		// comma and 3 NULLs.  we'll let it go anyway...
		return OSC_ERR_NONE;
	}
	/*
	m->data = m->typetags + strlen(m->typetags) + 1;
	while((m->data - bytes) % 4){
		m->data++;
	}
	*/
	m->data = tt + osc_util_getPaddedStringLen(tt);
	return OSC_ERR_NONE;
}

int32_t osc_message_s_renameCopy(char *dest, t_osc_msg_s *src, int32_t new_address_len, char *new_address)
{
	if(!dest){
		return 0;
	}
	if(osc_error_validateAddress(new_address)){
		return 0;
	}
	int32_t oldlen = osc_message_s_getSize(src);
	int32_t old_address_len = strlen(osc_message_s_getAddress(src));
	int32_t newlen = oldlen - (old_address_len - new_address_len);
	while(newlen % 4){
		newlen++;
	}
	*((int32_t *)dest) = hton32(newlen);
	char *ptr = dest + 4;
	if(new_address_len > 0){
		memcpy(ptr, new_address, new_address_len);
	}
	ptr += new_address_len;
	*ptr++ = '\0';
	while((ptr - dest) % 4){
		*ptr++ = '\0';
	}
	memcpy(ptr, src->typetags, oldlen - (src->typetags - src->address));
	return newlen;
}

int32_t osc_message_s_getSize(t_osc_msg_s *m)
{
	if(m){
		if(m->size){
			return ntoh32(*((int32_t *)(m->size)));
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

char *osc_message_s_getAddress(t_osc_msg_s *m){
	if(m){
		return m->address;
	}else{
		return NULL;
	}
}

char *osc_message_s_getTypetags(t_osc_msg_s *m){
	if(m){
		return m->typetags;
	}else{
		return NULL;
	}
}

char *osc_message_s_getData(t_osc_msg_s *m){
	if(m){
		return m->data;
	}else{
		return NULL;
	}
}

int osc_message_s_getArgCount(t_osc_msg_s *m){
	if(m){
		if(!(m->typetags)){
			return 0;
		}
		return strlen(m->typetags) - 1;
	}else{
		return 0;
	}
}

char *osc_message_s_getPtr(t_osc_msg_s *m){
	if(m){
		return m->size;
	}else{
		return NULL;
	}
}

#define OSC_MESSAGE_S_CAREFUL
char osc_message_s_getTypetag(t_osc_msg_s *m, int n){
#ifdef OSC_MESSAGE_S_CAREFUL
	int nn = osc_message_s_getArgCount(m);
	if(nn <= n){
		return 0;
	}
#endif
	return m->typetags[n + 1];
}

void osc_message_s_getArg(t_osc_msg_s *m, int n, t_osc_atom_s **atom){
#ifdef OSC_MESSAGE_S_CAREFUL
	int nn = osc_message_s_getArgCount(m);
	if(nn <= n){
		return;
	}
#endif
	char tt = osc_message_s_getTypetag(m, n);
	if(m->cache_size){
		if(*atom){
			osc_atom_s_set(*atom, tt, m->data + m->data_offset_cache[n]);
		}else{
			*atom = osc_atom_s_alloc(tt, m->data + m->data_offset_cache[n]);
		}
		return;
	}else{
		int offset = 0;
		int i;
		for(i = 0; i < n; i++){
			offset += osc_sizeof(osc_message_s_getTypetag(m, i), m->data + offset);
		}
		if(*atom){
			osc_atom_s_set(*atom, tt, m->data + offset);
		}else{
			*atom = osc_atom_s_alloc(tt, m->data + offset);
		}
		return;
	}
}

t_osc_err osc_message_s_cacheDataOffsets(t_osc_msg_s *m)
{
	int n = osc_message_s_getArgCount(m);
	m->data_offset_cache = (int *)osc_mem_alloc(n * sizeof(int));
	m->data_size_cache = (int *)osc_mem_alloc(n * sizeof(int));
	m->cache_buf_size = n;
	m->cache_size = n;
	int i;
	int offset = 0;
	for(i = 0; i < n; i++){
		m->data_offset_cache[i] = offset;
		int size = osc_sizeof(osc_message_s_getTypetag(m, i), m->data + offset);
		m->data_size_cache[i] = size;
		offset += size;
	}
	return OSC_ERR_NONE;
}

t_osc_msg_u *osc_message_s_deserialize(t_osc_msg_s *msg)
{
	t_osc_msg_u *m = osc_message_u_alloc();
	osc_message_u_setAddress(m, osc_message_s_getAddress(msg));
	t_osc_msg_it_s *it = osc_msg_it_s_get(msg);
	while(osc_msg_it_s_hasNext(it)){
		t_osc_atom_s *a = osc_msg_it_s_next(it);
		t_osc_atom_u *ua = osc_atom_s_deserialize(a);
		osc_message_u_appendAtom(m, ua);
	}
	osc_msg_it_s_destroy(it);
	return m;
}

long osc_message_s_getFormattedSize(t_osc_msg_s *m)
{
	return osc_message_s_nformat(NULL, 0, m, 0);
}

char *osc_message_s_format(t_osc_msg_s *m)
{
	if(!m){
		return NULL;
	}
	long len = osc_message_s_nformat(NULL, 0, m, 0) + 1;
	char *buf = osc_mem_alloc(len);
	osc_message_s_nformat(buf, len, m, 0);
	return buf;
}

long osc_message_s_nformat(char *buf, long n, t_osc_msg_s *m, int nindent)
{
	if(!m){
		return 0;
	}
	long numargs = osc_message_s_getArgCount(m);
	long offset = 0;
	t_osc_msg_it_s *it = osc_msg_it_s_get(m);
	char tabs[nindent + 1];
	for(int i = 0; i < nindent; i++){
		tabs[i] = '\t';
	}
	tabs[nindent] = '\0';
	if(!buf){
		if(numargs){
			if(numargs > 1){
				offset += snprintf(NULL, 0, "%s%s : [", tabs, osc_message_s_getAddress(m));
			}else{
				offset += snprintf(NULL, 0, "%s%s : ", tabs, osc_message_s_getAddress(m));
			}
		}else{
			offset += snprintf(NULL, 0, "%s%s", tabs, osc_message_s_getAddress(m));
		}
		while(osc_msg_it_s_hasNext(it)){
			//offset += snprintf(NULL, 0, " ");
			t_osc_atom_s *a = osc_msg_it_s_next(it);
			offset += osc_atom_s_nformat(NULL, 0, a, nindent);
			if(osc_msg_it_s_hasNext(it)){
				//if(osc_atom_s_getTypetag(a) == OSC_BUNDLE_TYPETAG){
				//offset += snprintf(NULL, 0, ",\n");
					//}else{
					offset += snprintf(NULL, 0, ", ");
					//}
			}
		}		
		if(numargs > 1){
			offset += snprintf(NULL, 0, "]");
		}
		//offset += snprintf(NULL, 0, "\n");
	}else{
		if(numargs){
			if(numargs > 1){
				offset += snprintf(buf + offset, n - offset, "%s%s : [", tabs, osc_message_s_getAddress(m));
			}else{
				offset += snprintf(buf + offset, n - offset, "%s%s : ", tabs, osc_message_s_getAddress(m));
			}
		}else{
			offset += snprintf(buf + offset, n - offset, "%s%s", tabs, osc_message_s_getAddress(m));
		}
		while(osc_msg_it_s_hasNext(it)){
			//offset += snprintf(NULL, 0, " ");
			t_osc_atom_s *a = osc_msg_it_s_next(it);
			offset += osc_atom_s_nformat(buf + offset, n - offset, a, nindent);
			if(osc_msg_it_s_hasNext(it)){
				//if(osc_atom_s_getTypetag(a) == OSC_BUNDLE_TYPETAG){
				//offset += snprintf(buf + offset, n - offset, ",\n");
				//}else{
					offset += snprintf(buf + offset, n - offset, ", ");
					//}
			}
		}		
		if(numargs > 1){
			offset += snprintf(buf + offset, n - offset, "]");
		}
		//offset += snprintf(buf + offset, n - offset, "\n");
	}
	osc_msg_it_s_destroy(it);
	return offset;
}

t_osc_message_array_s *osc_message_array_s_alloc(long len){
	return (t_osc_message_array_s *)osc_array_allocWithSize(len, sizeof(t_osc_msg_s));
}

void osc_message_array_s_free(t_osc_message_array_s *ar)
{
	osc_array_free((t_osc_array *)ar);
}

void osc_message_array_s_clear(t_osc_message_array_s *ar)
{
	osc_array_clear((t_osc_array *)ar);
}

t_osc_msg_s *osc_message_array_s_get(t_osc_message_array_s *ar, long idx)
{
	return (t_osc_msg_s *)osc_array_get((t_osc_array *)ar, idx);
}

long osc_message_array_s_getLen(t_osc_message_array_s *ar)
{
	return osc_array_getLen((t_osc_array *)ar);
}

t_osc_message_array_s *osc_message_array_s_copy(t_osc_message_array_s *ar)
{
	return (t_osc_message_array_s *)osc_array_copy((t_osc_array *)ar);
}

void osc_message_array_s_resize(t_osc_message_array_s *ar, long newlen)
{
	osc_array_resize((t_osc_array *)ar, newlen);
}
