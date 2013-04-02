/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2013, The Regents of
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

/** 	\file osc_linkedlist.r
	\author John MacCallum

*/
#ifndef __OSC_LINKEDLIST_R__
#define __OSC_LINKEDLIST_R__

#ifdef __cplusplus
extern "C" {
#endif

struct _osc_linkedlist_elem{
	void *data;
	struct _osc_linkedlist_elem *next, *prev;
};

struct _osc_linkedlist{
	t_osc_linkedlist_elem *e;
	unsigned long count;
	t_osc_linkedlist_elem *head, *tail;
	void (*dtor)(void *data);
};

#ifdef __cplusplus
}
#endif

#endif // __OSC_LINKEDLIST_R__

