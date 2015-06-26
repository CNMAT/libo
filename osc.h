/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009-11, The Regents of
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

#ifndef __OSC_H__
#define __OSC_H__

//#define OSC_2_0

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OSC_2_0
#define OSC_HEADER_SIZE 4
#define OSC_IDENTIFIER "#OSC"
#define OSC_IDENTIFIER_SIZE 4
#define OSC_EMPTY_HEADER "#OSC"

#else

#define OSC_HEADER_SIZE 16 // bundle\0 + 8 byte timetag.
#define OSC_IDENTIFIER "#bundle\0"
#define OSC_IDENTIFIER_SIZE 8
#define OSC_EMPTY_HEADER "#bundle\0\0\0\0\0\0\0\0\0"
#endif

#define OSC_ID OSC_IDENTIFIER
#define OSC_ID_SIZE OSC_IDENTIFIER_SIZE

#define OSC_TT_I8 'c'
#define OSC_TT_U8 'C'
#define OSC_TT_I16 'u'
#define OSC_TT_U16 'U'
#define OSC_TT_I32 'i'
#define OSC_TT_U32 'I'
#define OSC_TT_I64 'h'
#define OSC_TT_U64 'H'
#define OSC_TT_F32 'f'
#define OSC_TT_F64 'd'
#define OSC_TT_STR 's'
#define OSC_TT_SYM 'S'
#define OSC_TT_TIME 't'
#define OSC_TT_BLOB 'b'
// non-standard
#define OSC_TT_BNDL 'B'
#define OSC_TT_UNDEF 'r'
#define OSC_TT_EXPR 'A'
#define OSC_TT_NATIVE 'n'

#define OSC_VALUE_ADDRESS "/odot/value"


#define OSC_SUBBUNDLE_ACCESSOR_OPERATOR '.'
#define OSC_SUBBUNDLE_ACCESSOR_OPERATOR_STRING "."

#define OSC_ARRAY_CLEAR_ON_ALLOC 1

//#define OSC_INVALIDATE_PTR
#define OSC_VALIDATE_PTR

#define OSC_COMBINE1(x, y) x##y
#define OSC_COMBINE(x, y) OSC_COMBINE1(x, y)
#define OSC_UID(x) OSC_COMBINE(x, __LINE__)

#ifdef SWIG
#define OSC_DEPRECATED(decl, msg) decl;
#else
#define OSC_DEPRECATED(decl, msg) decl __attribute__((deprecated(msg)));
#endif

#ifdef __cplusplus
}
#endif

#endif // __OSC_H__
