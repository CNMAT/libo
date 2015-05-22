#include "osc_pvec.h"
#include "osc_mem.h"
//#define __OSC_PROFILE__
#include "osc_profile.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

int ntabs;
char tabs[128];


//#define P(fmt, ...) if(osc_pvec_debug) printf("%s%s:%d: "fmt"\n", tabs, __func__, __LINE__, ##__VA_ARGS__);
#define P(fmt, ...) {}

//#define OSC_PVEC_ENTER if(osc_pvec_debug){printf("%s%s {\n", tabs, __func__); tabs[ntabs++] = '\t';}; OSC_PROFILE_TIMER_START(t1);
#define OSC_PVEC_ENTER

//#define OSC_PVEC_RETURN if(osc_pvec_debug){OSC_PROFILE_TIMER_STOP(t1); OSC_PROFILE_TIMER_PRINTF(t1); tabs[--ntabs] = '\0'; printf("%s}\n", tabs);}; return;
//#define OSC_PVEC_RETURN_OBJ(obj, ...) if(osc_pvec_debug){OSC_PROFILE_TIMER_STOP(t1); OSC_PROFILE_TIMER_PRINTF(t1); __VA_ARGS__(obj); tabs[--ntabs] = '\0'; printf("%s}\n", tabs);}; return obj;
#define OSC_PVEC_RETURN return;
#define OSC_PVEC_RETURN_OBJ(obj, ...) return obj;

#define OSC_PVEC_INCRC(obj) (obj ? ++(*((int32_t *)(obj))) : -1); if(obj && osc_pvec_debug) P("INC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) - 1, (*((int32_t *)(obj))));
#define OSC_PVEC_DECRC(obj) (obj ? --(*((int32_t *)(obj))) : -1); if(obj && osc_pvec_debug) P("DEC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) + 1, (*((int32_t *)(obj))));
#define OSC_PVEC_GETRC(obj) (obj ? (*((int32_t *)(obj))) : -1)

#define OSC_PVEC2_INCRC(obj) (obj ? ++(*((int32_t *)(obj))) : -1); if(obj && osc_pvec_debug) P("INC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) - 1, (*((int32_t *)(obj))));
#define OSC_PVEC2_DECRC(obj) (obj ? --(*((int32_t *)(obj))) : -1); if(obj && osc_pvec_debug) P("DEC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) + 1, (*((int32_t *)(obj))));
#define OSC_PVEC2_GETRC(obj) (obj ? (*((int32_t *)(obj))) : -1)

int osc_pvec_debug = 1;

void osc_pvec_printRefCount(void *obj)
{
	if(obj){
		printf("%srefcount for %p = %d\n", tabs, obj, OSC_PVEC_GETRC(obj));
	}
}

#define OSC_PVEC_NBRANCHES 32
#define OSC_PVEC_NBITS 5
#define OSC_PVEC_MASK OSC_PVEC_NBRANCHES - 1

#pragma pack(push)
#pragma pack(4)
typedef struct _osc_pvec_ud
{
	int32_t refcount;
	void *data;
} t_osc_pvec_ud;
#pragma pack(pop)

t_osc_pvec_ud *osc_pvec_ud_alloc(void *userdata);
t_osc_pvec_ud *osc_pvec_ud_copy(t_osc_pvec_ud *ud);
void osc_pvec_ud_free(t_osc_pvec_ud *ud, void (*freefn)(void *));

t_osc_pvec_ud *osc_pvec_ud_alloc(void *userdata)
{
	OSC_PVEC_ENTER;
	t_osc_pvec_ud *ud = osc_mem_alloc(sizeof(t_osc_pvec_ud));
	if(ud){
		ud->data = userdata;
		ud->refcount = 1;
	}
	OSC_PVEC_RETURN_OBJ(ud, osc_pvec_printRefCount);
}

t_osc_pvec_ud *osc_pvec_ud_copy(t_osc_pvec_ud *ud)
{
	OSC_PVEC_ENTER;
	if(ud){
		OSC_PVEC_INCRC(ud);
	}
	OSC_PVEC_RETURN_OBJ(ud);
}

void *osc_pvec_ud_getData(t_osc_pvec_ud *ud)
{
	OSC_PVEC_ENTER;
	if(ud){
		OSC_PVEC_RETURN_OBJ(ud->data);
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

void osc_pvec_ud_free(t_osc_pvec_ud *ud, void (*freefn)(void *))
{
	OSC_PVEC_ENTER;
	if(ud){
		if(OSC_PVEC_GETRC(ud) < 0){
			OSC_PVEC_RETURN;
		}
		int rc = OSC_PVEC_DECRC(ud);
		if(rc == 0){
			if(freefn){
				freefn(osc_pvec_ud_getData(ud));
			}
			memset(ud, 0, sizeof(t_osc_pvec_ud));
			OSC_PVEC_DECRC(ud); // set to -1
			osc_mem_free(ud);
		}		
	}
	OSC_PVEC_RETURN;
}

#pragma pack(push)
#pragma pack(4)
struct _osc_pvec_node
{
	int32_t refcount;
	void *array[OSC_PVEC_NBRANCHES];
};
#pragma pack(pop)

//static t_osc_pvec_node _osc_pvec_node_empty;
//static t_osc_pvec_node *osc_pvec_node_empty = &_osc_pvec_node_empty;
t_osc_pvec_node *osc_pvec_node_allocWithArray(int length, void **array);
t_osc_pvec_node *osc_pvec_node_clone(t_osc_pvec_node *n);
t_osc_pvec_node *osc_pvec_node_copy(t_osc_pvec_node *n);
void **osc_pvec_node_getArray(t_osc_pvec_node *node);

t_osc_pvec_node *osc_pvec_node_alloc(void)
{
	OSC_PVEC_ENTER;
	t_osc_pvec_node *n = osc_pvec_node_allocWithArray(0, NULL);
	OSC_PVEC_RETURN_OBJ(n, osc_pvec_printRefCount);
}

t_osc_pvec_node *osc_pvec_node_allocWithArray(int length, void **array)
{
	OSC_PVEC_ENTER
	t_osc_pvec_node *node = osc_mem_alloc(sizeof(t_osc_pvec_node));
	memset(node, 0, sizeof(t_osc_pvec_node));
	if(array){
		memcpy(node->array, array, length * sizeof(void *));
		//}else{
		//memset(node->array, 0, length * sizeof(void *));
	}
	OSC_PVEC_INCRC(node);
	OSC_PVEC_RETURN_OBJ(node);
}

t_osc_pvec_node *osc_pvec_node_clone(t_osc_pvec_node *n)
{
	OSC_PVEC_ENTER
	t_osc_pvec_node *copy = osc_pvec_node_copy(n);
	t_osc_pvec_node *clone = osc_pvec_node_allocWithArray(OSC_PVEC_NBRANCHES, osc_pvec_node_getArray(copy));
	OSC_PVEC_DECRC(copy);
	OSC_PVEC_RETURN_OBJ(clone);
}

t_osc_pvec_node *osc_pvec_node_copy(t_osc_pvec_node *n)
{
	OSC_PVEC_ENTER;
	if(!n){
		OSC_PVEC_RETURN_OBJ(NULL);
	}
	OSC_PVEC_INCRC(n);
	void **array = osc_pvec_node_getArray(n);
	for(int i = 0; i < OSC_PVEC_NBRANCHES; i++){
		P("%d: %p", i, array[i]);
		OSC_PVEC_INCRC(array[i]);
	}
	OSC_PVEC_RETURN_OBJ(n);
}

void osc_pvec_node_free(t_osc_pvec_node *node, int level, void (*freefn)(void *))
{
	OSC_PVEC_ENTER
	if(node){
		if(OSC_PVEC_GETRC(node) < 0){
			OSC_PVEC_RETURN;
		}
		P("%p, rc = %d", node, OSC_PVEC_GETRC(node));
		int rc = OSC_PVEC_DECRC(node);
		if(rc == 0){
			void **array = osc_pvec_node_getArray(node);
			if(level == 0){
				for(int i = 0; i < OSC_PVEC_NBRANCHES; i++){
					osc_pvec_ud_free((t_osc_pvec_ud *)array[i], freefn);
				}				
				memset(node, 0, sizeof(t_osc_pvec_ud));
				OSC_PVEC_DECRC(node); // set to -1
			}else{
				for(int i = 0; i < OSC_PVEC_NBRANCHES; i++){
					osc_pvec_node_free((t_osc_pvec_node *)array[i], level - OSC_PVEC_NBITS, freefn);
				}
				memset(node, 0, sizeof(t_osc_pvec_node));
				OSC_PVEC_DECRC(node); // set to -1
			}
			osc_mem_free(node);
		}
	}
	OSC_PVEC_RETURN;
}

void **osc_pvec_node_getArray(t_osc_pvec_node *node)
{
	OSC_PVEC_ENTER
	if(node){
		OSC_PVEC_RETURN_OBJ(node->array);
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

#pragma pack(push)
#pragma pack(4)
struct _osc_pvec
{
	int32_t refcount;
	int count;
	int shift;
	t_osc_pvec_node *root;
	int taillength;
	void **tail;
	void (*freefn)(void *);
};

struct _osc_pvec2
{
	int32_t refcount;
	int length;
	struct _osc_pvec *head;
	struct _osc_pvec *tail;
};
#pragma pack(pop)

//t_osc_pvec _osc_pvec_empty = {-1, 0, OSC_PVEC_NBITS, &_osc_pvec_node_empty, 0, NULL, NULL};
//t_osc_pvec *osc_pvec_empty = &_osc_pvec_empty;

t_osc_pvec_node *osc_pvec_doAssoc(t_osc_pvec *pvec, int level, t_osc_pvec_node *node, int i, void *val);
t_osc_pvec *osc_pvec_cons(t_osc_pvec *pvec, void *val);
t_osc_pvec_node *osc_pvec_pushTail(t_osc_pvec *pvec, int level, t_osc_pvec_node *parent, t_osc_pvec_node *tailnode);
t_osc_pvec_node *osc_pvec_newPath(int level, t_osc_pvec_node *node);
t_osc_pvec_node *osc_pvec_popTail(t_osc_pvec *pvec, int level, t_osc_pvec_node *node);
void osc_pvec_setCount(t_osc_pvec *pvec, int count);
int osc_pvec_getShift(t_osc_pvec *pvec);
void osc_pvec_setShift(t_osc_pvec *pvec, int shift);
t_osc_pvec_node *osc_pvec_getRoot(t_osc_pvec *pvec);
void osc_pvec_setRoot(t_osc_pvec *pvec, t_osc_pvec_node *root);
int osc_pvec_getTailLength(t_osc_pvec *pvec);
void osc_pvec_setTailLength(t_osc_pvec *pvec, int taillength);
void **osc_pvec_getTail(t_osc_pvec *pvec);
void osc_pvec_setTail(t_osc_pvec *pvec, void **tail);
void (*osc_pvec_getFreeFn(t_osc_pvec *pvec))(void *);
void **osc_pvec_cloneTail(int len, void **array);
void **osc_pvec_copyTail(int len, void **array);

t_osc_pvec_node *osc_pvec_newPath_m(int level, t_osc_pvec_node *node);
t_osc_pvec_node *osc_pvec_doAssoc_m(t_osc_pvec *pvec, int level, t_osc_pvec_node *node, int i, void *val);
t_osc_pvec *osc_pvec_cons_m(t_osc_pvec *pvec, void *val);
t_osc_pvec_node *osc_pvec_pushTail_m(t_osc_pvec *pvec, int level, t_osc_pvec_node *parent, t_osc_pvec_node *tailnode);
t_osc_pvec_node *osc_pvec_newPath_m(int level, t_osc_pvec_node *node);

t_osc_pvec *_osc_pvec_alloc(int count, int shift, t_osc_pvec_node *root, int taillength, void **tail, void (*freefn)(void *))
{
	OSC_PVEC_ENTER
	t_osc_pvec *pvec = osc_mem_alloc(sizeof(t_osc_pvec));
	pvec->count = count;
	pvec->shift = shift;
	pvec->root = root;
	pvec->taillength = taillength;
	pvec->tail = tail;//osc_pvec_copyTail(taillength, tail);
	pvec->refcount = 1;
	pvec->freefn = freefn;
	OSC_PVEC_RETURN_OBJ(pvec, osc_pvec_printRefCount);
}

t_osc_pvec *osc_pvec_alloc(void (*freefn)(void *))
{
	OSC_PVEC_ENTER
	t_osc_pvec *pvec = _osc_pvec_alloc(0, OSC_PVEC_NBITS, osc_pvec_node_alloc(), 0, NULL, freefn);
	OSC_PVEC_RETURN_OBJ(pvec, osc_pvec_printRefCount);
}

void osc_pvec_release(t_osc_pvec *pvec)
{
	OSC_PVEC_ENTER
	if(pvec){
		P("%s: rc = %d", __func__, OSC_PVEC_GETRC(pvec));
		if(OSC_PVEC_GETRC(pvec) < 0){
			OSC_PVEC_RETURN;
		}
		int rc = OSC_PVEC_DECRC(pvec);
		if(rc == 0){
			P("count = %d", osc_pvec_length(pvec));
			P("freeing pvec %p", pvec);
			void (*freefn)(void *) = osc_pvec_getFreeFn(pvec);
			P("freeing root %p", osc_pvec_getRoot(pvec));
			osc_pvec_node_free(osc_pvec_getRoot(pvec), osc_pvec_getShift(pvec), freefn);
			P("done freeing root");
			int taillength = osc_pvec_getTailLength(pvec);
			void **tail = osc_pvec_getTail(pvec);
			P("freeing tail %p", tail);
			if(tail && taillength > 0){
				for(int i = 0; i < taillength; i++){
					P("%d: freeing %p", i, tail[i]);
					osc_pvec_ud_free(tail[i], freefn);
					P("done");
				}
				memset(tail, 0, taillength * sizeof(void *));
				osc_mem_free(tail);
			}
			P("done freeing tail");
			memset(pvec, 0, sizeof(t_osc_pvec));
			OSC_PVEC_DECRC(pvec); // set to -1
			osc_mem_free(pvec);
		}
	}
	OSC_PVEC_RETURN;
}

t_osc_pvec *osc_pvec_copy(t_osc_pvec *pvec)
{
	OSC_PVEC_ENTER
	if(pvec){
		OSC_PVEC_INCRC(pvec);
		OSC_PVEC_RETURN_OBJ(pvec);
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

t_osc_pvec *osc_pvec_clone(t_osc_pvec *pvec)
{
	OSC_PVEC_ENTER
	OSC_PVEC_RETURN_OBJ(NULL);
}

void **osc_pvec_cloneTail(int len, void **tail)
{
	OSC_PVEC_ENTER
	// just increments the refcounts
	void **copy = osc_pvec_copyTail(len, tail);
	// now make a copy and return
	void **newtail = (void **)osc_mem_alloc(len * sizeof(void *));
	memcpy(newtail, copy, len * sizeof(void *));
	OSC_PVEC_RETURN_OBJ(newtail);
}

void **osc_pvec_copyTail(int len, void **tail)
{
	OSC_PVEC_ENTER
	if(tail){
		for(int i = 0; i < len; i++){
			OSC_PVEC_INCRC(tail[i]);
		}
	}
	OSC_PVEC_RETURN_OBJ(tail);
}

// The 0th element of the tail is what index?
int osc_pvec_tailOffset(t_osc_pvec *pvec)
{
	OSC_PVEC_ENTER
	if(osc_pvec_length(pvec) < OSC_PVEC_NBRANCHES){
		OSC_PVEC_RETURN_OBJ(0);
	}
	int to = ((osc_pvec_length(pvec) - 1) >> OSC_PVEC_NBITS) << OSC_PVEC_NBITS;
	OSC_PVEC_RETURN_OBJ(to);
}

// return the array that contains the ith element
void **osc_pvec_getArrayFor(t_osc_pvec *pvec, int i)
{
	OSC_PVEC_ENTER
	if(i >= 0 && i < osc_pvec_length(pvec)){
		if(i >= osc_pvec_tailOffset(pvec)){
			P("%d is in the tail", i);
			OSC_PVEC_RETURN_OBJ(osc_pvec_getTail(pvec));
		}else{
			t_osc_pvec_node *node = osc_pvec_getRoot(pvec);
			for(int level = osc_pvec_getShift(pvec); level > 0; level -= OSC_PVEC_NBITS){
				P("level = %d, node = %p", level, node);
				node = osc_pvec_node_getArray(node)[(i >> level) & OSC_PVEC_MASK];
			}
			P("level = %d, node = %p", 0, node);
			OSC_PVEC_RETURN_OBJ(osc_pvec_node_getArray(node));
		}
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

// get the ith element
void *osc_pvec_nth(t_osc_pvec *pvec, int i)
{
	OSC_PVEC_ENTER
	void **node = osc_pvec_getArrayFor(pvec, i);
	if(node){
		P("found %p", node[i & OSC_PVEC_MASK]);
		t_osc_pvec_ud *ud = node[i & OSC_PVEC_MASK];
		if(ud){
			OSC_PVEC_RETURN_OBJ(osc_pvec_ud_getData(ud));
		}else{
			OSC_PVEC_RETURN_OBJ(NULL);
		}
	}else{
		P("found nothing");
		OSC_PVEC_RETURN_OBJ(NULL);
	}
}

// stick val in the ith slot. list must currently be i - 1 elements long.
t_osc_pvec *osc_pvec_assocN(t_osc_pvec *pvec, int i, void *val)
{
	OSC_PVEC_ENTER
	int count = osc_pvec_length(pvec);
	int shift = osc_pvec_getShift(pvec);
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	int taillength = osc_pvec_getTailLength(pvec);
	void **tail = osc_pvec_getTail(pvec);
	void (*freefn)(void *) = osc_pvec_getFreeFn(pvec);
	{
		t_osc_pvec_ud *ud = osc_pvec_ud_alloc(val);
		if(i >= 0 && i < osc_pvec_length(pvec)){
			P("i(%d) is less than count(%d)", i, count);
			if(i >= osc_pvec_tailOffset(pvec)){
				P("%d is in the tail", i);
				
				void **newtail = (void **)osc_mem_alloc(taillength * sizeof(void *));
				memcpy(newtail, tail, taillength * sizeof(void *));
				for(int j = 0; j < taillength; j++){
					OSC_PVEC_INCRC(newtail[j]);
				}
				t_osc_pvec_ud *old = newtail[i & OSC_PVEC_MASK];
				if(old){
					osc_pvec_ud_free(old, freefn);
				}
				newtail[i & OSC_PVEC_MASK] = ud;
				
				//void **newtail = osc_pvec_insertIntoTail(taillength, tail, i & OSC_PVEC_MASK, ud);
				t_osc_pvec *ret = _osc_pvec_alloc(count, shift, osc_pvec_node_copy(root), taillength, newtail, freefn);
				OSC_PVEC_RETURN_OBJ(ret);
			}
			P("i(%d) is not in the tail", i);
			t_osc_pvec *pv = _osc_pvec_alloc(count, shift, osc_pvec_doAssoc(pvec, shift, osc_pvec_node_copy(root), i, ud), taillength, tail, freefn);
			OSC_PVEC_RETURN_OBJ(pv);
		}
		if(i == osc_pvec_length(pvec)){
			P("i(%d) == count(%d)", i, osc_pvec_length(pvec));
			t_osc_pvec *pv = osc_pvec_cons(pvec, ud);
			OSC_PVEC_RETURN_OBJ(pv)
		}
		P("i(%d) is out of bounds (%d)", i, osc_pvec_length(pvec));
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

t_osc_pvec *osc_pvec_assocN_m(t_osc_pvec *pvec, int i, void *val)
{
	OSC_PVEC_ENTER;
	if(OSC_PVEC_GETRC(pvec) != 1){
		t_osc_pvec *pv = osc_pvec_assocN(pvec, i, val);
		osc_pvec_release(pvec);
		OSC_PVEC_RETURN_OBJ(pv);
	}
	int shift = osc_pvec_getShift(pvec);
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	void **tail = osc_pvec_getTail(pvec);
	void (*freefn)(void *) = osc_pvec_getFreeFn(pvec);
	{
		t_osc_pvec_ud *ud = osc_pvec_ud_alloc(val);
		if(i >= 0 && i < osc_pvec_length(pvec)){
			P("i(%d) is less than count(%d)", i, osc_pvec_length(pvec));
			if(i >= osc_pvec_tailOffset(pvec)){
				P("%d is in the tail", i);

				void **newtail = tail;
				//void **newtail = (void **)osc_mem_alloc(taillength * sizeof(void *));
				//memcpy(newtail, tail, taillength * sizeof(void *));
				//for(int j = 0; j < taillength; j++){
				//OSC_PVEC_INCRC(newtail[j]);
				//}
				t_osc_pvec_ud *old = newtail[i & OSC_PVEC_MASK];
				if(old){
					osc_pvec_ud_free(old, freefn);
				}
				newtail[i & OSC_PVEC_MASK] = ud;
				
				//t_osc_pvec *ret = _osc_pvec_alloc(count, shift, osc_pvec_node_copy(root), taillength, newtail, freefn);
				//OSC_PVEC_RETURN_OBJ(ret);
				OSC_PVEC_RETURN_OBJ(pvec);
			}
			P("i(%d) is not in the tail", i);
			//t_osc_pvec *pv = _osc_pvec_alloc(count, shift, osc_pvec_doAssoc_m(pvec, shift, osc_pvec_node_copy(root), i, ud), taillength, tail, freefn);
			//OSC_PVEC_RETURN_OBJ(pv);
			osc_pvec_setRoot(pvec, osc_pvec_doAssoc_m(pvec, shift, root, i, ud));
			OSC_PVEC_RETURN_OBJ(pvec);
		}
		if(i == osc_pvec_length(pvec)){
			P("i(%d) == count(%d)", i, osc_pvec_length(pvec));
			t_osc_pvec *pv = osc_pvec_cons_m(pvec, ud);
			OSC_PVEC_RETURN_OBJ(pv);
		}
		P("i(%d) is out of bounds (%d)", i, osc_pvec_length(pvec));
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

// perform the insertion. if level is 0, insert into the newly allocated node's array.
// if level is > 0, the newly allocated node becomes an internal node and we recursively
// call this function to continue creating internal nodes until we get to level 0
t_osc_pvec_node *osc_pvec_doAssoc(t_osc_pvec *pvec, int level, t_osc_pvec_node *node, int i, void *val)
{
	OSC_PVEC_ENTER
	void **clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	memcpy(clone, osc_pvec_node_getArray(node), OSC_PVEC_NBRANCHES * sizeof(void *));
	t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(OSC_PVEC_NBRANCHES, clone);
	P("level = %d", level);
	if(level == 0){
		t_osc_pvec_ud **array = (t_osc_pvec_ud **)osc_pvec_node_getArray(ret);
		t_osc_pvec_ud *ud = array[i & OSC_PVEC_MASK];
		if(ud){
			osc_pvec_ud_free(ud, osc_pvec_getFreeFn(pvec));
		}
		array[i & OSC_PVEC_MASK] = val;
	}else{
		int subidx = (i >> level) & OSC_PVEC_MASK;
		osc_pvec_node_getArray(ret)[subidx] = osc_pvec_doAssoc(pvec, level - OSC_PVEC_NBITS, (t_osc_pvec_node *)osc_pvec_node_getArray(node)[subidx], i, val);
	}
	OSC_PVEC_RETURN_OBJ(ret);
}

t_osc_pvec_node *osc_pvec_doAssoc_m(t_osc_pvec *pvec, int level, t_osc_pvec_node *node, int i, void *val)
{
	OSC_PVEC_ENTER;
	if(OSC_PVEC_GETRC(pvec) != 1){
		t_osc_pvec_node *n = osc_pvec_doAssoc(pvec, level, node, i, val);
		osc_pvec_node_free(node, level, osc_pvec_getFreeFn(pvec));
		OSC_PVEC_RETURN_OBJ(n);
	}
	//void **clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	//memcpy(clone, osc_pvec_node_getArray(node), OSC_PVEC_NBRANCHES * sizeof(void *));
	//t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(OSC_PVEC_NBRANCHES, clone);
	t_osc_pvec_node *ret = node;
	P("level = %d", level);
	if(level == 0){
		t_osc_pvec_ud **array = (t_osc_pvec_ud **)osc_pvec_node_getArray(ret);
		t_osc_pvec_ud *ud = array[i & OSC_PVEC_MASK];
		if(ud){
			osc_pvec_ud_free(ud, osc_pvec_getFreeFn(pvec));
		}
		array[i & OSC_PVEC_MASK] = val;
	}else{
		int subidx = (i >> level) & OSC_PVEC_MASK;
		osc_pvec_node_getArray(ret)[subidx] = osc_pvec_doAssoc_m(pvec, level - OSC_PVEC_NBITS, (t_osc_pvec_node *)osc_pvec_node_getArray(node)[subidx], i, val);
	}
	OSC_PVEC_RETURN_OBJ(ret);
}

// Append val to the end of the list
t_osc_pvec *osc_pvec_cons(t_osc_pvec *pvec, void *val)
{
	OSC_PVEC_ENTER
	if(!pvec){
		OSC_PVEC_RETURN_OBJ(NULL);
	}
	int count = osc_pvec_length(pvec);
	int shift = osc_pvec_getShift(pvec);
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	int taillength = osc_pvec_getTailLength(pvec);
	void **tail = osc_pvec_getTail(pvec);
	void (*freefn)(void *) = osc_pvec_getFreeFn(pvec);
	if(count - osc_pvec_tailOffset(pvec) < OSC_PVEC_NBRANCHES){
		P("there's room in the tail");
		void **newtail = (void **)osc_mem_alloc((taillength + 1) * sizeof(void *));
		if(tail){
			memcpy(newtail, tail, (taillength + 1) * sizeof(void *));
		}else{
			memset(newtail, 0, (taillength + 1) * sizeof(void *));
		}
		for(int i = 0; i < taillength; i++){
			OSC_PVEC_INCRC(newtail[i]);
		}
		P("inserting %p into slot %d", val, taillength);
		newtail[taillength] = val;
		OSC_PVEC_INCRC(root);
	        t_osc_pvec *pv = _osc_pvec_alloc(count + 1, shift, root, taillength + 1, newtail, freefn);
		OSC_PVEC_RETURN_OBJ(pv);
	}
	P("no room in the tail");
	t_osc_pvec_node *newroot;
	void **tailclone = osc_pvec_cloneTail(taillength, tail);
	t_osc_pvec_node *tailnode = osc_pvec_node_allocWithArray(taillength, tailclone);
	osc_mem_free(tailclone);
	int newshift = shift;
	if((count >> OSC_PVEC_NBITS) > (1 << shift)){
		P("no room at this level: construct a new path");
		newroot = osc_pvec_node_alloc();
		OSC_PVEC_INCRC(root);
		osc_pvec_node_getArray(newroot)[0] = root;//osc_pvec_node_copy(root);
		osc_pvec_node_getArray(newroot)[1] = osc_pvec_newPath(shift, tailnode);
		newshift += OSC_PVEC_NBITS;
	}else{
		P("room at the current level: push tail");
		//newroot = osc_pvec_node_copy(osc_pvec_pushTail(pvec, shift, root, tailnode));
		newroot = osc_pvec_pushTail(pvec, shift, root, tailnode);
		OSC_PVEC_INCRC(newroot);
	}
	void **array = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	memset(array, 0, OSC_PVEC_NBRANCHES * sizeof(void *));
	array[0] = val;
	t_osc_pvec *pv = _osc_pvec_alloc(count + 1, newshift, newroot, 1, array, freefn);
	OSC_PVEC_RETURN_OBJ(pv);
}

t_osc_pvec *osc_pvec_cons_m(t_osc_pvec *pvec, void *val)
{
	OSC_PVEC_ENTER;
	if(!pvec){
		OSC_PVEC_RETURN_OBJ(NULL);
	}
	if(OSC_PVEC_GETRC(pvec) != 1){
		t_osc_pvec *pv = osc_pvec_cons(pvec, val);
		osc_pvec_release(pvec);
		OSC_PVEC_RETURN_OBJ(pv);
	}
	int count = osc_pvec_length(pvec);
	int shift = osc_pvec_getShift(pvec);
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	int taillength = osc_pvec_getTailLength(pvec);
	void **tail = osc_pvec_getTail(pvec);
	if(count - osc_pvec_tailOffset(pvec) < OSC_PVEC_NBRANCHES){
		P("there's room in the tail");
		//void **newtail = (void **)osc_mem_alloc((taillength + 1) * sizeof(void *));
		//if(tail){
		//memcpy(newtail, tail, (taillength + 1) * sizeof(void *));
		//}else{
		//memset(newtail, 0, (taillength + 1) * sizeof(void *));
		//}
		//for(int i = 0; i < taillength; i++){
		//OSC_PVEC_INCRC(newtail[i]);
		//}
		void **newtail = tail;
		if(!newtail){
			//newtail = (void **)osc_mem_alloc((taillength + 1) * sizeof(void *));
			newtail = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
			memset(newtail, 0, (taillength + 1) * sizeof(void *));
			osc_pvec_setTail(pvec, newtail);
		}
		P("inserting %p into slot %d", val, taillength);
		newtail[taillength] = val;
	        //t_osc_pvec *pv = _osc_pvec_alloc(count + 1, shift, osc_pvec_node_copy(root), taillength + 1, newtail, freefn);
		osc_pvec_setTailLength(pvec, taillength + 1);
		osc_pvec_setCount(pvec, count + 1);
		OSC_PVEC_RETURN_OBJ(pvec);
	}
	P("no room in the tail");
	t_osc_pvec_node *newroot;
	//void **tailclone = osc_pvec_cloneTail(taillength, tail);
	//t_osc_pvec_node *tailnode = osc_pvec_node_allocWithArray(taillength, tailclone);
	t_osc_pvec_node *tailnode = osc_pvec_node_allocWithArray(taillength, tail);
	//OSC_PVEC_INCRC(tailnode);
	//osc_mem_free(tailclone);
	int newshift = shift;
	if((count >> OSC_PVEC_NBITS) > (1 << shift)){
		P("no room at this level: construct a new path");
		newroot = osc_pvec_node_alloc();
		//osc_pvec_node_getArray(newroot)[0] = osc_pvec_node_copy(root);
		//OSC_PVEC_INCRC(root);
		osc_pvec_node_getArray(newroot)[0] = root;
		osc_pvec_node_getArray(newroot)[1] = osc_pvec_newPath_m(shift, tailnode);
		newshift += OSC_PVEC_NBITS;
	}else{
		P("room at the current level: push tail");
		//newroot = osc_pvec_node_copy(osc_pvec_pushTail(pvec, shift, root, tailnode));
		newroot = osc_pvec_pushTail_m(pvec, shift, root, tailnode);
		//OSC_PVEC_INCRC(newroot);
	}
	void **array = tail;
	if(!array){
		array = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	}
	memset(array, 0, OSC_PVEC_NBRANCHES * sizeof(void *));
	array[0] = val;
	//t_osc_pvec *pv = _osc_pvec_alloc(count + 1, newshift, newroot, 1, array, freefn);
	osc_pvec_setCount(pvec, count + 1);
	osc_pvec_setShift(pvec, newshift);
	osc_pvec_setRoot(pvec, newroot);
	osc_pvec_setTailLength(pvec, 1);
	osc_pvec_setTail(pvec, array);
	OSC_PVEC_RETURN_OBJ(pvec);
}

// create a new tail and construct a new path to the old one
t_osc_pvec_node *osc_pvec_pushTail(t_osc_pvec *pvec, int level, t_osc_pvec_node *parent, t_osc_pvec_node *tailnode)
{
	OSC_PVEC_ENTER
	int subidx = ((osc_pvec_length(pvec) - 1) >> level) & OSC_PVEC_MASK;
	//void *parent_clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	//P("osc_pvec_node_getArray(parent) = %p\n", osc_pvec_node_getArray(parent));
	//memcpy(parent_clone, osc_pvec_node_getArray(parent), OSC_PVEC_NBRANCHES * sizeof(void *));
	//t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(parent_clone);
	//t_osc_pvec_node *ret = osc_pvec_node_clone(parent);
	t_osc_pvec_node *ret = parent;
	t_osc_pvec_node *nodeToInsert = NULL;
	if(level == OSC_PVEC_NBITS){
		nodeToInsert = tailnode;
	}else{
		t_osc_pvec_node *child = (t_osc_pvec_node *)osc_pvec_node_getArray(parent)[subidx];
		if(child == NULL){
			P("no child");
			nodeToInsert = osc_pvec_newPath(level - OSC_PVEC_NBITS, tailnode);
		}else{
			P("have child");
			nodeToInsert = osc_pvec_pushTail(pvec, level - OSC_PVEC_NBITS, child, tailnode);
		}
	}
	osc_pvec_node_getArray(ret)[subidx] = nodeToInsert;//osc_pvec_node_copy(nodeToInsert);
	OSC_PVEC_RETURN_OBJ(ret);
}

t_osc_pvec_node *osc_pvec_pushTail_m(t_osc_pvec *pvec, int level, t_osc_pvec_node *parent, t_osc_pvec_node *tailnode)
{
	OSC_PVEC_ENTER;
	if(OSC_PVEC_GETRC(pvec) != 1){
		t_osc_pvec_node *n = osc_pvec_pushTail(pvec, level, parent, tailnode);
		OSC_PVEC_RETURN_OBJ(n);
	}
	int subidx = ((osc_pvec_length(pvec) - 1) >> level) & OSC_PVEC_MASK;
	//void *parent_clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
	//P("osc_pvec_node_getArray(parent) = %p\n", osc_pvec_node_getArray(parent));
	//memcpy(parent_clone, osc_pvec_node_getArray(parent), OSC_PVEC_NBRANCHES * sizeof(void *));
	//t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(parent_clone);
	t_osc_pvec_node *ret = parent;
	t_osc_pvec_node *nodeToInsert = NULL;
	if(level == OSC_PVEC_NBITS){
		nodeToInsert = tailnode;
	}else{
		t_osc_pvec_node *child = (t_osc_pvec_node *)osc_pvec_node_getArray(parent)[subidx];
		if(child == NULL){
			P("no child");
			nodeToInsert = osc_pvec_newPath_m(level - OSC_PVEC_NBITS, tailnode);
		}else{
			P("have child");
			nodeToInsert = osc_pvec_pushTail_m(pvec, level - OSC_PVEC_NBITS, child, tailnode);
		}
	}
	osc_pvec_node_getArray(ret)[subidx] = nodeToInsert;//osc_pvec_node_copy(nodeToInsert);
	OSC_PVEC_RETURN_OBJ(ret);
}

// construct a new path to node
t_osc_pvec_node *osc_pvec_newPath(int level, t_osc_pvec_node *node)
{
	OSC_PVEC_ENTER
	if(level == 0){
		OSC_PVEC_RETURN_OBJ(node);
	}
	t_osc_pvec_node *ret = osc_pvec_node_alloc();
	osc_pvec_node_getArray(ret)[0] = osc_pvec_newPath(level - OSC_PVEC_NBITS, node);
	OSC_PVEC_RETURN_OBJ(ret);
}

t_osc_pvec_node *osc_pvec_newPath_m(int level, t_osc_pvec_node *node)
{
	OSC_PVEC_ENTER;
	if(OSC_PVEC_GETRC(node) != 1){
		t_osc_pvec_node *n = osc_pvec_newPath(level, node);
		OSC_PVEC_RETURN_OBJ(n);
	}
	if(level == 0){
		OSC_PVEC_RETURN_OBJ(node);
	}
	t_osc_pvec_node *ret = osc_pvec_node_alloc();
	//t_osc_pvec_node *ret = node;
	osc_pvec_node_getArray(ret)[0] = osc_pvec_newPath_m(level - OSC_PVEC_NBITS, node);
	OSC_PVEC_RETURN_OBJ(ret);
}

// remove the last element
t_osc_pvec *osc_pvec_pop(t_osc_pvec *pvec)
{
	OSC_PVEC_ENTER
	int count = osc_pvec_length(pvec);
	int shift = osc_pvec_getShift(pvec);
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	int taillength = osc_pvec_getTailLength(pvec);
	void **tail = osc_pvec_getTail(pvec);
	void (*freefn)(void *) = osc_pvec_getFreeFn(pvec);
	if(count == 0){
		OSC_PVEC_RETURN_OBJ(NULL);
	}
	if(count == 1){
		t_osc_pvec *pv = osc_pvec_alloc(freefn); // osc_pvec_empty
		OSC_PVEC_RETURN_OBJ(pv);
	}
	if(count - osc_pvec_tailOffset(pvec) > 1){
		P("removing element from tail");
		void **newTail = (void **)osc_mem_alloc((taillength - 1) * sizeof(void *));
		memcpy(newTail, tail, (taillength - 1) * sizeof(void *));
		t_osc_pvec *pv = _osc_pvec_alloc(count - 1, shift, root, taillength - 1, newTail, freefn);
		OSC_PVEC_RETURN_OBJ(pv);
	}
	P("removing last element from tail and poping tail");
	void **newTail = osc_pvec_getArrayFor(pvec, taillength - 1);
	t_osc_pvec_node *newroot = osc_pvec_popTail(pvec, shift, root);
	int newshift = shift;
	if(newroot == NULL){
		newroot = osc_pvec_node_alloc();//osc_pvec_node_empty;
	}
	if(shift > OSC_PVEC_NBITS && osc_pvec_node_getArray(newroot)[1] == NULL){
		newroot = (t_osc_pvec_node *)osc_pvec_node_getArray(newroot)[0];
		newshift -= OSC_PVEC_NBITS;
	}
	t_osc_pvec *pv = _osc_pvec_alloc(count - 1, newshift, newroot, taillength - 1, newTail, freefn);
	OSC_PVEC_RETURN_OBJ(pv);
}

// pop the empty tail off
t_osc_pvec_node *osc_pvec_popTail(t_osc_pvec *pvec, int level, t_osc_pvec_node *node)
{
	OSC_PVEC_ENTER
	int subidx = ((osc_pvec_length(pvec) - 2) >> level) & OSC_PVEC_MASK;
	if(level > OSC_PVEC_NBITS){
		P("We're not at the bottom level---keep digging");
		t_osc_pvec_node *newchild = osc_pvec_popTail(pvec, level - OSC_PVEC_NBITS, (t_osc_pvec_node *)osc_pvec_node_getArray(node)[subidx]);
		if(newchild == NULL && subidx == 0){
			OSC_PVEC_RETURN_OBJ(NULL);
		}else{
			void **clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
			memcpy(clone, osc_pvec_node_getArray(node), OSC_PVEC_NBRANCHES * sizeof(void *));
			t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(OSC_PVEC_NBRANCHES, clone);
			osc_pvec_node_getArray(ret)[subidx] = newchild;
			OSC_PVEC_RETURN_OBJ(ret);
		}
	}else if(subidx == 0){
		P("subidx = 0");
		OSC_PVEC_RETURN_OBJ(NULL);
	}else{
		P("else");
		void **clone = (void **)osc_mem_alloc(OSC_PVEC_NBRANCHES * sizeof(void *));
		memcpy(clone, osc_pvec_node_getArray(node), OSC_PVEC_NBRANCHES * sizeof(void *));
		t_osc_pvec_node *ret = osc_pvec_node_allocWithArray(OSC_PVEC_NBRANCHES, clone);
		osc_pvec_node_getArray(ret)[subidx] = NULL;
		OSC_PVEC_RETURN_OBJ(ret);
	}
}

int osc_pvec_length(t_osc_pvec *pvec)
{
	if(pvec){
		return pvec->count;
	}
	return 0;
}

void osc_pvec_setCount(t_osc_pvec *pvec, int count)
{
	if(pvec){
		pvec->count = count;
	}
}

int osc_pvec_getShift(t_osc_pvec *pvec)
{
	if(pvec){
		return pvec->shift;
	}
	return 0;
}

void osc_pvec_setShift(t_osc_pvec *pvec, int shift)
{
	if(pvec){
		pvec->shift = shift;
	}
}

t_osc_pvec_node *osc_pvec_getRoot(t_osc_pvec *pvec)
{
	if(pvec){
		return pvec->root;
	}
	return NULL;
}

void osc_pvec_setRoot(t_osc_pvec *pvec, t_osc_pvec_node *root)
{
	if(pvec){
		pvec->root = root;
	}
}

int osc_pvec_getTailLength(t_osc_pvec *pvec)
{
	if(pvec){
		return pvec->taillength;
	}
	return 0;
}

void osc_pvec_setTailLength(t_osc_pvec *pvec, int taillength)
{
	if(pvec){
		pvec->taillength = taillength;
	}
}

void **osc_pvec_getTail(t_osc_pvec *pvec)
{
	if(pvec){
		return pvec->tail;
	}
	return NULL;
}

void osc_pvec_setTail(t_osc_pvec *pvec, void **tail)
{
	if(pvec){
		pvec->tail = tail;
	}
}

void (*osc_pvec_getFreeFn(t_osc_pvec *pvec))(void *)
{
	if(pvec){
		return pvec->freefn;
	}
	return NULL;
}

void print_node(t_osc_pvec_node *node, int level, int branch)
{
	if(node == NULL){
		return;
	}
	void **array = osc_pvec_node_getArray(node);
	if(level == 0){
		for(int i = 0; i < OSC_PVEC_NBRANCHES; i++){
			t_osc_pvec_ud *ud = (t_osc_pvec_ud *)(array[i]);
			void *data = osc_pvec_ud_getData(ud);
			printf("%*p: %p (%d) (rc : %d)\n", branch * 10, ud, data, (int)(data), OSC_PVEC_GETRC(ud));
		}
	}else{
		for(int i = 0; i < OSC_PVEC_NBRANCHES; i++){
			printf("%*d (%p):\n", branch * 10, i, array[i]);
			print_node(array[i], level - OSC_PVEC_NBITS, branch + 1);
		}
	}
}

void osc_pvec_print(t_osc_pvec *pvec)
{
	int debug = osc_pvec_debug;
	osc_pvec_debug = 0;
	printf("%p: count = %d (rc : %d)\n", pvec, osc_pvec_length(pvec), OSC_PVEC_GETRC(pvec));
	t_osc_pvec_node *root = osc_pvec_getRoot(pvec);
	print_node(root, osc_pvec_getShift(pvec), 0);
	void **tail = osc_pvec_getTail(pvec);
	printf("tail %p:\n", tail);
	for(int i = 0; i < osc_pvec_getTailLength(pvec); i++){
		t_osc_pvec_ud *ud = tail[i];
		void *data = osc_pvec_ud_getData(ud);
		printf("%p: %p (%d) (rc : %d)\n", ud, data, (int)data, OSC_PVEC_GETRC(ud));
	}
	osc_pvec_debug = debug;
}

void myfree(void *obj)
{
	P("%p", obj);
}

t_osc_pvec2 *_osc_pvec2_alloc(t_osc_pvec *head, t_osc_pvec *tail)
{
	t_osc_pvec2 *pvec2 = osc_mem_alloc(sizeof(t_osc_pvec2));
	if(pvec2){
		pvec2->refcount = 1;
		pvec2->length = 0;
		pvec2->head = head;
		pvec2->tail = tail;
	}
	return pvec2;
}

t_osc_pvec2 *osc_pvec2_alloc(void (*freefn)(void *))
{
	OSC_PVEC_ENTER
	OSC_PVEC_RETURN_OBJ(_osc_pvec2_alloc(osc_pvec_alloc(freefn), osc_pvec_alloc(freefn)), osc_pvec_printRefCount);
}

void osc_pvec2_release(t_osc_pvec2 *pvec2)
{
	OSC_PVEC_ENTER
	if(pvec2){
		P("%s: rc = %d", __func__, OSC_PVEC2_GETRC(pvec2));
		if(OSC_PVEC2_GETRC(pvec2) < 0){
			OSC_PVEC_RETURN;
		}
		int rc = OSC_PVEC2_DECRC(pvec2);
		if(rc == 0){
			osc_pvec_release(pvec2->head);
			osc_pvec_release(pvec2->tail);
			memset(pvec2, 0, sizeof(t_osc_pvec2));
			OSC_PVEC2_DECRC(pvec2);
			osc_mem_free(pvec2);
		}
	}
	OSC_PVEC_RETURN;
}

t_osc_pvec2 *osc_pvec2_copy(t_osc_pvec2 *pvec2)
{
	OSC_PVEC_ENTER
	if(pvec2){
		OSC_PVEC2_INCRC(pvec2);
		OSC_PVEC_RETURN_OBJ(pvec2);
	}
	OSC_PVEC_RETURN_OBJ(NULL);
}

t_osc_pvec2 *osc_pvec2_clone(t_osc_pvec2 *pvec2)
{
	OSC_PVEC_ENTER
	OSC_PVEC_RETURN_OBJ(NULL);
}

void *osc_pvec2_nth(t_osc_pvec2 *pvec2, int i)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	int headcount = osc_pvec_length(head);
	t_osc_pvec *tail = pvec2->tail;
	if(i < headcount){
		return osc_pvec_nth(head, headcount - i - 1);
	}else{
		return osc_pvec_nth(tail, i - headcount);
	}
}

t_osc_pvec2 *osc_pvec2_assocN(t_osc_pvec2 *pvec2, int i, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	int headcount = osc_pvec_length(head);
	t_osc_pvec *tail = pvec2->tail;
	int tailcount = osc_pvec_length(tail);
	if(i <= headcount + tailcount){
		if(i < headcount){
			return _osc_pvec2_alloc(osc_pvec_assocN(head, headcount - i - 1, val), osc_pvec_copy(tail));
		}else{
			return _osc_pvec2_alloc(osc_pvec_copy(head), osc_pvec_assocN(tail, i - headcount, val));
		}
	}else{
		return pvec2;
	}
}

t_osc_pvec2 *osc_pvec2_assocN_m(t_osc_pvec2 *pvec2, int i, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	int headcount = osc_pvec_length(head);
	t_osc_pvec *tail = pvec2->tail;
	int tailcount = osc_pvec_length(tail);
	if(i <= headcount + tailcount){
		if(i < headcount){
			osc_pvec_assocN_m(head, headcount - i - 1, val);
			return pvec2;
		}else{
			osc_pvec_assocN_m(tail, i - headcount, val);
			return pvec2;
		}
	}else{
		return pvec2;
	}
}

t_osc_pvec2 *osc_pvec2_append(t_osc_pvec2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	t_osc_pvec *tail = pvec2->tail;
	return _osc_pvec2_alloc(osc_pvec_copy(head), osc_pvec_assocN(tail, osc_pvec_length(tail), val));
}

t_osc_pvec2 *osc_pvec2_append_m(t_osc_pvec2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *tail = pvec2->tail;
	osc_pvec_assocN_m(tail, osc_pvec_length(tail), val);
	return pvec2;
}

t_osc_pvec2 *osc_pvec2_prepend(t_osc_pvec2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	t_osc_pvec *tail = pvec2->tail;
	return _osc_pvec2_alloc(osc_pvec_assocN(head, osc_pvec_length(head), val), osc_pvec_copy(tail));
}

t_osc_pvec2 *osc_pvec2_prepend_m(t_osc_pvec2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	t_osc_pvec *head = pvec2->head;
	osc_pvec_assocN_m(head, osc_pvec_length(head), val);
	return pvec2;
}

t_osc_pvec2 *osc_pvec2_pop(t_osc_pvec2 *pvec2)
{

}

int osc_pvec2_length(t_osc_pvec2 *pvec2)
{
	if(pvec2){
		return osc_pvec_length(pvec2->head) + osc_pvec_length(pvec2->tail);
	}else{
		return 0;
	}
}

/*
int main(int argc, char **argv)
{
	long n = 5;
	t_osc_pvec **pvec = osc_mem_alloc(n * sizeof(t_osc_pvec *));;
	t_osc_pvec *pvec_empty = osc_pvec_alloc(myfree);
	pvec[0] = osc_pvec_assocN(pvec_empty, 0, (void *)0);
	//t_osc_pvec_node *node = osc_pvec_nth(pvec[0], 0);
	for(long i = 1; i < n; i++){
		printf("**************************************************\n");
		pvec[i] = osc_pvec_assocN(pvec[i - 1], i, (void *)i);
	}
	printf("**************************************************\n");
	printf("HERE\n");
	//pvec[n - 1] = osc_pvec_assocN(pvec[n - 1], 0, (void *)10);
	t_osc_pvec *p = pvec[n - 1];
	pvec[n - 1] = osc_pvec_pop(p);
	printf("**************************************************\n");
	printf("FREE\n");
	osc_pvec_release(p);
	print_pvec(pvec[n - 1]);
	print_pvec(pvec[n - 2]);
	//osc_mem_free(pvec);
	for(int i = 0; i < n; i++){
		printf("Freeing %d\n", i);
		osc_pvec_release(pvec[i]);
		printf("**************************************************\n");
	}
	while(1){
		sleep(1);
	}
	return 0;
}
*/
