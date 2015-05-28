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

int main(int av, char **ac)
{
	int buflen = 65536;
	char *buf = osc_mem_alloc(buflen);
	int n = 0;
	t_osc_bndl *g = NULL;
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
			continue;
		}
		t_osc_bndl *w = NULL;
		if(g == NULL){
			w = osc_bndl_retain(b);
		}else{
			w = osc_bndl_union(g, b);
		}
		osc_bndl_release(g);
		t_osc_bndl *r = osc_bndl_eval(w, NULL);
		osc_bndl_format_m((t_osc_bndl_m *)r, 0);
		printf("%s\n", osc_bndl_getPrettyPtr(r));
		g = r;
		osc_bndl_release(w);
		n = 0;
	}
	return 0;
}
