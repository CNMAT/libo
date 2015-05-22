/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009-14, The Regents of
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

 */
#include <string.h>
#include <stdio.h>
#include "osc.h"
#include "osc_typetag.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_hashtab.h"

struct _osc_typetag_type {
	char typetag;
	char *name;
	int weight;
	struct _osc_typetag_type **edges;
};

#define OSC_TYPETAG_DEFTYPE(typetag, name, weight, ...)		\
	static t_osc_typetag_type _osc_typetag_##name = {	\
		typetag,				\
		#name,					\
		weight,					\
		(t_osc_typetag_type *[]){__VA_ARGS__}	\
	};\
	t_osc_typetag_type *osc_typetag_##name = &_osc_typetag_##name

#if OSC_TIMETAG_TYPETAG != 't'
#error Incorrect value for timetag typetag!
#endif 
#if OSC_BUNDLE_TYPETAG != 'B'
#error Incorrect value for bundle typetag!
#endif 
#if OSC_EXPR_TYPETAG != 'A'
#error Incorrect value for expr typetag!
#endif 

OSC_TYPETAG_DEFTYPE('s', string, 17, NULL);
OSC_TYPETAG_DEFTYPE('B', bundle, 16, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('b', blob, 15, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('A', expr, 14, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('d', double, 13, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('t', timetag, 12, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('H', uint64, 11, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('h', int64, 10, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('I', uint32, 9, &_osc_typetag_int64, &_osc_typetag_uint64, &_osc_typetag_double, NULL);
OSC_TYPETAG_DEFTYPE('f', float, 8, &_osc_typetag_double, NULL);
OSC_TYPETAG_DEFTYPE('i', int32, 7, &_osc_typetag_double, &_osc_typetag_int64, NULL);
OSC_TYPETAG_DEFTYPE('U', uint16, 6, &_osc_typetag_int32, &_osc_typetag_uint32, &_osc_typetag_float, NULL);
OSC_TYPETAG_DEFTYPE('u', int16, 5, &_osc_typetag_int32, &_osc_typetag_float, NULL);
OSC_TYPETAG_DEFTYPE('C', uint8, 4, &_osc_typetag_int16, &_osc_typetag_uint16, &_osc_typetag_float, NULL);
OSC_TYPETAG_DEFTYPE('c', int8, 3, &_osc_typetag_int16, &_osc_typetag_float, NULL);
OSC_TYPETAG_DEFTYPE('T', true, 2, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('F', false, 1, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE('N', null, 0, &_osc_typetag_string, NULL);
OSC_TYPETAG_DEFTYPE(0, unknown, -1, &_osc_typetag_true, &_osc_typetag_false, &_osc_typetag_null, &_osc_typetag_int8, &_osc_typetag_uint8, &_osc_typetag_float, &_osc_typetag_timetag, &_osc_typetag_bundle, &_osc_typetag_string, &_osc_typetag_blob, &_osc_typetag_expr, NULL);

// supported typetags:
// N T F c C u U i I h H f d s OSC_TYPETAG_EXPR OSC_TYPETAG_BUNDLE OSC_TYPETAG_TIMETAG

static t_osc_typetag_type *osc_typetag_map[128] = {
	&_osc_typetag_unknown, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 32
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 64
	NULL, &_osc_typetag_expr, &_osc_typetag_bundle, &_osc_typetag_uint8, NULL, NULL, &_osc_typetag_false, NULL,
	&_osc_typetag_uint64, &_osc_typetag_uint32, NULL, NULL, NULL, NULL, &_osc_typetag_null, NULL,
	NULL, NULL, NULL, NULL, &_osc_typetag_true, &_osc_typetag_uint16, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 96
	NULL, NULL, &_osc_typetag_blob, &_osc_typetag_int8, &_osc_typetag_double, NULL, &_osc_typetag_float, NULL,
	&_osc_typetag_int64, &_osc_typetag_int32, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, &_osc_typetag_string, &_osc_typetag_timetag, &_osc_typetag_int16, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static t_osc_typetag_type *_osc_typetag_findLUB_t1(t_osc_typetag_type *t1, t_osc_typetag_type *t2, t_osc_typetag_type *lub, int *visited)
{
	if(!t1 || !t2){
		return lub;
	}
	if(visited[t1->weight]){
		return lub;
	}
	visited[t1->weight] = 1;
	if(t1 == t2){
		if(!lub || t1->weight < lub->weight){
			lub = t1;
		}
	}
	{
		int i = 0;
		t_osc_typetag_type *t1_edge = t1->edges[0];
		while(t1_edge){
			if(t1_edge == t2){
				if(!lub || t1_edge->weight < lub->weight){
					lub = t1_edge;
				}
			}	
			lub = _osc_typetag_findLUB_t1(t1_edge, t2, lub, visited);
			i++;
			t1_edge = t1->edges[i];
		}
	}

	return lub;
}

static t_osc_typetag_type *_osc_typetag_findLUB_t2(t_osc_typetag_type *t1, t_osc_typetag_type *t2, t_osc_typetag_type *lub)
{
	if(!t1 || !t2){
		return lub;
	}
	int visited[18];
	memset(visited, '\0', sizeof(int) * 18);
	lub = _osc_typetag_findLUB_t1(t1, t2, lub, visited);
	{
		int i = 0;
		t_osc_typetag_type *t2_edge = t2->edges[0];
		while(t2_edge){
			lub = _osc_typetag_findLUB_t2(t1, t2_edge, lub);
			i++;
			t2_edge = t2->edges[i];
		}
	}
	return lub;
}

char osc_typetag_findLUB(char t1, char t2)
{
	if(t1 > 127 || t1 < 0 || t2 > 127 || t2 < 0){
		return 0;
	}
	t_osc_typetag_type *_t1 = osc_typetag_map[(int)t1];
	t_osc_typetag_type *_t2 = osc_typetag_map[(int)t2];
	t_osc_typetag_type *lub = NULL;
	lub = _osc_typetag_findLUB_t2(_t1, _t2, NULL);
	if(lub){
		return lub->typetag;
	}else{
		return 0;
	}
}

static t_osc_typetag_type *_osc_typetag_isSubtype(t_osc_typetag_type *t1, t_osc_typetag_type *t2)
{
	if(!t1 || !t2){
		return NULL;
	}
	if(t1 == t2){
		return t1;
	}
	{
		int i = 0;
		t_osc_typetag_type *t1_edge = t1->edges[0];
		while(t1_edge){
			if(t1_edge == t2){
				return t2;
			}	
			t_osc_typetag_type *t = _osc_typetag_isSubtype(t1_edge, t2);
			if(t){
				return t;
			}
			i++;
			t1_edge = t1->edges[i];
		}
	}

	return NULL;
}

char osc_typetag_isSubtype(char t1, char t2)
{
	if(t1 > 127 || t1 < 0 || t2 > 127 || t2 < 0){
		return 0;
	}
	t_osc_typetag_type *_t1 = osc_typetag_map[(int)t1];
	t_osc_typetag_type *_t2 = osc_typetag_map[(int)t2];
	t_osc_typetag_type *t = _osc_typetag_isSubtype(_t1, _t2);
	if(t){
		return t->typetag;
	}else{
		return 0;
	}
}

char *osc_typetag_name(char typetag)
{
	if(typetag > 127 || typetag < 0){
		return NULL;
	}
	t_osc_typetag_type *t = osc_typetag_map[(int)typetag];
	if(t){
		return t->name;
	}
	return NULL;
}

void osc_typetag_formatTypeLattice_dot_r(t_osc_typetag_type *tt, int *visited)//t_osc_hashtab *ht)
{
	if(visited[tt->weight] && tt->weight >= 0){
		return;
	}
	visited[tt->weight] = 1;
	t_osc_typetag_type **edges = tt->edges;
	t_osc_typetag_type *edge = *edges;
	int i = 0;
	while(edge){
		printf("%s -> %s;\n", tt->name, edge->name);
		osc_typetag_formatTypeLattice_dot_r(edge, visited);
		edge = edges[++i];
	}
}

void osc_typetag_formatTypeLattice_dot(void)
{
	int visited[18];
	memset(visited, '\0', sizeof(int) * 18);
	printf("digraph typelattice {\n");
	printf("rankdir = BT;\n");
	osc_typetag_formatTypeLattice_dot_r(osc_typetag_unknown, visited);
	printf("}\n");
}


//////////////////////////////////////////////////
// old
//////////////////////////////////////////////////

static char *osc_typetag_strings[] = {"unknown type: (decimal: 0)", "unknown type: (decimal: 1)", "unknown type: (decimal: 2)", "unknown type: (decimal: 3)", "unknown type: (decimal: 4)", "unknown type: (decimal: 5)", "unknown type: (decimal: 6)", "unknown type: (decimal: 7)", "unknown type: (decimal: 8)", "unknown type: (decimal: 9)", "unknown type: (decimal: 10)", "unknown type: (decimal: 11)", "unknown type: (decimal: 12)", "unknown type: (decimal: 13)", "unknown type: (decimal: 14)", "unknown type: (decimal: 15)", "unknown type: (decimal: 16)", "unknown type: (decimal: 17)", "unknown type: (decimal: 18)", "unknown type: (decimal: 19)", "unknown type: (decimal: 20)", "unknown type: (decimal: 21)", "unknown type: (decimal: 22)", "unknown type: (decimal: 23)", "unknown type: (decimal: 24)", "unknown type: (decimal: 25)", "unknown type: (decimal: 26)", "unknown type: (decimal: 27)", "unknown type: (decimal: 28)", "unknown type: (decimal: 29)", "unknown type: (decimal: 30)", "unknown type: (decimal: 31)", "unknown type: (decimal: 32)", "unknown type: '!'", "unknown type: '\"'", "unknown type: '#'", "unknown type: '$'", "unknown type: '\%'", "unknown type: '&'", "unknown type: '''", "unknown type: '('", "unknown type: ')'", "unknown type: '*'", "unknown type: '+'", "unknown type: ','", "unknown type: '-'", "bundle", "unknown type: '/'", "unknown type: '0'", "unknown type: '1'", "unknown type: '2'", "unknown type: '3'", "unknown type: '4'", "unknown type: '5'", "unknown type: '6'", "unknown type: '7'", "unknown type: '8'", "unknown type: '9'", "unknown type: ':'", "unknown type: ';'", "unknown type: '<'", "unknown type: '='", "unknown type: '>'", "unknown type: '?'", "unknown type: '@'", "unknown type: 'A'", "unknown type: 'B'", "unsigned int8", "unknown type: 'D'", "unknown type: 'E'", "false", "unknown type: 'G'", "unsigned int64", "unsigned int32", "unknown type: 'J'", "unknown type: 'K'", "unknown type: 'L'", "unknown type: 'M'", "null", "unknown type: 'O'", "unknown type: 'P'", "unknown type: 'Q'", "unknown type: 'R'", "unknown type: 'S'", "true", "unsigned int16", "unknown type: 'V'", "unknown type: 'W'", "unknown type: 'X'", "unknown type: 'Y'", "unknown type: 'Z'", "unknown type: '['", "unknown type: '\'", "unknown type: ']'", "unknown type: '^'", "unknown type: '_'", "unknown type: '`'", "unknown type: 'a'", "unknown type: 'b'", "int8", "float64", "unknown type: 'e'", "float32", "unknown type: 'g'", "int64", "int32", "unknown type: 'j'", "unknown type: 'k'", "unknown type: 'l'", "unknown type: 'm'", "unknown type: 'n'", "unknown type: 'o'", "unknown type: 'p'", "unknown type: 'q'", "unknown type: 'r'", "string", "unknown type: 't'", "int16", "unknown type: 'v'", "unknown type: 'w'", "unknown type: 'x'", "unknown type: 'y'", "unknown type: 'z'", "unknown type: '{'", "unknown type: '|'", "unknown type: '}'", "unknown type: '~'", "unknown type: (decimal: 127)"};

static char osc_typetag_typetags[] = {'N', 'F', 'T', 'c', 'C', 'u', 'U', 'i', 'I', 'h', 'H', 'f', 'd', OSC_TIMETAG_TYPETAG, 's'};

char osc_typetag_indexes[] __attribute__((unused)) = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 0, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 12, 0, 11, 0, 9, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

char *osc_typetag_str(int8_t tt)
{
	return osc_typetag_strings[tt];
}

char osc_typetag_getLargestType(int argc, t_osc_atom_u **argv)
{
	int nspecial = 0;
	char last_special = 0;
	int special_typetags_are_all_the_same = 1;
	for(int i = 0; i < argc; i++){
		char tt = osc_atom_u_getTypetag(argv[i]);
		switch(tt){
		case 'b':
		case OSC_BUNDLE_TYPETAG:
			nspecial++;
		}
		if(last_special == 0){
			last_special = tt;
		}else{
			if(tt != last_special){
				special_typetags_are_all_the_same = 0;
			}
		}
	}
	if(nspecial > 0 && nspecial != argc){
		// typetags can't be compared
		return 0;
	}
	if(nspecial > 0){
		if(!special_typetags_are_all_the_same){
			// typetags can't be compared
			return 0;
		}
		return last_special;
	}

	uint32_t largest_type = 0;

	for(int i = 0; i < argc; i++){
		switch(osc_atom_u_getTypetag(argv[i])){
		case 'N':
			if(OSC_TYPES_NULL > largest_type){
				largest_type = OSC_TYPES_NULL;
			}
			break;
		case 'T':
		case 'F':
			if(OSC_TYPES_TRUE > largest_type){
				largest_type = OSC_TYPES_TRUE;
			}
			break;
		case 'c':
			if(OSC_TYPES_INT8 > largest_type){
				largest_type = OSC_TYPES_INT8;
			}
			break;
		case 'C':
			if(OSC_TYPES_UINT8 > largest_type){
				largest_type = OSC_TYPES_UINT8;
			}
			break;
		case 'u':
			if(OSC_TYPES_INT16 > largest_type){
				largest_type = OSC_TYPES_INT16;
			}
			break;
		case 'U':
			if(OSC_TYPES_UINT16 > largest_type){
				largest_type = OSC_TYPES_UINT16;
			}
			break;
		default:
		case 'i':
			if(OSC_TYPES_INT32 > largest_type){
				largest_type = OSC_TYPES_INT32;
			}
			break;
		case 'I':
			if(OSC_TYPES_UINT32 > largest_type){
				largest_type = OSC_TYPES_UINT32;
			}
			break;
		case 'h':
			if(OSC_TYPES_INT64 > largest_type){
				largest_type = OSC_TYPES_INT64;
			}
			break;
		case 'H':
			if(OSC_TYPES_UINT64 > largest_type){
				largest_type = OSC_TYPES_UINT64;
			}
			break;
		case 'f':
			if(OSC_TYPES_FLOAT32 > largest_type){
				largest_type = OSC_TYPES_FLOAT32;
			}
			break;
		case 'd':
			if(OSC_TYPES_FLOAT64 > largest_type){
				largest_type = OSC_TYPES_FLOAT64;
			}
			break;
		case OSC_TIMETAG_TYPETAG:
			if(OSC_TYPES_TIMETAG > largest_type){
				largest_type = OSC_TYPES_TIMETAG;
			}
			break;
		case 's':
			if(OSC_TYPES_STRING > largest_type){
				largest_type = OSC_TYPES_STRING;
			}
			break;
		}
	}
	return osc_typetag_typetags[largest_type];
}
