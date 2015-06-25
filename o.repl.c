#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osc.h"
#include "osc_mem.h"
#include "osc_parse.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_timetag.h"
#include "osc_builtin.h"

int main(int av, char **ac)
{
	int buflen = 65536;
	char *buf = osc_mem_alloc(buflen);
	int n = 0;
	t_osc_bndl *g = NULL;
	t_osc_bndl *math = osc_parse(osc_builtin_math);
	while(1){
		printf("> ");
		while((buf[n++] = getchar()) != '\n'){
			if(n == buflen){
				buf = osc_mem_resize(buf, buflen += 65536);
			}
		}
		if(n == 2){
			switch(buf[0]){
			case 'c':
				osc_bndl_release(g);
				g = NULL;
				break;
			default:
				printf("I don't understand %c\n", buf[0]);
				break;
			}
			n = 0;
			continue;
		}
		t_osc_bndl *b = osc_parse(buf);
		if(!b){
			n = 0;
			continue;
		}
		osc_bndl_format_m((t_osc_bndl_m *)b, 0);
		printf("parsed bndl:\n%s\n", osc_bndl_getPrettyPtr(b));
		t_osc_bndl *w = NULL;
		if(g == NULL){
			w = osc_bndl_retain(b);
			//w = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_expraddress, 1, osc_atom_allocExpr(b, 1)));
		}else{
			w = osc_bndl_union(b, g);
		}
		//t_osc_bndl *ww = w;
		//w = osc_bndl_union(w, math);
		//osc_bndl_release(ww);
		osc_bndl_format_m((t_osc_bndl_m *)w, 0);
		printf("working bndl:\n%s\n", osc_bndl_getPrettyPtr(w));
		osc_bndl_release(g);
		t_osc_bndl *r = osc_bndl_reduce(w, math);
		osc_bndl_format_m((t_osc_bndl_m *)r, 0);
		printf("eval'd bndl:\n%s\n", osc_bndl_getPrettyPtr(r));
		//t_osc_bndl *rr = osc_bndl_reduce(r, math);
		//osc_bndl_format_m((t_osc_bndl_m *)rr, 0);
		//printf("reduced bndl:\n%s\n", osc_bndl_getPrettyPtr(rr));
		g = r;
		osc_bndl_release(w);
		n = 0;
	}
	return 0;
}
