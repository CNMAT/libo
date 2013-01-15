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

#else

#define OSC_HEADER_SIZE 16 // bundle\0 + 8 byte timetag.
#define OSC_IDENTIFIER "#bundle\0"
#define OSC_IDENTIFIER_SIZE 8
#endif

#define OSC_ID OSC_IDENTIFIER
#define OSC_ID_SIZE OSC_IDENTIFIER_SIZE

#define OSC_BUNDLE_TYPETAG '.'

#define OSC_ARRAY_CLEAR_ON_ALLOC 1

#define OSC_QUOTE_STRINGS 1

// this is a workaround for a bug in Max.  the function that passes arguments to functions
// declared with static types (ie, not with A_GIMME) is not thread safe.  This has been fixed in
// max 6, but not in earlier versions.
#define OSC_GET_LEN_AND_PTR \
	if(argc != 2){\
		object_error((t_object *)x, "%s: expected 2 arguments but got %d", __func__, argc);\
		return;\
	}\
	if(atom_gettype(argv) != A_LONG){\
		object_error((t_object *)x, "%s: argument 1 should be an int", __func__);\
		return;\
	}\
	if(atom_gettype(argv + 1) != A_LONG){\
		object_error((t_object *)x, "%s: argument 2 should be an int", __func__);\
		return;\
	}\
	long len = atom_getlong(argv);\
	long ptr = atom_getlong(argv + 1);

#ifdef __cplusplus
}
#endif

#endif // __OSC_H__
