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

#include <string.h>
#include "osc_expr_lexenv.h"
#include "osc_hashtab.h"
#include "osc_mem.h"

void osc_expr_lexenv_dtor(char *key, void *val)
{
	if(key){
		osc_mem_free(key);
	}
	if(val){
		osc_atom_array_u_free((t_osc_atom_ar_u *)val);
	}
}

t_osc_expr_lexenv *osc_expr_lexenv_alloc(void)
{
	return osc_hashtab_new(0, osc_expr_lexenv_dtor);
}

void osc_expr_lexenv_free(t_osc_expr_lexenv *lexenv)
{
	osc_hashtab_destroy(lexenv);
}

void osc_expr_lexenv_copy_cb(char *key, void *val, void *context)
{
	t_osc_hashtab *ht = (t_osc_hashtab *)context;
	if(!ht || !val || !key){
		return;
	}
	int keylen = strlen(key) + 1;
	//char *key_copy = osc_mem_alloc(keylen);
	//strncpy(key_copy, key, keylen);
	//t_osc_atom_ar_u *ar = osc_atom_array_u_copy((t_osc_atom_ar_u *)val);
	osc_hashtab_store(ht, keylen - 1, key, val);
}

void osc_expr_lexenv_copy(t_osc_expr_lexenv **dest, t_osc_expr_lexenv *src)
{
	t_osc_hashtab *copy = osc_hashtab_new(0, osc_expr_lexenv_dtor);
	osc_hashtab_foreach(src, osc_expr_lexenv_copy_cb, (void *)copy);
	*dest = (t_osc_expr_lexenv *)copy;
}

void osc_expr_lexenv_deepCopy_cb(char *key, void *val, void *context)
{
	t_osc_hashtab *ht = (t_osc_hashtab *)context;
	if(!ht || !val || !key){
		return;
	}
	int keylen = strlen(key) + 1;
	char *key_copy = osc_mem_alloc(keylen);
	strncpy(key_copy, key, keylen);
	t_osc_atom_ar_u *ar = osc_atom_array_u_copy((t_osc_atom_ar_u *)val);
	osc_hashtab_store(ht, keylen - 1, key_copy, (void *)ar);
}

void osc_expr_lexenv_deepCopy(t_osc_expr_lexenv **dest, t_osc_expr_lexenv *src)
{
	t_osc_hashtab *copy = osc_hashtab_new(0, osc_expr_lexenv_dtor);
	osc_hashtab_foreach(src, osc_expr_lexenv_deepCopy_cb, (void *)copy);
	*dest = (t_osc_expr_lexenv *)copy;
}

void osc_expr_lexenv_bind(t_osc_expr_lexenv *lexenv, char *varname, t_osc_atom_ar_u *val)
{
	int len = strlen(varname) + 1;
	char *copy = osc_mem_alloc(len);
	strncpy(copy, varname, len);
	if(lexenv && varname){
		osc_hashtab_store((t_osc_hashtab *)lexenv, len - 1, copy, (void *)val);
	}
}

t_osc_atom_ar_u *osc_expr_lexenv_lookup(t_osc_expr_lexenv *lexenv, char *varname)
{
	if(lexenv && varname){
		return osc_hashtab_lookup((t_osc_hashtab *)lexenv, strlen(varname), varname);
	}
	return NULL;
}

