#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_match.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_bundle_s.h"
#include "osc_message_s.h"
#include "osc_bundle_iterator_u.h"
#include "osc_bundle_iterator_s.h"
#include "osc_message_iterator_u.h"
#include "osc_parser.h"

int main(int argc, char **argv)
{
	// create a bundle and add messages to it
	t_osc_bndl_u *bndl_u = osc_bundle_u_alloc();
	t_osc_msg_u *m1 = osc_message_u_alloc();
	osc_message_u_setAddress(m1, "/foo");
	osc_message_u_appendFloat(m1, 3.14);
	osc_bundle_u_addMsg(bndl_u, m1);

	t_osc_msg_u *m2 = osc_message_u_allocWithString("/bar", "whatevs");
	osc_bundle_u_addMsg(bndl_u, m2);

	t_osc_msg_u *m3 = osc_message_u_allocWithAddress("/bloo");
	t_osc_atom_u *a = osc_atom_u_allocWithInt32(12);
	osc_message_u_appendAtom(m3, a);
	osc_bundle_u_addMsg(bndl_u, m3);

	// serialize the bundle
	long len = osc_bundle_u_nserialize(NULL, 0, bndl_u);
	char bndl_s[len];
	osc_bundle_u_nserialize(bndl_s, len, bndl_u);

	// free the original unserialized bundle
	osc_bundle_u_free(bndl_u);
	bndl_u = NULL;

	// deserialize the serialized bundle
	osc_bundle_s_deserialize(len, bndl_s, &bndl_u);
	
	// iterate over messages in a serialized bundle
	t_osc_bndl_it_s *b_it_s = osc_bndl_it_s_get(len, bndl_s);
	while(osc_bndl_it_s_hasNext(b_it_s)){
		t_osc_msg_s *m = osc_bndl_it_s_next(b_it_s);
		printf("%s\n", osc_message_s_getAddress(m));
	}
	osc_bndl_it_s_destroy(b_it_s);

	// turn a serialized bundle into printable text
	long tlen = osc_bundle_s_nformat(NULL, 0, len, bndl_s, 0);
	char text[tlen + 1];
	osc_bundle_s_nformat(text, tlen, len, bndl_s, 0);
	printf("\nBUNDLE:\n");
	printf("%s\n", text);
	printf("\n");

	// turn text into an unserialized bundle
	t_osc_bndl_u *bndl_u_2 = NULL;
	char *text2 = "/jean : [1, 2, 3], /john : 6.66, /jeremy : \"is cool\"";
	osc_parser_parseString(strlen(text2), text2, &bndl_u_2);

	// iterate over messages in an unserialized bundle
	t_osc_bndl_it_u *b_it_u = osc_bndl_it_u_get(bndl_u_2);
	while(osc_bndl_it_u_hasNext(b_it_u)){
		t_osc_msg_u *m = osc_bndl_it_u_next(b_it_u);
		printf("%s has typetags ", osc_message_u_getAddress(m));
		// iterate over atoms in list
		t_osc_msg_it_u *m_it_u = osc_msg_it_u_get(m);
		while(osc_msg_it_u_hasNext(m_it_u)){
			t_osc_atom_u *a = osc_msg_it_u_next(m_it_u);
			printf("%c", osc_atom_u_getTypetag(a));
		}
		osc_msg_it_u_destroy(m_it_u);
		printf("\n");
	}
	osc_bndl_it_u_destroy(b_it_u);
}

