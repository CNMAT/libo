#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_match.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_bundle_s.h"
#include "osc_message_s.h"

int main(int argc, char **argv)
{
	//char *pattern = "/*cd{e,ee,eee}ef/{bar,car}/barf";
	//char *address = "/abcdcdeeef/bar";
	char *pattern = "/a*{cc}d";
	char *address = "/abccd";
	int po = 0, ao = 0;
	int ret = osc_match(pattern, address, &po, &ao);
	printf("ret = %d, po = %d, ao = %d\n", ret, po, ao);
	printf("%s %s\n", pattern + po, address + ao);


	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_msg_u *m1 = osc_message_u_alloc();
	t_osc_msg_u *m2 = osc_message_u_alloc();
	osc_message_u_setAddress(m1, "/foo/bar");
	osc_message_u_setAddress(m2, "/foo/car");
	osc_message_u_appendInt32(m1, 10);
	osc_message_u_appendInt32(m2, 20);

	osc_bundle_u_addMsg(b, m1);
	osc_bundle_u_addMsg(b, m2);

	long len = 0;
	char *ptr = NULL;
	osc_bundle_u_serialize(b, &len, &ptr);

	t_osc_err e = osc_bundle_s_removeMessage("/foo/c*", &len, ptr, 1);
	if(e){
		printf("e = %d\n", e);
	}
	long slen = 0;
	slen = osc_bundle_s_nformat(NULL, 0, len, ptr, 0);
	printf("slen = %ld\n", slen);
	char sptr[slen + 1];
	osc_bundle_s_nformat(sptr, slen + 1, len, ptr, 0);
	printf("%s\n", sptr);
}

