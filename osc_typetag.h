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

#ifndef __OSC_TYPETAG_H__
#define __OSC_TYPETAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"

typedef struct _osc_typetag_type t_osc_typetag_type;
char osc_typetag_findLUB(char t1, char t2);
char osc_typetag_isSubtype(char t1, char t2);
char *osc_typetag_name(char typetag);

//////////////////////////////////////////////////
// old
//////////////////////////////////////////////////

enum{
	OSC_TYPES_NULL = 0,
	OSC_TYPES_FALSE,
	OSC_TYPES_TRUE,
	OSC_TYPES_INT8,
	OSC_TYPES_UINT8,
	OSC_TYPES_INT16,
	OSC_TYPES_UINT16,
	OSC_TYPES_INT32,
	OSC_TYPES_UINT32,
	OSC_TYPES_INT64,
	OSC_TYPES_UINT64,
	OSC_TYPES_FLOAT32,
	OSC_TYPES_FLOAT64,
	OSC_TYPES_TIMETAG,
	OSC_TYPES_STRING
};

#define OSC_TYPETAG_INSTANCE_NULL 0x1llu
#define OSC_TYPETAG_INSTANCE_TRUE 0x2llu
#define OSC_TYPETAG_INSTANCE_FALSE 0x4llu
#define OSC_TYPETAG_INSTANCE_INT8 0x8llu
#define OSC_TYPETAG_INSTANCE_UINT8 0x10llu
#define OSC_TYPETAG_INSTANCE_INT16 0x20llu
#define OSC_TYPETAG_INSTANCE_UINT16 0x40llu
#define OSC_TYPETAG_INSTANCE_INT32 0x80llu
#define OSC_TYPETAG_INSTANCE_UINT32 0x100llu
#define OSC_TYPETAG_INSTANCE_INT64 0x200llu
#define OSC_TYPETAG_INSTANCE_UINT64 0x400llu
#define OSC_TYPETAG_INSTANCE_FLOAT 0x800llu
#define OSC_TYPETAG_INSTANCE_DOUBLE 0x1000llu
#define OSC_TYPETAG_INSTANCE_TIMETAG 0x2000llu
#define OSC_TYPETAG_INSTANCE_BUNDLE 0x4000llu
#define OSC_TYPETAG_INSTANCE_EXPR 0x8000llu
#define OSC_TYPETAG_INSTANCE_STRING 0x10000llu

#define OSC_TYPETAG_CLASS_BOOL OSC_TYPETAG_INSTANCE_TRUE | OSC_TYPETAG_INSTANCE_FALSE
#define OSC_TYPETAG_CLASS_INTEGER OSC_TYPETAG_INSTANCE_INT8 | OSC_TYPETAG_INSTANCE_UINT8 | OSC_TYPETAG_INSTANCE_INT16 | OSC_TYPETAG_INSTANCE_UINT16 | OSC_TYPETAG_INSTANCE_INT32 | OSC_TYPETAG_INSTANCE_UINT32 | OSC_TYPETAG_INSTANCE_INT64 | OSC_TYPETAG_INSTANCE_UINT64
#define OSC_TYPETAG_CLASS_FLOAT OSC_TYPETAG_INSTANCE_FLOAT | OSC_TYPETAG_INSTANCE_DOUBLE
#define OSC_TYPETAG_CLASS_NUMBER OSC_TYPETAG_CLASS_INTEGER | OSC_TYPETAG_CLASS_FLOAT

#define OSC_TYPETAG_ISFLOAT(tt) (tt == 'f' || tt == 'd')
#define OSC_TYPETAG_ISINT(tt) (tt == 'i' || tt == 'I' || tt == 'h' || tt == 'H' || tt == 'u' || tt == 'U' || tt == 'c' || tt == 'C')
#define OSC_TYPETAG_ISBOOL(tt) (tt == 'T' || tt == 'F')
#define OSC_TYPETAG_ISSTRING(tt) (tt == 's')
#define OSC_TYPETAG_ISNUMERIC(tt) (tt == 'f' || tt == 'd' || tt == 'T' || tt == 'F' ||tt == 'i' || tt == 'I' || tt == 'h' || tt == 'H' || tt == 'u' || tt == 'U' || tt == 'c' || tt == 'C')

char *osc_typetag_str(int8_t tt);
char osc_typetag_getLargestType(int argc, t_osc_atom_u **argv);

#ifdef __cplusplus
}
#endif

#endif
