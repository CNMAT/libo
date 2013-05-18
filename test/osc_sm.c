#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_byteorder.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_serial.h"

int main(int argc, char **argv)
{
	/*
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_msg_u *m = osc_message_u_alloc();
	osc_message_u_setAddress(m, "/fo{}");
	osc_message_u_appendInt32(m, 20);
	osc_message_u_appendFloat(m, 44.1);
	osc_message_u_appendInt32(m, 666);
	osc_bundle_u_addMsg(b, m);

	m = osc_message_u_alloc();
	osc_message_u_setAddress(m, "/bar?");
	osc_message_u_appendFloat(m, 66.6);
	osc_bundle_u_addMsg(b, m);
	long len = 0;
	char *bundle = NULL;
	osc_bundle_u_serialize(b, &len, &bundle);
	*/
	char *buf = "/foo\0\0\0\0,ifi\0\0\0\0n\0\0\0arst8\0\0\0";
	int len = 28;
	char *bundle = buf;
	uint64_t state = OSC_SERIAL_INIT;
	for(int i = 0; i < len; i++){
		printf("state: 0x%llx (count = %lld)\n", state, state & 0xffffffff);
		printf("processing byte %d (%c, 0x%x)\n", i, bundle[i], bundle[i]);
		state = osc_serial_processByte(bundle[i], state);
		if(state & 0x000000ff00000000){
			printf("error:(byte %d): %s\n", i, osc_serial_errstr(state));
			return 0;
		}else{
			//printf("state: 0x%llx\n", state);
		}
	}
	return 0;
}
