#define OSC_OBJ_INVALID 666

typedef struct _osc_obj
{
	int refcount;
	void (*freefn)(void *);
} t_osc_obj;

