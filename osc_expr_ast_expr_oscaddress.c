/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-13, The Regents of
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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_expr_oscaddress.h"
#include "osc_expr_ast_expr_oscaddress.r"

t_osc_expr_ast_expr_oscaddress *osc_expr_ast_expr_oscaddress_alloc(char *address)
{
	t_osc_expr_ast_expr_oscaddress *v = osc_mem_alloc(sizeof(t_osc_expr_ast_expr_oscaddress));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v, OSC_EXPR_AST_EXPR_TYPE_OSCADDRESS, NULL, osc_expr_ast_expr_oscaddress_format, osc_expr_ast_expr_oscaddress_free, sizeof(t_osc_expr_ast_expr_oscaddress));
		osc_expr_ast_expr_oscaddress_setOSCAddress(v, address);
	}
	return v;
}

long osc_expr_ast_expr_oscaddress_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return snprintf(buf, n, "%s", osc_expr_ast_expr_oscaddress_getOSCAddress((t_osc_expr_ast_expr_oscaddress *)v));
	}
	return 0;
}

void osc_expr_ast_expr_oscaddress_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_mem_free(osc_expr_ast_expr_oscaddress_getOSCAddress((t_osc_expr_ast_expr_oscaddress *)v));
		osc_mem_free(v);
	}
}

char *osc_expr_ast_expr_oscaddress_getOSCAddress(t_osc_expr_ast_expr_oscaddress *v)
{
	if(v){
		return v->oscaddress;
	}
	return NULL;
}

char *osc_expr_ast_expr_oscaddress_getOSCAddressCopy(t_osc_expr_ast_expr_oscaddress *v)
{
	if(v){
		char *s = v->oscaddress;
		long len = strlen(s);
		char *copy = osc_mem_alloc(len);
		strncpy(copy, s, len);
		return copy;
	}
	return NULL;
}

void osc_expr_ast_expr_oscaddress_setOSCAddress(t_osc_expr_ast_expr_oscaddress *v, char *s)
{
	if(v){
		long len = strlen(s);
		char *copy = osc_mem_alloc(len);
		strncpy(copy, s, len);
		v->oscaddress = copy;
	}
}
