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


/*
define(`types', ``c', `C', `u', `U', `i', `I', `h', `H', `f', `d', `s', `T', `F', `N', `B', `t', `b'')

define(`make_define_name', ``OSC_EXPR_BUILTIN_'translit($1, a-z, A-Z)`_$2'')
define(`make_function_name', `osc_expr_builtin_$1_$2')

define(`make_def', ``#define 'make_define_name($1, $2) make_function_name($1, $2)')

define(`make_ndef',
``#ifndef' make_define_name($1, $2)
`#define' make_define_name($1, $2)` NULL'
`#endif'')

define(`make_ndefs', `ifelse(`$#', `1', , `$#', `2', `make_ndefs($1)make_ndef($1, $2)', `make_ndef($1, $2)
make_ndefs($1, shift(shift($@)))')')

define(`DEFINE_TYPES_FOR_FUNCTION', `ifelse(`$#', `1',
`make_ndefs($1, types)
make_vtab($1)',
`$#', `2', `make_def($1, $2)
DEFINE_TYPES_FOR_FUNCTION($1)', `make_def($1, $2)
DEFINE_TYPES_FOR_FUNCTION($1, shift(shift($@)))')
')

define(`make_vtab', 
``#define OSC_EXPR_BUILTIN_'translit($1, a-z, A-Z)`_VTAB (void *[]){\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*10*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*20*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*30*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*40*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*50*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*60*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*65: A*/NULL,\
'make_define_name($1, B)`,\
'make_define_name($1, C)`,\
NULL,\
NULL,\
'make_define_name($1, F)`,\
NULL,\
'make_define_name($1, H)`,\
'make_define_name($1, I)`,\
NULL,\
NULL,\
NULL,\
NULL,\
'make_define_name($1, N)`,\
NULL,\
/*80: P*/NULL,\
NULL,\
NULL,\
NULL,\
'make_define_name($1, T)`,\
'make_define_name($1, U)`,\
NULL,\
NULL,\
NULL,\
NULL,\
/*90: Z*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
/*97: a*/NULL,\
'make_define_name($1, b)`,\
'make_define_name($1, c)`,\
'make_define_name($1, d)`,\
NULL,\
'make_define_name($1, f)`,\
NULL,\
'make_define_name($1, h)`,\
'make_define_name($1, i)`,\
NULL,\
NULL,\
NULL,\
NULL,\
/*110: n*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
'make_define_name($1, t)`,\
'make_define_name($1, u)`,\
NULL,\
NULL,\
/*120: x*/NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL,\
NULL\
}'')

*/

DEFINE_TYPES_FOR_FUNCTION(add, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(sub, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(mul, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(div, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(le, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(ge, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(lt, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(gt, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(eq, c, C, u, U, i, I, h, H, f, d, T, F)
DEFINE_TYPES_FOR_FUNCTION(neq, c, C, u, U, i, I, h, H, f, d, T, F)
DEFINE_TYPES_FOR_FUNCTION(mod, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(pow, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(aseq, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(scale, c, C, u, U, i, I, h, H, f, d)
DEFINE_TYPES_FOR_FUNCTION(and, T, F)
DEFINE_TYPES_FOR_FUNCTION(or, T, F)
