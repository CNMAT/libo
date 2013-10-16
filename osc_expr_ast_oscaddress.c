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
#include "osc_message_s.h"
#include "osc_message_iterator_s.h"
#include "osc_rset.h"
#include "osc_query.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_oscaddress.h"
#include "osc_expr_ast_oscaddress.r"

int osc_expr_ast_oscaddress_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out)
{
	if(!ast){
		return 1;
	}
	t_osc_expr_ast_oscaddress *a = (t_osc_expr_ast_oscaddress *)ast;
	char *address = osc_expr_ast_oscaddress_getAddress(a);
	*out = NULL;
	if(!oscbndl || !len){
		return OSC_ERR_EXPR_ADDRESSUNBOUND;
	}
	if(!(*oscbndl) || *len <= 16){
		return OSC_ERR_EXPR_ADDRESSUNBOUND;
	}
	t_osc_rset *rset = NULL;
	osc_query_select(1, &(address), *len, *oscbndl, 0, &rset);
	t_osc_rset_result *res = osc_rset_select(rset, address);
	if(rset){
		t_osc_bndl_s *complete_matches = osc_rset_result_getCompleteMatches(res);
		if(complete_matches){
			t_osc_msg_s *m = osc_bundle_s_getFirstMsg(complete_matches);
			if(m){
				long arg_count = osc_message_s_getArgCount(m);
				*out = osc_atom_array_u_alloc(arg_count);
						
				t_osc_atom_ar_u *atom_ar = *out;
				osc_atom_array_u_clear(atom_ar);
				int i = 0;
				t_osc_msg_it_s *it = osc_msg_it_s_get(m);
				while(osc_msg_it_s_hasNext(it)){
					t_osc_atom_s *as = osc_msg_it_s_next(it);
					t_osc_atom_u *au = osc_atom_array_u_get(atom_ar, i);
					osc_atom_s_deserialize(as, &au);
					i++;
				}
				osc_msg_it_s_destroy(it);
				osc_rset_free(rset);
				osc_message_s_free(m);
				return 0;
			}
		}
		osc_rset_free(rset);
	}

	//osc_error_handler(__FILE__,
	//__func__,
	//__LINE__,
	//OSC_ERR_EXPR_ADDRESSUNBOUND,
	//"address %s is unbound\n",
	//arg->arg.osc_address);

	return OSC_ERR_EXPR_ADDRESSUNBOUND;
}

long osc_expr_ast_oscaddress_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return snprintf(buf, n, "%s", osc_expr_ast_oscaddress_getAddress((t_osc_expr_ast_oscaddress *)v));
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_oscaddress_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		return (t_osc_expr_ast_expr *)osc_expr_ast_oscaddress_alloc(osc_expr_ast_oscaddress_getAddress((t_osc_expr_ast_oscaddress *)ast));
	}else{
		return NULL;
	}
}

void osc_expr_ast_oscaddress_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_mem_free(osc_expr_ast_oscaddress_getAddress((t_osc_expr_ast_oscaddress *)v));
		osc_mem_free(v);
	}
}

char *osc_expr_ast_oscaddress_getAddress(t_osc_expr_ast_oscaddress *v)
{
	if(v){
		return v->oscaddress;
	}
	return NULL;
}

char *osc_expr_ast_oscaddress_getAddressCopy(t_osc_expr_ast_oscaddress *v)
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

void osc_expr_ast_oscaddress_setAddress(t_osc_expr_ast_oscaddress *v, char *s)
{
	if(v){
		long len = strlen(s);
		char *copy = osc_mem_alloc(len);
		strncpy(copy, s, len);
		v->oscaddress = copy;
	}
}

t_osc_expr_ast_oscaddress *osc_expr_ast_oscaddress_alloc(char *address)
{
	t_osc_expr_ast_oscaddress *v = osc_mem_alloc(sizeof(t_osc_expr_ast_oscaddress));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v, OSC_EXPR_AST_NODETYPE_OSCADDRESS, NULL, osc_expr_ast_oscaddress_evalInLexEnv, osc_expr_ast_oscaddress_format, osc_expr_ast_oscaddress_free, osc_expr_ast_oscaddress_copy, sizeof(t_osc_expr_ast_oscaddress));
		osc_expr_ast_oscaddress_setAddress(v, address);
	}
	return v;
}
