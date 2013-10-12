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

/** 	\file osc_expr_funcdecls.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_FUNCDECLS_H__
#define __OSC_EXPR_FUNCDECLS_H__

#ifdef __cplusplus
extern "C" {
#endif

t_osc_expr_rec *osc_expr_funcdecls_lookupFunction(char *name);
void osc_expr_funcdecls_makeFunctionBundle(void);
void osc_expr_funcdecls_makeCategoryBundle_cb(char *key, void *val, void *bundle);
void osc_expr_funcdecls_makeCategoryBundle(void);
t_osc_bndl_s *osc_expr_funcdecls_getCategories(void);
t_osc_err osc_expr_funcdecls_getFunctionsForCategory(char *cat, long *len, char **ptr);
t_osc_err osc_expr_funcdecls_getDocForFunction(char *function_name, t_osc_bndl_u **bndl);
t_osc_err osc_expr_funcdecls_formatFunctionDoc(t_osc_expr_rec *rec, t_osc_bndl_u **bndl);
void osc_expr_funcdecls_formatTypes(int types, char *str);

#ifdef __cplusplus
}
#endif

#endif
