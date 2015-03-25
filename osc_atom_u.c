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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h> // for floor for formatting float
#include "osc.h"
#include "osc_mem.h"
#include "osc_util.h"
#include "osc_byteorder.h"
#include "osc_bundle_s.h"
#include "osc_atom_u.h"
#include "osc_atom_u.r"
#include "osc_strfmt.h"

t_osc_atom_u *osc_atom_u_alloc(void)
{
	t_osc_atom_u *a = osc_mem_alloc(sizeof(t_osc_atom_u));
	memset(a, '\0', sizeof(t_osc_atom_u));
	return a;
}

size_t osc_atom_u_getStructSize(void)
{
	return sizeof(t_osc_atom_u);
}

void osc_atom_u_free(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	if(a->typetag == 's'){
		if(a->w.s){
			osc_mem_free(a->w.s);
		}
	}else if(a->typetag == OSC_BUNDLE_TYPETAG){
		//if(a->alloc){ // should always be true
		//osc_mem_free(osc_bundle_s_getPtr(a->w.bndl));
			osc_bundle_u_free(a->w.bndl);
			//}
	}
	osc_mem_free(a);
}

void osc_atom_u_copyInto(t_osc_atom_u **dest, t_osc_atom_u *src)
{
	if(!src){
		return;
	}
	t_osc_atom_u *aa = *dest;
	if(!aa){
		aa = osc_mem_alloc(sizeof(t_osc_atom_u));
	}
	aa->next = aa->prev = NULL;
	aa->typetag = src->typetag;
	aa->alloc = src->alloc;
	switch(aa->typetag){
	case 's':
		{
			aa->w.s = osc_mem_alloc(strlen(src->w.s) + 1);
			strcpy(aa->w.s, src->w.s);
		}
		break;
	case OSC_BUNDLE_TYPETAG:
		{
			aa->w.bndl = NULL;
			osc_bundle_u_copy(&(aa->w.bndl), src->w.bndl);
		}
		break;
	default:
		aa->w = src->w;
	}
	*dest = aa;
}

t_osc_atom_u *osc_atom_u_copy(t_osc_atom_u *src)
{
	t_osc_atom_u *dest = NULL;
	osc_atom_u_copyInto(&dest, src);
	return dest;
}

void osc_atom_u_setShouldFreePtr(t_osc_atom_u *a, int bool)
{
	if(a){
		a->alloc = bool;
	}
}

void osc_atom_u_clear(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	if(a->alloc && a->typetag == 's' && a->w.s){
		osc_mem_free(a->w.s);
		a->w.s = NULL;
	}else if(a->typetag == OSC_BUNDLE_TYPETAG){
		osc_bundle_u_free(a->w.bndl);
		a->w.bndl = NULL;
	}
	a->alloc = 0;
}

void osc_atom_u_append(t_osc_atom_u *a1, t_osc_atom_u *a2)
{
	if(!a1){
		return;
	}
	while(a1->next){
		a1 = a1->next;
	}
	a1->next = a2;
}

char osc_atom_u_getTypetag(t_osc_atom_u *a)
{
	if(!a){
		return '\0';
	}
	return a->typetag;
}

float osc_atom_u_getFloat(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'f': // 32-bit IEEE 754 float
		return a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (float)a->w.d;
	case 'i': // signed 32-bit int
		return (float)a->w.i;
	case 'h': // signed 64-bit int
		return (float)a->w.h;
	case 'I': // unsigned 32-bit int
		return (float)a->w.I;
	case 's': // string
		{
			char *endp = NULL;
			return strtof(a->w.s, &endp);
		}
	case 'T': // true
		return 1.f;
	case 'F': // false
		return 0.f;
	case 'H': // unsigned 64-bit int
		return (float)a->w.H;
	case 'c': // signed 8-bit int
		return (float)a->w.c;
	case 'C': // unsigned 8-bit int
		return (float)a->w.C;
	case 'u': // signed 16-bit int
		return (float)a->w.u;
	case 'U': // unsigned 16-bit int
		return (float)a->w.U;
	case 'N': // NULL
		return 0.f;
	case OSC_TIMETAG_TYPETAG:
		return (float)osc_timetag_timetagToFloat(a->w.t);
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				int32_t i = ntoh32(*((int32_t *)(blob + 4)));
				return *((float *)&i);
			}
		}
	}
	return 0.f;
}

double osc_atom_u_getDouble(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'd': // 64-bit IEEE 754 double
		return a->w.d;
	case 'f': // 32-bit IEEE 754 float
		return (double)a->w.f;
	case 'i': // signed 32-bit int
		return (double)a->w.i;
	case 'h': // signed 64-bit int
		return (double)a->w.h;
	case 'I': // unsigned 32-bit int
		return (double)a->w.I;
	case 's': // string
		{
			char *endp = NULL;
			return strtod(a->w.s, &endp);
		}
	case 'T': // true
		return 1.f;
	case 'F': // false
		return 0.f;
	case 'H': // unsigned 64-bit int
		return (double)a->w.H;
	case 'c': // signed 8-bit int
		return (double)a->w.c;
	case 'C': // unsigned 8-bit int
		return (double)a->w.C;
	case 'u': // signed 16-bit int
		return (double)a->w.u;
	case 'U': // unsigned 16-bit int
		return (double)a->w.U;
	case 'N': // NULL
		return 0.;
	case OSC_TIMETAG_TYPETAG:
		return osc_timetag_timetagToFloat(a->w.t);
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 8){
				int64_t i = ntoh64(*((int64_t *)(blob + 4)));
				return *((double *)&i);
			}
		}
	}
	return 0.;
}

int8_t osc_atom_u_getInt8(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'c': // signed 8-bit int
		return a->w.c;
	case 'C': // unsigned 8-bit int
		return (int8_t)a->w.C;
	case 'i': // signed 32-bit int
		return (int8_t)a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (int8_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (int8_t)a->w.d;
	case 's': // string
		{
			char *s = a->w.s;
			if(s){
				return *s;
			}
		}
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'u': // signed 16-bit int
		return (int8_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (int8_t)a->w.U;
	case 'h': // signed 64-bit int
		return (int8_t)a->w.h;
	case 'I': // unsigned 32-bit int
		return (int8_t)a->w.I;
	case 'H': // unsigned 64-bit int
		return (int8_t)a->w.H;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return blob[4];
			}
		}
	}
	return 0;
}

int16_t osc_atom_u_getInt16(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'u': // signed 16-bit int
		return (int16_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (int16_t)a->w.U;
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (int16_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (int16_t)a->w.d;
	case 's': // string
		{
			char *endp = NULL;
			return (int16_t)strtol(a->w.s, &endp, 0);
		}
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'c': // signed 8-bit int
		return (int16_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (int16_t)a->w.C;
	case 'h': // signed 64-bit int
		return (int16_t)a->w.h;
	case 'I': // unsigned 32-bit int
		return (int16_t)a->w.I;
	case 'H': // unsigned 64-bit int
		return (int16_t)a->w.H;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return ntoh16(*((int16_t *)(blob + 4)));
			}
		}
	}
	return 0;
}



int32_t osc_atom_u_getInt32(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (int32_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (int32_t)a->w.d;
	case 's': // string
		{
			char *endp = NULL;
			return (int32_t)strtol(a->w.s, &endp, 0);
		}
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'c': // signed 8-bit int
		return (int32_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (int32_t)a->w.C;
	case 'u': // signed 16-bit int
		return (int32_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (int32_t)a->w.U;
	case 'h': // signed 64-bit int
		return (int32_t)a->w.h;
	case 'I': // unsigned 32-bit int
		return (int32_t)a->w.I;
	case 'H': // unsigned 64-bit int
		return (int32_t)a->w.H;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return ntoh32(*((int32_t *)(blob + 4)));
			}
		}
	}
	return 0;
}

int64_t osc_atom_u_getInt64(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'h': // signed 64-bit int
		return (int64_t)a->w.h;
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'd': // 64-bit IEEE 754 double
		return (int64_t)a->w.d;
	case 'f': // 32-bit IEEE 754 float
		return (int64_t)a->w.f;
	case 'I': // unsigned 32-bit int
		return (int64_t)a->w.I;
	case 'H': // unsigned 64-bit int
		return (int64_t)a->w.H;
	case 's': // string
		{
			char *endp = NULL;
			return (int64_t)strtoll(a->w.s, &endp, 0);
		}
	case 'c': // signed 8-bit int
		return (int64_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (int64_t)a->w.C;
	case 'u': // signed 16-bit int
		return (int64_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (int64_t)a->w.U;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 8){
				return ntoh64(*((int64_t *)(blob + 4)));
			}
		}
	}
	return 0;
}

uint8_t osc_atom_u_getUInt8(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'C': // signed 8-bit int
		return (uint8_t)a->w.C;
	case 'c': // unsigned 8-bit int
		return (uint8_t)a->w.c;
	case 'I': // unsigned 32-bit int
		return (uint8_t)a->w.I;
	case 'i': // signed 32-bit int
		return (uint8_t)a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (uint8_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (uint8_t)a->w.d;
	case 'h': // signed 64-bit int
		return (uint8_t)a->w.h;
	case 'H': // unsigned 64-bit int
		return (uint8_t)a->w.H;
	case 's': // string
		{
			char *s = a->w.s;
			if(s){
				return *s;
			}
		}
	case 'u': // signed 16-bit int
		return (uint8_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (uint8_t)a->w.U;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return (uint8_t)blob[4];
			}
		}
	}
	return 0;
}

uint16_t osc_atom_u_getUInt16(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'U': // signed 16-bit int
		return a->w.U;
	case 'u': // unsigned 16-bit int
		return (uint16_t)a->w.u;
	case 'I': // unsigned 32-bit int
		return (uint16_t)a->w.I;
	case 'i': // signed 32-bit int
		return (uint16_t)a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (uint16_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (uint16_t)a->w.d;
	case 'h': // signed 64-bit int
		return (uint16_t)a->w.h;
	case 'H': // unsigned 64-bit int
		return (uint16_t)a->w.H;
	case 's': // string
		{
			char *endp = NULL;
			return (uint16_t)strtoul(a->w.s, &endp, 0);
		}
	case 'c': // signed 8-bit int
		return (uint16_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (uint16_t)a->w.C;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return ntoh16(*((uint16_t *)(blob + 4)));
			}
		}
	}
	return 0;
}

uint32_t osc_atom_u_getUInt32(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'I': // unsigned 32-bit int
		return (uint32_t)a->w.I;
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (uint32_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (uint32_t)a->w.d;
	case 'h': // signed 64-bit int
		return (uint32_t)a->w.h;
	case 'H': // unsigned 64-bit int
		return (uint32_t)a->w.H;
	case 's': // string
		{
			char *endp = NULL;
			return (uint32_t)strtoul(a->w.s, &endp, 0);
		}
	case 'c': // signed 8-bit int
		return (uint32_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (uint32_t)a->w.C;
	case 'u': // signed 16-bit int
		return (uint32_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (uint32_t)a->w.U;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 4){
				return ntoh32(*((uint32_t *)(blob + 4)));
			}
		}
	}
	return 0;
}

uint64_t osc_atom_u_getUInt64(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'H': // unsigned 64-bit int
		return (uint64_t)a->w.H;
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'h': // signed 64-bit int
		return (uint64_t)a->w.h;
	case 'I': // unsigned 32-bit int
		return (uint64_t)a->w.I;
	case 'f': // 32-bit IEEE 754 float
		return (uint64_t)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (uint64_t)a->w.d;
	case 's': // string
		{
			char *endp = NULL;
			return (uint64_t)strtoull(a->w.s, &endp, 0);
		}
	case 'c': // signed 8-bit int
		return (uint64_t)a->w.c;
	case 'C': // unsigned 8-bit int
		return (uint64_t)a->w.C;
	case 'u': // signed 16-bit int
		return (uint64_t)a->w.u;
	case 'U': // unsigned 16-bit int
		return (uint64_t)a->w.U;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == 8){
				return ntoh32(*((uint32_t *)(blob + 4)));
			}
		}
	}
	return 0;
}

int osc_atom_u_getInt(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'i': // signed 32-bit int
		return a->w.i;
	case 'f': // 32-bit IEEE 754 float
		return (int)a->w.f;
	case 'd': // 64-bit IEEE 754 double
		return (int)a->w.d;
	case 'h': // signed 64-bit int
		return (int)a->w.h;
	case 'I': // unsigned 32-bit int
		return (int)a->w.I;
	case 'H': // unsigned 64-bit int
		return (int)a->w.H;
	case 's': // string
		{
			char *endp = NULL;
			return (int)strtol(a->w.s, &endp, 0);
		}
	case 'c': // signed 8-bit int
		return (int)a->w.c;
	case 'C': // unsigned 8-bit int
		return (int)a->w.C;
	case 'u': // signed 16-bit int
		return (int)a->w.u;
	case 'U': // unsigned 16-bit int
		return (int)a->w.U;
	case 'T': // true
		return 1;
	case 'F': // false
		return 0;
	case 'N': // NULL
		return 0;
	case 'b': // blob
		{
			char *blob = a->w.b;
			int32_t len = ntoh32(*((int32_t *)blob));
			if(len == sizeof(int)){
				if(sizeof(int) == 4){
					return ntoh32(*((uint32_t *)(blob + 4)));
				}else if(sizeof(int) == 8){
					return ntoh64(*((uint64_t *)(blob + 4)));
				}
			}
		}
	}
	return 0;
}

char *osc_atom_u_getStringPtr(t_osc_atom_u *a)
{
	if(!a){
		return NULL;
	}
	if(a->typetag == 's'){
		return a->w.s;
	}else{
		return NULL;
	}
}

int osc_atom_u_getStringLen(t_osc_atom_u *a)
{
	if(!a){
		return 0;
	}
	switch(a->typetag){
	case 's':
		return strlen(a->w.s);
	case 'i': // signed 32-bit int
		return osc_strfmt_int32(NULL, 0, a->w.i);
	case 'f': // 32-bit IEEE 754 float
		return osc_strfmt_float32(NULL, 0, a->w.f);
	case 'd': // 64-bit IEEE 754 double
		return osc_strfmt_float64(NULL, 0, a->w.d);
	case 'h': // signed 64-bit int
		return osc_strfmt_int64(NULL, 0, a->w.h);
	case 'I': // unsigned 32-bit int
		return osc_strfmt_uint32(NULL, 0, a->w.I);
	case 'H': // unsigned 64-bit int
		return osc_strfmt_uint64(NULL, 0, a->w.H);
	case 'c': // signed 8-bit int
		return osc_strfmt_int8(NULL, 0, a->w.c);
	case 'C': // unsigned 8-bit int
		return osc_strfmt_uint32(NULL, 0, a->w.C);
	case 'u': // signed 16-bit int
		return osc_strfmt_int16(NULL, 0, a->w.u);
	case 'U': // unsigned 16-bit int
		return osc_strfmt_uint16(NULL, 0, a->w.U);
	case 'T': // true
		return osc_strfmt_bool(NULL, 0, 'T');
	case 'F': // false
		return osc_strfmt_bool(NULL, 0, 'F');
	case 'N': // NULL
		return osc_strfmt_null(NULL, 0);
	case 't': // timetag
		return osc_strfmt_timetag(NULL, 0, a->w.t);
	case 'b': // blob
		return osc_strfmt_blob(NULL, 0, a->w.b);
	}
	return 0;
}

int osc_atom_u_getString(t_osc_atom_u *a, size_t n, char **out)
{
	if(!a){
		return 0;
	}
	int stringlen = osc_atom_u_getStringLen(a);
	int nn = n;
	if(!(*out)){
		nn = stringlen + 1;
		*out = osc_mem_alloc(nn);
	}
	if(!(*out)){
		return 0;
	}
	switch(a->typetag){
	case 's': // string
		strncpy(*out, a->w.s, nn);
		break;
	case 'i': // signed 32-bit int
		osc_strfmt_int32(*out, nn, a->w.i);
		break;
	case 'f': // 32-bit IEEE 754 float
		osc_strfmt_float32(*out, nn, a->w.f);
		break;
	case 'd': // 64-bit IEEE 754 double
		osc_strfmt_float64(*out, nn, a->w.d);
		break;
	case 'h': // signed 64-bit int
		osc_strfmt_int64(*out, nn, a->w.h);
		break;
	case 'I': // unsigned 32-bit int
		osc_strfmt_uint32(*out, nn, a->w.I);
		break;
	case 'H': // unsigned 64-bit int
		osc_strfmt_uint64(*out, nn, a->w.H);
		break;
	case 'c': // signed 8-bit char
		osc_strfmt_int8(*out, nn, a->w.c);
		break;
	case 'C': // unsigned 8-bit char
		osc_strfmt_uint8(*out, nn, a->w.C);
		break;
	case 'u': // unsigned 32-bit int
		osc_strfmt_int16(*out, nn, a->w.u);
		break;
	case 'U': // unsigned 64-bit int
		osc_strfmt_uint16(*out, nn, a->w.U);
		break;
	case 'T': // true
		osc_strfmt_bool(*out, nn, 'T');
		break;
	case 'F': // false
		osc_strfmt_bool(*out, nn, 'F');
		break;
	case 'N': // NULL
		osc_strfmt_null(*out, nn);
		break;
	case 't': // timetag
		stringlen = osc_strfmt_timetag(*out, nn, a->w.t);
		break;
	case 'b': // blob
		stringlen = osc_strfmt_blob(*out, nn, a->w.b);
		break;
	}
	return stringlen;
}

int osc_atom_u_getBool(t_osc_atom_u *a){
	if(!a){
		return '\0';
	}
	switch(a->typetag){
	case 'T':
		return 1;
	case 'F':
	case 'N':
		return 0;
	case 'i':
		return !(a->w.i == 0);
	case 'h':
		return !(a->w.h == 0);
	case 'I':
		return !(a->w.I == 0);
	case 'H':
		return !(a->w.H == 0);
	case 'f':
		return !(a->w.f == 0);
	case 'd':
		return !(a->w.d == 0);
	case 's':
		return 0;
	case 'c':
		return !(a->w.c == 0);
	case 'C':
		return !(a->w.C == 0);
	case 'u':
		return !(a->w.u == 0);
	case 'U':
		return !(a->w.U == 0);
	case OSC_BUNDLE_TYPETAG:
		// uh...
		return 0;
	}
	return 0;
}

t_osc_bndl_u *osc_atom_u_getBndl(t_osc_atom_u *a)
{
	if(!a){
		return NULL;
	}
	if(a->typetag == OSC_BUNDLE_TYPETAG){
		return a->w.bndl;
	}else{
		return NULL;
	}
}

t_osc_timetag osc_atom_u_getTimetag(t_osc_atom_u *a)
{
	if(!a){
		return OSC_TIMETAG_NULL;
	}
	if(a->typetag == OSC_TIMETAG_TYPETAG){
		return a->w.t;
	}else{
		return OSC_TIMETAG_NULL;
	}
}

int32_t osc_atom_u_getBlobLen(t_osc_atom_u *a)
{
	if(!a){
		return -1;
	}
	switch(osc_atom_u_getTypetag(a)){
	case 'b':
		if(a->w.b){
			return ntoh32(*((int32_t *)a->w.b));
		}else{
			return 0;
		}
	case OSC_BUNDLE_TYPETAG:
		{
			t_osc_bndl_s *bs = osc_bundle_u_serialize(a->w.bndl);
			long len = osc_bundle_s_getLen(bs);
			osc_bundle_s_deepFree(bs);
			return len;
		}
	case 's':
		return strlen(a->w.s);
	default:
		return osc_sizeof(osc_atom_u_getTypetag(a), a->w.s);
	}
}

char *osc_atom_u_getBlob(t_osc_atom_u *a)
{
	if(!a){
		return NULL;
	}
	if(osc_atom_u_getTypetag(a) != 'b'){
		return NULL;
	}
	return a->w.b;
}

void osc_atom_u_getBlobCopy(t_osc_atom_u *a, int32_t *buflen, char **blob)
{
	if(!a){
		return;
	}
	int bloblen = osc_atom_u_getBlobLen(a);
	if(!(*blob)){
		*buflen = bloblen + 1;
		while(*buflen % 4){
			*buflen += 1;
		}
		*buflen += 4;
		*blob = osc_mem_alloc(*buflen);
	}
	memset(*blob, '\0', *buflen);
	switch(osc_atom_u_getTypetag(a)){
	case 'b':
		memcpy(*blob, a->w.b, *buflen);
		break;
	case 'c':
	case 'C':
		*((int32_t *)(*blob)) = hton32(1);
		(*blob)[4] = a->w.c;
		break;
	case 'u':
	case 'U':
		*((int32_t *)(*blob)) = hton32(2);
		*((int16_t *)((*blob) + 4)) = hton16(a->w.u);
		break;
	case 'i':
	case 'I':
		*((int32_t *)(*blob)) = hton32(4);
		*((int32_t *)((*blob) + 4)) = hton32(a->w.i);
		break;
	case 'h':
	case 'H':
		{
			int64_t i = a->w.i;
			*((int32_t *)(*blob)) = hton32(8);
			*((int64_t *)((*blob) + 4)) = hton64(i);
		}
		break;		
	case 'f':
		{
			float f = a->w.f;
			*((int32_t *)(*blob)) = hton32(4);
			*((int32_t *)((*blob) + 4)) = hton32(*((int32_t *)&f));
		}
		break;
	case 'd':
		{
			double f = a->w.d;
			*((int32_t *)(*blob)) = hton32(8);
			*((int64_t *)((*blob) + 4)) = hton64(*((int64_t *)&f));
		}
		break;
	case 's':
		{
			*((int32_t *)(*blob)) = hton32(*buflen - 4);
			char *p = (*blob) + 4;
			osc_atom_u_getString(a, *buflen, &p);
		}
		break;
	case OSC_BUNDLE_TYPETAG:
		{
			t_osc_bndl_s *bs = osc_bundle_u_serialize(a->w.bndl);
			if(bs){
				long l = osc_bundle_s_getLen(bs);
				char *bndl = osc_bundle_s_getPtr(bs);
				*((int32_t *)(*blob)) = hton32(l);
				memcpy((*blob) + 4, bndl, l);
				osc_bundle_s_deepFree(bs);
			}
		}
		break;
	case OSC_TIMETAG_TYPETAG:
		*((int32_t *)(*blob)) = hton32(OSC_TIMETAG_SIZEOF);
		osc_timetag_encodeForHeader(a->w.t, (*blob) + 4);
		break;
	default: ;
	}
}

void osc_atom_u_setFloat(t_osc_atom_u *a, float v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.f = v;
	a->typetag = 'f';
}

void osc_atom_u_setDouble(t_osc_atom_u *a, double v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.d = v;
	a->typetag = 'd';
}

void osc_atom_u_setInt8(t_osc_atom_u *a, int8_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.c = v;
	a->typetag = 'c';
}

void osc_atom_u_setInt16(t_osc_atom_u *a, int16_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.u = v;
	a->typetag = 'u';
}

void osc_atom_u_setInt32(t_osc_atom_u *a, int32_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.i = v;
	a->typetag = 'i';
}

void osc_atom_u_setInt64(t_osc_atom_u *a, int64_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.h = v;
	a->typetag = 'h';
}

void osc_atom_u_setUInt8(t_osc_atom_u *a, uint8_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.C = v;
	a->typetag = 'C';
}

void osc_atom_u_setUInt16(t_osc_atom_u *a, uint16_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.U = v;
	a->typetag = 'U';
}

void osc_atom_u_setUInt32(t_osc_atom_u *a, uint32_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.I = v;
	a->typetag = 'I';
}

void osc_atom_u_setUInt64(t_osc_atom_u *a, uint64_t v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.H = v;
	a->typetag = 'H';
}

void osc_atom_u_setString(t_osc_atom_u *a, const char *v)
{
	if(!a){
		return;
	}
	if(a->w.s && a->typetag == 's' && a->alloc){
		if(v){
			long len = strlen(v) + 1;
			a->w.s = osc_mem_resize(a->w.s, len);
			strncpy(a->w.s, v, len);
		}else{
			a->w.s = NULL;
		}
	}else{
		if(v){
			a->w.s = osc_mem_alloc(strlen(v) + 1);
			strcpy(a->w.s, v);
			a->typetag = 's';
			a->alloc = 1;
		}else{
			a->w.s = NULL;
		}
	}
}

void osc_atom_u_setStringPtr(t_osc_atom_u *a, char *v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.s = v;
	a->typetag = 's';
}

void osc_atom_u_setBool(t_osc_atom_u *a, int v)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	if(v == 0){
		osc_atom_u_setFalse(a);
	}else{
		osc_atom_u_setTrue(a);
	}
}

void osc_atom_u_setTrue(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->typetag = 'T';
}

void osc_atom_u_setFalse(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->typetag = 'F';
}

void osc_atom_u_setNil(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->typetag = 'N';
}

void osc_atom_u_setBndl(t_osc_atom_u *a, long len, char *ptr)
{
	osc_atom_u_setBndl_s(a, len, ptr);
}

void osc_atom_u_setBndl_s(t_osc_atom_u *a, long len, char *ptr)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	t_osc_bndl_u *b = osc_bundle_s_deserialize(len, ptr);
	osc_atom_u_setBndl_u(a, b);
/*
	char *copy = osc_mem_alloc(len);
	memcpy(copy, ptr, len);
	t_osc_bndl_s *bndl = osc_bundle_s_alloc(len, copy);
	a->w.bndl = bndl;
	a->typetag = OSC_BUNDLE_TYPETAG;
	a->alloc = 1;
*/
}

void osc_atom_u_setBndl_u(t_osc_atom_u *a, t_osc_bndl_u *b)
{
	a->w.bndl = b;
	a->typetag = OSC_BUNDLE_TYPETAG;
}

void osc_atom_u_setTimetag(t_osc_atom_u *a, t_osc_timetag timetag)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.t = timetag;
	a->typetag = OSC_TIMETAG_TYPETAG;
}

void osc_atom_u_setBlob(t_osc_atom_u *a, char *blob)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	int32_t l = ntoh32(*((int32_t *)blob));
	int buflen = l + 1;
	while(buflen % 4){
		buflen++;
	}
	char *copy = osc_mem_alloc(buflen + 4);
	memset(copy, '\0', buflen + 4);
	memcpy(copy, blob, l + 4);
	a->w.b = copy;
	a->typetag = 'b';
}

void osc_atom_u_setBlobPtr(t_osc_atom_u *a, char *blob)
{
	if(!a){
		return;
	}
	osc_atom_u_clear(a);
	a->w.b = blob;
	a->typetag = 'b';
}

void osc_atom_u_negate(t_osc_atom_u *a)
{
	if(!a){
		return;
	}
	switch(osc_atom_u_getTypetag(a)){
	case 'c':
		osc_atom_u_setInt8(a, -osc_atom_u_getInt8(a));
		break;
	case 'u':
		osc_atom_u_setInt16(a, -osc_atom_u_getInt16(a));
		break;
	case 'i':
		osc_atom_u_setInt32(a, -osc_atom_u_getInt32(a));
		break;
	case 'h':
		osc_atom_u_setInt64(a, -osc_atom_u_getInt64(a));
		break;
	case 'C':
		osc_atom_u_setInt8(a, -osc_atom_u_getUInt8(a));
		break;
	case 'U':
		osc_atom_u_setInt16(a, -osc_atom_u_getUInt16(a));
		break;
	case 'I':
		osc_atom_u_setInt32(a, -osc_atom_u_getUInt32(a));
		break;
	case 'H':
		osc_atom_u_setInt64(a, -osc_atom_u_getUInt64(a));
		break;
	case 'f':
		osc_atom_u_setFloat(a, -osc_atom_u_getFloat(a));
		break;
	case 'd':
		osc_atom_u_setDouble(a, -osc_atom_u_getDouble(a));
		break;
	}
}

size_t osc_atom_u_sizeof(t_osc_atom_u *a)
{
	if(!a){
		return 0;
	}
	switch(a->typetag){
	case 'c':
	case 'C':
	case 'u':
	case 'U':
	case 'i':
	case 'I':
	case 'f':
		return 4;
	case 'h':
	case 'H':
	case 'd':
		return 8;
	case 's':
		return strlen(a->w.s);
	case 'T':
	case 'F':
	case 'N':
		return 0;
	case OSC_BUNDLE_TYPETAG:
		return osc_bundle_u_getSerializedSize(a->w.bndl);
	case OSC_TIMETAG_TYPETAG:
		return OSC_TIMETAG_SIZEOF;
	case 'b':
		return ntoh32(*((int32_t *)a->w.b));
	}
	return 0;
}

size_t osc_atom_u_nserialize(char *buf, size_t n, t_osc_atom_u *a)
{
	if(!a){
		return 0;
	}
	switch(osc_atom_u_getTypetag(a)){
	case 'c':
	case 'C':
	case 'u':
	case 'U':
	case 'i':
	case 'I':
		if(!buf){
			return 4;
		}else if(n >= 4){
			int32_t i = a->w.i;
			*((int32_t *)buf) = hton32(i);
			return 4;
		}
		break;
	case 'f':
		if(!buf){
			return 4;
		}else if(n >= 4){
			float f = a->w.f;
			*((int32_t *)buf) = hton32(*((int32_t *)(&f)));
			return 4;
		}
		break;
	case 'd':
		if(!buf){
			return 8;
		}else if(n >= 8){
			double d = a->w.d;
			*((int64_t *)buf) = hton64(*((int64_t *)(&d)));
			return 8;
		}
		break;
	case 's':
		{
			size_t len = strlen(a->w.s);
			size_t plen = osc_util_getPaddingForNBytes(len);
			if(!buf){
				return plen;
			}else if(n >= plen){
				memset(buf, '\0', plen);
				memcpy(buf, a->w.s, len);
				return plen;
			}
		}
		break;
	case OSC_BUNDLE_TYPETAG:
		{
			size_t len = osc_bundle_u_nserialize(NULL, 0, a->w.bndl);
			if(!buf){
				return len + 4;
			}else if(n >= len + 4){
				*((int32_t *)buf) = hton32(len);
				return osc_bundle_u_nserialize(buf + 4, len, a->w.bndl) + 4;
			}
		}
	case OSC_TIMETAG_TYPETAG:
		if(!buf){
			return OSC_TIMETAG_SIZEOF;
		}else if(n >= OSC_TIMETAG_SIZEOF){
			osc_timetag_encodeForHeader(a->w.t, buf);
			return OSC_TIMETAG_SIZEOF;
		}
		break;
	case 'h':
	case 'H':
		if(!buf){
			return 8;
		}else if(n >= 8){
			*((int64_t *)buf) = hton64(a->w.h);
			return 8;
		}
		break;
// nothing to do for T, F, or N
	case 'b':
		{
			int32_t len = ntoh32(*((int32_t *)a->w.b));
			size_t plen = osc_util_getBlobLength(len); // includes size
			if(!buf){
				return plen;
			}else if(n >= plen){
				memset(buf, '\0', plen);
				memcpy(buf, a->w.b, len + 4);
				return plen;
			}
		}
		break;
	default: return 0;
	}
	return 0;
}

long osc_atom_u_getSerializedSize(t_osc_atom_u *a)
{
	return osc_atom_u_nserialize(NULL, 0, a);
}

t_osc_atom_s *osc_atom_u_serialize(t_osc_atom_u *a)
{
	size_t n = osc_atom_u_nserialize(NULL, 0, a);
	char *buf = osc_mem_alloc(n);
	osc_atom_u_nserialize(buf, n, a);
	return osc_atom_s_alloc(osc_atom_u_getTypetag(a), buf);
}

long osc_atom_u_getFormattedSize(t_osc_atom_u *a)
{
	return osc_atom_u_nformat(NULL, 0, a, 0);
}

char *osc_atom_u_format(t_osc_atom_u *a)
{
	if(!a){
		return NULL;
	}
	long len = osc_atom_u_nformat(NULL, 0, a, 0) + 1;
	char *buf = osc_mem_alloc(len);
	osc_atom_u_nformat(buf, len, a, 0);
	return buf;
}

long osc_atom_u_nformat(char *buf, long n, t_osc_atom_u *a, int nindent)
{
	if(!a){
		return 0;
	}
	char tt = osc_atom_u_getTypetag(a);
	if(!buf){
		if(tt == OSC_BUNDLE_TYPETAG){
			return osc_bundle_u_nformatNestedBndl(NULL, 0, a->w.bndl, nindent + 1);
		}else if(tt == 's'){
			return osc_strfmt_quotedStringWithQuotedMeta(NULL, 0, osc_atom_u_getStringPtr(a));
		}else{
			return osc_atom_u_getStringLen(a);
		}
	}else{
		if(tt == OSC_BUNDLE_TYPETAG){
			return osc_bundle_u_nformatNestedBndl(buf, n, a->w.bndl, nindent + 1);
		}else if(tt == 's'){
			return osc_strfmt_quotedStringWithQuotedMeta(buf, n, osc_atom_u_getStringPtr(a));
		}else{
			return osc_atom_u_getString(a, n, &buf);
		}
	}
}

t_osc_atom_u *osc_atom_u_allocWithString(char *string)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setString(a, string);
	}
	return a;
}

t_osc_atom_u *osc_atom_u_allocWithDouble(double f)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setDouble(a, f);
	}
	return a;
}

t_osc_atom_u *osc_atom_u_allocWithFloat(float f)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setFloat(a, f);
	}
	return a;
}

t_osc_atom_u *osc_atom_u_allocWithInt32(int32_t i)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setInt32(a, i);
	}
	return a;
}

t_osc_atom_u *osc_atom_u_allocWithTimetag(t_osc_timetag t)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setTimetag(a, t);
	}
	return a;
}

t_osc_atom_u *osc_atom_u_allocWithBndl(t_osc_bndl_u *b)
{
	t_osc_atom_u *a = osc_atom_u_alloc();
	if(a){
		osc_atom_u_setBndl_u(a, b);
	}
	return a;
}
