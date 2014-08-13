/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011, The Regents of
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

/** 	\file osc_util.c
	\author John MacCallum
	\brief Random utilities used throughout the lib
*/

#include <string.h>
#include "osc_mem.h"

int osc_util_strdup(char **dest, char *src)
{
	int len = strlen(src) + 1;
	if(*dest){
		strncpy(*dest, src, len);
	}else{
		*dest = (char *)osc_mem_alloc(len);
		strncpy(*dest, src, len);
	}
	return len - 1;
}

size_t osc_util_getPaddedStringLen(char *s)
{
	if(!s){
		return 0;
	}
	size_t n = strlen(s);
	n = (n + 4) & 0xfffffffc;
	return n;
}

size_t osc_util_getPaddingForNBytes(size_t n)
{
	return (n + 4) & 0xfffffffc;
}

size_t osc_util_getBlobLength(size_t blob_data_size)
{
	size_t len = 4 + blob_data_size;
	if((len % 4) != 0){
		len += (4 - (len % 4));
	}
	return len;
}
