/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2013, The Regents of
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

#include <stdio.h>
#include "osc_linkedlist.h"
#include "osc_linkedlist.r"
#include "osc.h"
#include "osc_mem.h"

typedef struct _osc_linkedlist_cloneContext{
	t_osc_linkedlist *clone;
	void (*copy)(void**, void*);
} t_osc_linkedlist_cloneContext;

t_osc_linkedlist_elem *osc_linkedlist_getHead(t_osc_linkedlist *ll);
t_osc_linkedlist_elem *osc_linkedlist_getTail(t_osc_linkedlist *ll);
void osc_linkedlist_elem_free(t_osc_linkedlist_elem *e);

void osc_linkedlist_init(t_osc_linkedlist *ll, t_osc_linkedlist_dtor dtor)
{
	ll->e = NULL;
	ll->count = 0;
	ll->head = NULL;
	ll->tail = NULL;
	ll->dtor = dtor;
}

t_osc_linkedlist *osc_linkedlist_new(t_osc_linkedlist_dtor dtor)
{
	t_osc_linkedlist *ll = (t_osc_linkedlist *)osc_mem_alloc(sizeof(t_osc_linkedlist));
	if(ll){
		osc_linkedlist_init(ll, dtor);
	}
	return ll;
}

void osc_linkedlist_clear(t_osc_linkedlist *ll)
{
	if(ll){
		t_osc_linkedlist_elem *next = NULL;
		t_osc_linkedlist_elem *e = ll->head;
		if(ll->dtor){
			while(e){
				next = e->next;
				if(e->data){
					ll->dtor(e->data);
				}
				osc_linkedlist_elem_free(e);
				e = next;
			}
		}else{
			while(e){
				next = e->next;
				osc_linkedlist_elem_free(e);
				e = next;
			}
		}
		ll->head = ll->tail = NULL;
		ll->count = 0;
	}
}

void osc_linkedlist_cloneCallback(void *context, int index, void *data)
{
	t_osc_linkedlist_cloneContext *clone = (t_osc_linkedlist_cloneContext *)context;
	if(index >= 0 && data){
		void *copy = NULL;
		clone->copy(&copy, data);
		osc_linkedlist_append(clone->clone, copy);
	}
}

t_osc_linkedlist *osc_linkedlist_clone(t_osc_linkedlist *ll, void (*copydata)(void**, void*))
{
	t_osc_linkedlist *clone = osc_linkedlist_new(ll->dtor);
	t_osc_linkedlist_cloneContext context;
	context.clone = clone;
	context.copy = copydata;
	osc_linkedlist_iterate(ll, osc_linkedlist_cloneCallback, (void *)&context);
	return clone;
}

void osc_linkedlist_destroy(t_osc_linkedlist *ll)
{
	if(ll){
		osc_linkedlist_clear(ll);
		osc_mem_free(ll);
	}
}

t_osc_linkedlist_elem *osc_linkedlist_elem_alloc(void *data)
{
	t_osc_linkedlist_elem *e = (t_osc_linkedlist_elem *)osc_mem_alloc(sizeof(t_osc_linkedlist_elem));
	if(e){
		e->data = data;
		e->next = e->prev = NULL;
	}
	return e;
}

void osc_linkedlist_elem_free(t_osc_linkedlist_elem *e)
{
	if(e){
		osc_mem_free(e);
	}
}

void osc_linkedlist_prepend(t_osc_linkedlist *ll, void *data)
{
	if(ll && data){
		t_osc_linkedlist_elem *e = osc_linkedlist_elem_alloc(data);
		if(ll->head){
			ll->head->prev = e;
			e->next = ll->head;
			e->prev = NULL;
			ll->head = e;
		}else{
			ll->head = e;
			ll->tail = e;
		}
		ll->count++;
	}
}

void osc_linkedlist_append(t_osc_linkedlist *ll, void *data)
{
	if(ll && data){
		t_osc_linkedlist_elem *e = osc_linkedlist_elem_alloc(data);
		if(ll->tail){
			ll->tail->next = e;
			e->next = NULL;
			e->prev = ll->tail;
			ll->tail = e;
		}else{
			ll->head = e;
			ll->tail = e;
		}
		ll->count++;
	}
}

void osc_linkedlist_unlink(t_osc_linkedlist *ll, t_osc_linkedlist_elem *e)
{
	if(ll && e){
		if(ll->head == e){
			ll->head = e->next;
			/*
			if(ll->head){
				ll->head->prev = NULL;
			}
			*/
		}
		if(ll->tail == e){
			ll->tail = e->prev;
			/*
			if(ll->tail){
				ll->tail->next = NULL;
			}
			*/
		}
		if(e->next){
			e->next->prev = e->prev;
		}
		if(e->prev){
			e->prev->next = e->next;
		}

		e->next = e->prev = NULL;
		ll->count--;
	}
}

t_osc_linkedlist_elem *osc_linkedlist_getHeadElem(t_osc_linkedlist *ll)
{
	if(ll){
		return ll->head;
	}
	return NULL;
}

t_osc_linkedlist_elem *osc_linkedlist_getTailElem(t_osc_linkedlist *ll)
{
	if(ll){
		return ll->tail;
	}
	return NULL;
}

void *osc_linkedlist_peekHead(t_osc_linkedlist *ll)
{
	if(!ll){
		return NULL;
	}
	t_osc_linkedlist_elem *e = ll->head;
	if(e){
		return e->data;
	}else{
		return NULL;
	}
}

void *osc_linkedlist_peekTail(t_osc_linkedlist *ll)
{
	if(!ll){
		return NULL;
	}
	t_osc_linkedlist_elem *e = ll->tail;
	if(e){
		return e->data;
	}else{
		return NULL;
	}
}

void *osc_linkedlist_peekNth(t_osc_linkedlist *ll, int n)
{
	if(!ll){
		return NULL;
	}
	if(n == 0){
		return osc_linkedlist_peekHead(ll);
	}
	if(n == ll->count - 1 || n == -1){
		return osc_linkedlist_peekTail(ll);
	}
	t_osc_linkedlist_elem *e = NULL;
	unsigned long count = osc_linkedlist_getCount(ll);
	if(n < count / 2){
		e = osc_linkedlist_getHeadElem(ll);
		int i = 0;
		while(e){
			if(i == n){
				return e->data;
			}
			i++;
			e = e->next;
		}
	}else{
		e = osc_linkedlist_getTailElem(ll);
		int i = count - 1;
		while(e){
			if(i == n){
				return e->data;
			}
			i--;
			e = e->prev;
		}
	}
	return NULL;
}

void *osc_linkedlist_popHead(t_osc_linkedlist *ll)
{
	if(!ll){
		return NULL;
	}
	t_osc_linkedlist_elem *e = ll->head;
	if(e){
		osc_linkedlist_unlink(ll, e);
		void *data = e->data;
		osc_linkedlist_elem_free(e);
		return data;
	}else{
		return NULL;
	}
}

void *osc_linkedlist_popTail(t_osc_linkedlist *ll)
{
	if(!ll){
		return NULL;
	}
	t_osc_linkedlist_elem *e = ll->tail;
	if(e){
		osc_linkedlist_unlink(ll, e);
		void *data = e->data;
		osc_linkedlist_elem_free(e);
		return data;
	}else{
		return NULL;
	}

}

void *osc_linkedlist_popNth(t_osc_linkedlist *ll, int n)
{
	if(!ll){
		return NULL;
	}
	if(n == 0){
		return osc_linkedlist_popHead(ll);
	}
	if(n == ll->count - 1 || n == -1){
		return osc_linkedlist_popTail(ll);
	}
	t_osc_linkedlist_elem *e = NULL;
	unsigned long count = osc_linkedlist_getCount(ll);
	int found = 0;
	if(n < count / 2){
		e = osc_linkedlist_getHeadElem(ll);
		int i = 0;
		while(e){
			if(i == n){
				found = 1;
				break;
			}
			i++;
			e = e->next;
		}
	}else{
		e = osc_linkedlist_getTailElem(ll);
		int i = count - 1;
		while(e){
			if(i == n){
				found = 1;
				break;
			}
			i--;
			e = e->prev;
		}
	}
	if(found){
		osc_linkedlist_unlink(ll, e);
		void *data = e->data;
		osc_linkedlist_elem_free(e);
		return data;
	}else{
		return NULL;
	}
}


void osc_linkedlist_iterate(t_osc_linkedlist *ll, void (*cb)(void*, int, void*), void *context)
{
	if(!cb){
		return;
	}
	if(ll){
		int i = 0;
		t_osc_linkedlist_elem *e = osc_linkedlist_getHeadElem(ll);
		t_osc_linkedlist_elem *next = NULL;
		while(e){
			next = e->next;
			cb(context, i++, e->data);
			e = next;
		}
	}
	cb(context, -1, NULL);
}

unsigned long osc_linkedlist_getCount(t_osc_linkedlist *ll)
{
	if(ll){
		return ll->count;
	}else{
		return 0;
	}
}
