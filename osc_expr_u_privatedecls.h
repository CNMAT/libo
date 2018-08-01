
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

/** 	\file osc_expr_privatedecls.h
	\author John MacCallum

*/

static void osc_expr_err_badInfixArg(void *context, char *func, char typetag, int argnum, t_osc_atom_u *left, t_osc_atom_u *right);
static void osc_expr_err_unbound(void *context, char *address, char *func);
static void osc_expr_err_argnum(void *context, unsigned int expected, unsigned int found, unsigned int optional_args_allowed, char *func);

static t_osc_err osc_expr_u_evalArgInLexEnv(t_osc_expr_arg *arg,
					  t_osc_expr_lexenv *lexenv,
					  t_osc_bndl_u *u_bndl,
					  t_osc_atom_ar_u **out,
						void *context);

static int osc_expr_u_specFunc_apply(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_map(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_reduce(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_assign(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_assigntoindex(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_if(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_emptybundle(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_bound(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_exists(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_getaddresses(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_delete(t_osc_expr *f,
                    t_osc_expr_lexenv *lexenv,
                    t_osc_bndl_u *u_bndl,
				    t_osc_atom_ar_u **out,
 					 void *context);

static int osc_expr_u_specFunc_getmsgcount(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_value(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_quote(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_eval(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_tokenize(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_compile(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

/*
static int osc_expr_u_specFunc_gettimetag(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_settimetag(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);
*/
static int osc_expr_u_specFunc_getBundleMember(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_assignToBundleMember(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_andalso(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);

static int osc_expr_u_specFunc_orelse(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context);
