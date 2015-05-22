#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_timetag.h"
#include "osc_util.h"
#define __OSC_PROFILE__
#include "osc_profile.h"
#include "osc_pvec.h"
#include "osc_mem.h"

#include "osc_bundle_u.h"
#include "osc_bundle_s.h"
#include "osc_message_u.h"
#include "osc_builtin.h"

#include "osc_parse.h"

typedef struct _osc_test_ret
{
	int numfaults;
} t_osc_test_ret;

char dots[32] = "................................";
#define PRINT_TEST(fmt, ...) printf("TEST: "fmt"%*.*s", __VA_ARGS__, 0, 32, dots)
#define PRINT_PASSED printf("passed\n");
#define PRINT_FAILED(fmt, ...) faultcount++; printf("FAILED: "fmt"\n", __VA_ARGS__)
#define PRINT_TEST_HEADER(testname) printf("**************************************************\n* TESTING %s\n**************************************************\n", testname);

t_osc_test_ret osc_atom_test(void);

#define SLEEP while(1) sleep(1);

int __main(int argc, char **argv)
{
	t_osc_pvec2 *pv = osc_pvec2_alloc(NULL);
	for(int i = 0; i < 5; i++){
		osc_pvec2_append_m(pv, (void *)i);
	}
	for(int i = 6; i < 10; i++){
		osc_pvec2_prepend_m(pv, (void *)i);
	}

	for(int i = 0; i < osc_pvec2_length(pv); i++){
		printf("%d: %p\n", i, osc_pvec2_nth(pv, i));
	}

	osc_pvec2_assocN_m(pv, 2, (void *)0xa);
	osc_pvec2_assocN_m(pv, 8, (void *)0xb);
	for(int i = 0; i < osc_pvec2_length(pv); i++){
		printf("%d: %p\n", i, osc_pvec2_nth(pv, i));
	}
	SLEEP;
	return 0;
}

int _main(int argc, char **argv)
{
	t_osc_msg *m = NULL;
	t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 2,
				       osc_msg_alloc(osc_atom_allocSymbol("/foo", 0), 0),
				       m = osc_msg_alloc(osc_atom_allocSymbol("/bar", 0), 2, osc_atom_allocFloat(2.0), osc_atom_allocInt32(10)));
	b = osc_bndl_append(b, osc_msg_alloc(osc_atom_allocSymbol("/bloo", 0), 2, osc_atom_allocFloat(2.0), osc_atom_allocInt32(10)));
	//osc_msg_assocn_m((t_osc_msg_m *)m, osc_atom_allocInt32(110), osc_msg_length(m));
	osc_msg_assocn(m, osc_atom_allocInt32(110), osc_msg_length(m));
	b = osc_bndl_format_m((t_osc_bndl_m *)b, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(b));


	char *str = "{ /a : 10, /b : /a }";
	t_osc_bndl *bb = osc_parse(str);
	osc_bndl_format_m((t_osc_bndl_m *)bb, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bb));
	t_osc_bndl *bbb = osc_bndl_eval(bb, NULL);
	osc_bndl_format_m((t_osc_bndl_m *)bbb, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bbb));

	return 0;
}

int main(int argc, char **argv)
{
	goto foo;
	//osc_typetag_formatTypeLattice_dot();
	t_osc_bndl *bndl1 = osc_bndl_alloc(OSC_TIMETAG_NULL, 2, osc_msg_alloc(osc_atom_allocString("/foo", 0), 1, osc_atom_allocInt32(10)), osc_msg_alloc(osc_atom_allocString("/bar", 0), 1, osc_atom_allocFloat(3.14159)));
	t_osc_bndl *bndl2 = osc_bndl_alloc(OSC_TIMETAG_NULL, 2, osc_msg_alloc(osc_atom_allocString("/fo?", 0), 1, osc_atom_allocInt32(12)), osc_msg_alloc(osc_atom_allocString("/bar", 0), 1, osc_atom_allocFloat(6.28)));

	t_osc_bndl *bndl3 = osc_bndl_union(bndl1, bndl2);
	t_osc_bndl *bndl4 = osc_bndl_intersect(bndl1, bndl2);
	t_osc_bndl *bndl5 = osc_bndl_rcompliment(bndl1, bndl2);
	osc_bndl_format_m(bndl3, 0);
	osc_bndl_format_m(bndl4, 0);
	osc_bndl_format_m(bndl5, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bndl3));
	printf("%s\n", osc_bndl_getPrettyPtr(bndl4));
	printf("%s\n", osc_bndl_getPrettyPtr(bndl5));
	osc_bndl_release(bndl1);
	osc_bndl_release(bndl2);
	osc_bndl_release(bndl3);
	osc_bndl_release(bndl4);
	osc_bndl_release(bndl5);

	t_osc_atom *a1, *a2;
	t_osc_atom *a = osc_atom_nth(a1 = osc_atom_allocString("foo", 0), a2 = osc_atom_allocInt32(0));
	osc_atom_format_m(a, 0);
	printf("%s\n", osc_atom_getPrettyPtr(a));
	osc_atom_release(a);
	osc_atom_release(a1);
	osc_atom_release(a2);

	osc_atom_test();

	int n = 1000;
	/*
	OSC_PROFILE_TIMER_START(t0);
	t_osc_pvec *pvec = osc_pvec_alloc(NULL);
	for(int i = 0; i < n; i++){
		//printf("%d\n", i);
		//OSC_PROFILE_TIMER_START(t2);
		//t_osc_pvec *pvec2 = osc_pvec_assocN(pvec, i, (void *)i);
		pvec = osc_pvec_assocN_m(pvec, i, (void *)i);
		printf("**************************************************\n");
		osc_pvec_print(pvec);
		printf("**************************************************\n");
		//OSC_PROFILE_TIMER_STOP(t2);
		//OSC_PROFILE_TIMER_PRINTF(t2);
		//osc_pvec_free(pvec);
		//pvec = pvec2;
	}
	//printf("**************************************************\n");
	//for(int i = 0; i < n + 1; i++){
	//osc_pvec_print(pvec);
	//}
	osc_pvec_free(pvec);
	//printf("**************************************************\n");
	OSC_PROFILE_TIMER_STOP(t0);
	OSC_PROFILE_TIMER_PRINTF(t0);
	*/
	//t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_allocString(osc_util_strcpy("/foo")), 1, osc_atom_allocString(osc_util_strcpy("bar"))), 0);

	OSC_PROFILE_TIMER_START(t1);
	t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	for(int i = 0; i < n; i++){
		//printf("i = %d\n", i);
		char address[32];
		sprintf(address, "/%d", i);
		t_osc_msg *m = osc_msg_alloc(osc_atom_allocSymbol(address, 0), 1, osc_atom_allocInt32(i));
		//OSC_PROFILE_TIMER_START(t1);
		b = osc_bndl_assocn(b, m, i);
		//OSC_PROFILE_TIMER_STOP(t1);
		//OSC_PROFILE_TIMER_PRINTF(t1);
	}
	printf("**************************************************\n");
	b = osc_bndl_serialize_m(b);
	b = osc_bndl_format_m(b, 0);
	/*
	char *ptr = osc_bndl_getSerializedPtr(b);
	for(int i = 0; i < osc_bndl_getSerializedLen(b); i++){
		printf("%d: %c %d\n", i, ptr[i], ptr[i]);
	}
	printf("%s\n", osc_bndl_getPrettyPtr(b));
	*/
	osc_bndl_free(b);
	printf("**************************************************\n");
	OSC_PROFILE_TIMER_STOP(t1);
	OSC_PROFILE_TIMER_PRINTF(t1);

	OSC_PROFILE_TIMER_START(t2);
	t_osc_bndl_u *bu = osc_bundle_u_alloc();
	for(int i = 0; i < n; i++){
		char address[32];
		sprintf(address, "/%d", i);
		t_osc_msg_u *m = osc_message_u_alloc();
		osc_message_u_setAddress(m, address);
		osc_message_u_appendInt32(m, i);
		osc_bundle_u_addMsg(bu, m);
	}
	//t_osc_bndl_s *bs = osc_bundle_u_serialize(bu);
	//osc_bundle_s_deepFree(bs);
	osc_bundle_u_free(bu);
	OSC_PROFILE_TIMER_STOP(t2);
	OSC_PROFILE_TIMER_PRINTF(t2);

	{
		t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 2,
					       osc_msg_alloc(osc_atom_allocSymbol("/a", 0), 1, osc_atom_allocInt32(1)),
					       osc_msg_alloc(osc_atom_allocSymbol("/b", 0), 1, osc_atom_allocSymbol("/a", 0)));
		t_osc_bndl *bb = osc_bndl_eval(b, NULL);
		osc_bndl_format_m(bb, 0);
		printf("%s\n", osc_bndl_getPrettyPtr(bb));
		osc_bndl_release(b);
		osc_bndl_release(bb);
	}
 foo:
	{
		t_osc_bndl *b = osc_parse("{/foo : 10, /bar : /foo, /bloo, /blurp : add {/lhs : 1, /rhs : 2}}");
		//t_osc_bndl *b = osc_parse("{/foo : 10, /bar : /foo, /bloo}");
		osc_bndl_format_m(b, 0);
		printf("parsed bndl:\n%s\n", osc_bndl_getPrettyPtr(b));
		t_osc_bndl *bb = osc_bndl_eval(b, NULL);
		osc_bndl_format_m(bb, 0);
		printf("eval'd bndl:\n%s\n", osc_bndl_getPrettyPtr(bb));
		osc_bndl_release(bb);
		osc_bndl_release(b);
		printf("math string:\n%s\n", osc_builtin_math);
		t_osc_bndl *math = osc_parse(osc_builtin_math);
		osc_bndl_format_m(math, 0);
		printf("parsed math:\n%s\n", osc_bndl_getPrettyPtr(math));
	}
	SLEEP;
	//int slen1 = osc_bndl_getSerializedLen(b);
	//char *sptr1 = osc_bndl_getSerializedPtr(b);
	//for(int i = 0; i < slen1; i++){
	//printf("%d: %c %d\n", i, sptr1[i], sptr1[i]);
	//}
	return 0;
}

t_osc_test_ret osc_atom_test(void)
{
	int faultcount = 0;
	srand(time(NULL));
	int8_t c = (int8_t)rand();
	uint8_t C = (uint8_t)rand();
	int16_t u = (int16_t)rand();
	uint16_t U = (uint16_t)rand();
	int32_t i = (int32_t)rand();
	uint32_t I = (uint32_t)rand();
	int64_t h = (int64_t)rand();
	uint64_t H = (uint64_t)rand();
	float f = rand() / (float)RAND_MAX;
	double d = rand() / (double)RAND_MAX;
	char *s = "foo";
	t_osc_bndl *B = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_allocString("/foo", 0), 1, osc_atom_allocInt32(10)));
	t_osc_timetag t = osc_timetag_floatToTimetag(rand() / (double)RAND_MAX);
	char b[8];
	*((int32_t *)b) = hton32(4);
	*((int32_t *)(b + 4)) = hton32((int32_t)rand());

	t_osc_atom *ac = osc_atom_allocInt8(c);
	t_osc_atom *aC = osc_atom_allocUInt8(C);
	t_osc_atom *au = osc_atom_allocInt16(u);
	t_osc_atom *aU = osc_atom_allocUInt16(U);
	t_osc_atom *ai = osc_atom_allocInt32(i);
	t_osc_atom *aI = osc_atom_allocUInt32(I);
	t_osc_atom *ah = osc_atom_allocInt64(h);
	t_osc_atom *aH = osc_atom_allocUInt64(H);
	t_osc_atom *af = osc_atom_allocFloat(f);
	t_osc_atom *ad = osc_atom_allocDouble(d);
	t_osc_atom *as = osc_atom_allocString(s, 0);
	t_osc_atom *aB = osc_atom_allocBndl(B, 1);
	t_osc_atom *at = osc_atom_allocTimetag(t);
	t_osc_atom *ab = osc_atom_allocBlob(b, 0);
	t_osc_atom *aT = osc_atom_true;
	t_osc_atom *aF = osc_atom_false;
	t_osc_atom *aN = osc_atom_nil;

	t_osc_atom *atoms[17] = {ac, aC, au, aU, ai, aI, ah, aH, af, ad, as, aB, at, ab, aT, aF, aN};
	char typetags[] = {'c', 'C', 'u', 'U', 'i', 'I', 'h', 'H', 'f', 'd', 's', OSC_BUNDLE_TYPETAG, 't', 'b', 'T', 'F', 'N'};
	char **blobs = (char **)osc_mem_alloc(17 * sizeof(char*));

	for(int i = 0; i < sizeof(atoms) / sizeof(t_osc_atom*); i++){
		osc_atom_format_m((t_osc_atom_m *)atoms[i], 0);
		osc_atom_serialize_m((t_osc_atom_m *)atoms[i]);
		blobs[i] = osc_mem_alloc(osc_atom_getSerializedLen(atoms[i]) + 4);
		*((int32_t *)blobs[i]) = hton32(osc_atom_getSerializedLen(atoms[i]));
		memcpy(blobs[i] + 4, osc_atom_getSerializedPtr(atoms[i]), osc_atom_getSerializedLen(atoms[i]));
	}
	{
		PRINT_TEST_HEADER("serialization");
		// expected results
		int32_t sc = hton32(((int32_t)c) & 0xFF);
		uint32_t sC = hton32(((uint32_t)C) & 0xFF);
		int32_t su = hton32(((int32_t)u) & 0xFFFF);
		uint32_t sU = hton32(((uint32_t)U) & 0xFFFF);
		int32_t si = hton32(i);
		uint32_t sI = hton32(I);
		int64_t sh = hton64(h);
		uint64_t sH = hton64(H);
		int32_t sf = hton32(*((int32_t *)&f));
		int64_t sd = hton64(*((int64_t *)&d));
		char ss[4] = {'f', 'o', 'o', '\0'};
		char sB[36] = {'#', 'b', 'u', 'n', 'd', 'l', 'e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, '/', 'f', 'o', 'o', 0, 0, 0, 0, ',', 'i', 0, 0, 0, 0, 0, 10};
		char st[8];
		osc_timetag_encodeForHeader(t, st);
		char sb[8];
		memcpy(sb, b, 8);

		struct _serialize_test
		{
			t_osc_atom *a;
			int resultlen;
			char *result;
			char *string;
		} serialize_test[] = {
			{ac, 4, (char *)&sc, "'c'"},
			{aC, 4, (char *)&sC, "'C'"},
			{au, 4, (char *)&su, "'u'"},
			{aU, 4, (char *)&sU, "'U'"},
			{ai, 4, (char *)&si, "'i'"},
			{aI, 4, (char *)&sI, "'I'"},
			{ah, 8, (char *)&sh, "'h'"},
			{aH, 8, (char *)&sH, "'H'"},
			{af, 4, (char *)&sf, "'f'"},
			{ad, 8, (char *)&sd, "'d'"},
			{as, 4, ss, "'s'"},
			{aB, 36, sB, "'B'"},
			{at, 8, st, "'t'"},
			{ab, 8, sb, "'b'"},
		};
		for(int i = 0; i < sizeof(serialize_test) / sizeof(struct _serialize_test); i++){
			struct _serialize_test t = serialize_test[i];
			t_osc_atom *a = osc_atom_serialize(t.a);
			int fail = 0;
			PRINT_TEST("serialize %s atom", t.string);
			for(int i = 0; i < t.resultlen; i++){
				if(osc_atom_getSerializedPtr(a)[i] != t.result[i]){
					fail++;
				}
			}
			if(fail){
				if(t.resultlen == 4){
					PRINT_FAILED("Expected 0x%"PRIX32" but got 0x%"PRIX32, *((uint32_t *)t.result), *((uint32_t *)osc_atom_getSerializedPtr(a)));
				}else if(t.resultlen == 8){
					PRINT_FAILED("Expected 0x%"PRIX64" but got 0x%"PRIX64, *((uint64_t *)t.result), *((uint64_t *)osc_atom_getSerializedPtr(a)));
				}else{
					PRINT_FAILED("%s", "");
				}
			}else{
				PRINT_PASSED;
			}
			osc_atom_release(a);
		}
	}

	{
		PRINT_TEST_HEADER("conversion");
		struct _conv_test
		{
			t_osc_atom *atom;
			char ttfrom;
			char ttto;
			t_osc_atom *expected;
		} conv_test[] = {
			{ac, 'c', 'c', osc_atom_allocInt8((int8_t)c)},
			{ac, 'c', 'C', osc_atom_allocUInt8((uint8_t)c)},
			{ac, 'c', 'u', osc_atom_allocInt16((int16_t)c)},
			{ac, 'c', 'U', osc_atom_allocUInt16((uint16_t)c)},
			{ac, 'c', 'i', osc_atom_allocInt32((int32_t)c)},
			{ac, 'c', 'I', osc_atom_allocUInt32((uint32_t)c)},
			{ac, 'c', 'h', osc_atom_allocInt64((int64_t)c)},
			{ac, 'c', 'H', osc_atom_allocUInt64((uint64_t)c)},
			{ac, 'c', 'f', osc_atom_allocFloat((float)c)},
			{ac, 'c', 'd', osc_atom_allocDouble((double)c)},
			{ac, 'c', 's', osc_atom_allocString(osc_atom_getPrettyPtr(ac), 0)},
			{ac, 'c', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(c))), 1)},
			{ac, 'c', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)c))},
			{ac, 'c', 'b', osc_atom_allocBlob(blobs[0], 0)},
			{ac, 'c', 'T', osc_atom_true},
			{ac, 'c', 'F', osc_atom_false},
			{ac, 'c', 'N', osc_atom_nil},

			{aC, 'C', 'c', osc_atom_allocInt8((int8_t)C)},
			{aC, 'C', 'C', osc_atom_allocUInt8((uint8_t)C)},
			{aC, 'C', 'u', osc_atom_allocInt16((int16_t)C)},
			{aC, 'C', 'U', osc_atom_allocUInt16((uint16_t)C)},
			{aC, 'C', 'i', osc_atom_allocInt32((int32_t)C)},
			{aC, 'C', 'I', osc_atom_allocUInt32((uint32_t)C)},
			{aC, 'C', 'h', osc_atom_allocInt64((int64_t)C)},
			{aC, 'C', 'H', osc_atom_allocUInt64((uint64_t)C)},
			{aC, 'C', 'f', osc_atom_allocFloat((float)C)},
			{aC, 'C', 'd', osc_atom_allocDouble((double)C)},
			{aC, 'C', 's', osc_atom_allocString(osc_atom_getPrettyPtr(aC), 0)},
			{aC, 'C', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocUInt8(C))), 1)},
			{aC, 'C', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)C))},
			{aC, 'C', 'b', osc_atom_allocBlob(blobs[1], 0)},
			{aC, 'C', 'T', osc_atom_true},
			{aC, 'C', 'F', osc_atom_false},
			{aC, 'C', 'N', osc_atom_nil},

			{au, 'u', 'c', osc_atom_allocInt8((int8_t)u)},
			{au, 'u', 'C', osc_atom_allocUInt8((uint8_t)u)},
			{au, 'u', 'u', osc_atom_allocInt16((int16_t)u)},
			{au, 'u', 'U', osc_atom_allocUInt16((uint16_t)u)},
			{au, 'u', 'i', osc_atom_allocInt32((int32_t)u)},
			{au, 'u', 'I', osc_atom_allocUInt32((uint32_t)u)},
			{au, 'u', 'h', osc_atom_allocInt64((int64_t)u)},
			{au, 'u', 'H', osc_atom_allocUInt64((uint64_t)u)},
			{au, 'u', 'f', osc_atom_allocFloat((float)u)},
			{au, 'u', 'd', osc_atom_allocDouble((double)u)},
			{au, 'u', 's', osc_atom_allocString(osc_atom_getPrettyPtr(au), 0)},
			{au, 'u', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(u))), 1)},
			{au, 'u', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)u))},
			 {au, 'u', 'b', osc_atom_allocBlob(blobs[2], 0)},
			{au, 'u', 'T', osc_atom_true},
			{au, 'u', 'F', osc_atom_false},
			{au, 'u', 'N', osc_atom_nil},

			{aU, 'U', 'c', osc_atom_allocInt8((int8_t)U)},
			{aU, 'U', 'C', osc_atom_allocUInt8((uint8_t)U)},
			{aU, 'U', 'u', osc_atom_allocInt16((int16_t)U)},
			{aU, 'U', 'U', osc_atom_allocUInt16((uint16_t)U)},
			{aU, 'U', 'i', osc_atom_allocInt32((int32_t)U)},
			{aU, 'U', 'I', osc_atom_allocUInt32((uint32_t)U)},
			{aU, 'U', 'h', osc_atom_allocInt64((int64_t)U)},
			{aU, 'U', 'H', osc_atom_allocUInt64((uint64_t)U)},
			{aU, 'U', 'f', osc_atom_allocFloat((float)U)},
			{aU, 'U', 'd', osc_atom_allocDouble((double)U)},
			{aU, 'U', 's', osc_atom_allocString(osc_atom_getPrettyPtr(aU), 0)},
			{aU, 'U', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(U))), 1)},
			{aU, 'U', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)U))},
			{aU, 'U', 'b', osc_atom_allocBlob(blobs[3], 0)},
			{aU, 'U', 'T', osc_atom_true},
			{aU, 'U', 'F', osc_atom_false},
			{aU, 'U', 'N', osc_atom_nil},

			{ai, 'i', 'c', osc_atom_allocInt8((int8_t)i)},
			{ai, 'i', 'C', osc_atom_allocUInt8((uint8_t)i)},
			{ai, 'i', 'u', osc_atom_allocInt16((int16_t)i)},
			{ai, 'i', 'U', osc_atom_allocUInt16((uint16_t)i)},
			{ai, 'i', 'i', osc_atom_allocInt32((int32_t)i)},
			{ai, 'i', 'I', osc_atom_allocUInt32((uint32_t)i)},
			{ai, 'i', 'h', osc_atom_allocInt64((int64_t)i)},
			{ai, 'i', 'H', osc_atom_allocUInt64((uint64_t)i)},
			{ai, 'i', 'f', osc_atom_allocFloat((float)i)},
			{ai, 'i', 'd', osc_atom_allocDouble((double)i)},
			{ai, 'i', 's', osc_atom_allocString(osc_atom_getPrettyPtr(ai), 0)},
			{ai, 'i', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(i))), 1)},
			{ai, 'i', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)i))},
			{ai, 'i', 'b', osc_atom_allocBlob(blobs[4], 0)},
			{ai, 'i', 'T', osc_atom_true},
			{ai, 'i', 'F', osc_atom_false},
			{ai, 'i', 'N', osc_atom_nil},

			{aI, 'I', 'c', osc_atom_allocInt8((int8_t)I)},
			{aI, 'I', 'C', osc_atom_allocUInt8((uint8_t)I)},
			{aI, 'I', 'u', osc_atom_allocInt16((int16_t)I)},
			{aI, 'I', 'U', osc_atom_allocUInt16((uint16_t)I)},
			{aI, 'I', 'i', osc_atom_allocInt32((int32_t)I)},
			{aI, 'I', 'I', osc_atom_allocUInt32((uint32_t)I)},
			{aI, 'I', 'h', osc_atom_allocInt64((int64_t)I)},
			{aI, 'I', 'H', osc_atom_allocUInt64((uint64_t)I)},
			{aI, 'I', 'f', osc_atom_allocFloat((float)I)},
			{aI, 'I', 'd', osc_atom_allocDouble((double)I)},
			{aI, 'I', 's', osc_atom_allocString(osc_atom_getPrettyPtr(aI), 0)},
			{aI, 'I', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(I))), 1)},
			{aI, 'I', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)I))},
			{aI, 'I', 'b', osc_atom_allocBlob(blobs[5], 0)},
			{aI, 'I', 'T', osc_atom_true},
			{aI, 'I', 'F', osc_atom_false},
			{aI, 'I', 'N', osc_atom_nil},

		        {ah, 'h', 'c', osc_atom_allocInt8((int8_t)h)},
			{ah, 'h', 'C', osc_atom_allocUInt8((uint8_t)h)},
			{ah, 'h', 'u', osc_atom_allocInt16((int16_t)h)},
			{ah, 'h', 'U', osc_atom_allocUInt16((uint16_t)h)},
			{ah, 'h', 'i', osc_atom_allocInt32((int32_t)h)},
			{ah, 'h', 'I', osc_atom_allocUInt32((uint32_t)h)},
			{ah, 'h', 'h', osc_atom_allocInt64((int64_t)h)},
			{ah, 'h', 'H', osc_atom_allocUInt64((uint64_t)h)},
			{ah, 'h', 'f', osc_atom_allocFloat((float)h)},
			{ah, 'h', 'd', osc_atom_allocDouble((double)h)},
			{ah, 'h', 's', osc_atom_allocString(osc_atom_getPrettyPtr(ah), 0)},
			{ah, 'h', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(h))), 1)},
			{ah, 'h', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)h))},
			{ah, 'h', 'b', osc_atom_allocBlob(blobs[6], 0)},
			{ah, 'h', 'T', osc_atom_true},
			{ah, 'h', 'F', osc_atom_false},
			{ah, 'h', 'N', osc_atom_nil},

			{aH, 'H', 'c', osc_atom_allocInt8((int8_t)H)},
			{aH, 'H', 'C', osc_atom_allocUInt8((uint8_t)H)},
			{aH, 'H', 'u', osc_atom_allocInt16((int16_t)H)},
			{aH, 'H', 'U', osc_atom_allocUInt16((uint16_t)H)},
			{aH, 'H', 'i', osc_atom_allocInt32((int32_t)H)},
			{aH, 'H', 'I', osc_atom_allocUInt32((uint32_t)H)},
			{aH, 'H', 'h', osc_atom_allocInt64((int64_t)H)},
			{aH, 'H', 'H', osc_atom_allocUInt64((uint64_t)H)},
			{aH, 'H', 'f', osc_atom_allocFloat((float)H)},
			{aH, 'H', 'd', osc_atom_allocDouble((double)H)},
			{aH, 'H', 's', osc_atom_allocString(osc_atom_getPrettyPtr(aH), 0)},
			{aH, 'H', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(H))), 1)},
			{aH, 'H', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)H))},
			{aH, 'H', 'b', osc_atom_allocBlob(blobs[7], 0)},
			{aH, 'H', 'T', osc_atom_true},
			{aH, 'H', 'F', osc_atom_false},
			{aH, 'H', 'N', osc_atom_nil},

			{af, 'f', 'c', osc_atom_allocInt8((int8_t)f)},
			{af, 'f', 'C', osc_atom_allocUInt8((uint8_t)f)},
			{af, 'f', 'u', osc_atom_allocInt16((int16_t)f)},
			{af, 'f', 'U', osc_atom_allocUInt16((uint16_t)f)},
			{af, 'f', 'i', osc_atom_allocInt32((int32_t)f)},
			{af, 'f', 'I', osc_atom_allocUInt32((uint32_t)f)},
			{af, 'f', 'h', osc_atom_allocInt64((int64_t)f)},
			{af, 'f', 'H', osc_atom_allocUInt64((uint64_t)f)},
			{af, 'f', 'f', osc_atom_allocFloat((float)f)},
			{af, 'f', 'd', osc_atom_allocDouble((double)f)},
			{af, 'f', 's', osc_atom_allocString(osc_atom_getPrettyPtr(af), 0)},
			{af, 'f', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(f))), 1)},
			{af, 'f', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)f))},
			{af, 'f', 'b', osc_atom_allocBlob(blobs[8], 0)},
			{af, 'f', 'T', osc_atom_true},
			{af, 'f', 'F', osc_atom_false},
			{af, 'f', 'N', osc_atom_nil},

			{ad, 'd', 'c', osc_atom_allocInt8((int8_t)d)},
			{ad, 'd', 'C', osc_atom_allocUInt8((uint8_t)d)},
			{ad, 'd', 'u', osc_atom_allocInt16((int16_t)d)},
			{ad, 'd', 'U', osc_atom_allocUInt16((uint16_t)d)},
			{ad, 'd', 'i', osc_atom_allocInt32((int32_t)d)},
			{ad, 'd', 'I', osc_atom_allocUInt32((uint32_t)d)},
			{ad, 'd', 'h', osc_atom_allocInt64((int64_t)d)},
			{ad, 'd', 'H', osc_atom_allocUInt64((uint64_t)d)},
			{ad, 'd', 'f', osc_atom_allocFloat((float)d)},
			{ad, 'd', 'd', osc_atom_allocDouble((double)d)},
			{ad, 'd', 's', osc_atom_allocString(osc_atom_getPrettyPtr(ad), 0)},
			{ad, 'd', OSC_BUNDLE_TYPETAG, osc_atom_allocBndl(osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocInt8(d))), 1)},
			{ad, 'd', 't', osc_atom_allocTimetag(osc_timetag_floatToTimetag((double)d))},
			{ad, 'd', 'b', osc_atom_allocBlob(blobs[9], 0)},
			{ad, 'd', 'T', osc_atom_true},
			{ad, 'd', 'F', osc_atom_false},
			{ad, 'd', 'N', osc_atom_nil},

			// s, OSC_BUNDLE_TYPETAG, t, b, T, F, N
		};
		for(int i = 0; i < sizeof(conv_test) / sizeof(struct _conv_test); i++){
			struct _conv_test t = conv_test[i];
			PRINT_TEST("%c -> %c", t.ttfrom, t.ttto);
			t_osc_atom *a = osc_atom_convertAny(t.atom, t.ttto);
			if(osc_atom_eql(a, t.expected) == osc_atom_true){
				PRINT_PASSED;
			}else{
				osc_atom_format_m((t_osc_atom_m *)a, 0);
				PRINT_FAILED("Expected atom of type %c (%s), but got %c (%s)", osc_atom_getTypetag(t.expected), osc_atom_getPrettyPtr(t.expected), osc_atom_getTypetag(a), osc_atom_getPrettyPtr(a));
			}
			osc_atom_release(a);
			osc_atom_release(t.expected);
		}
	}

	{
		struct _optest
		{
			t_osc_atom *lhs;
			t_osc_atom *rhs;
			t_osc_atom *(*fn)(t_osc_atom*, t_osc_atom*);
			t_osc_atom *result;
			char *opstring;
		} optest[] = {
			{ac, ac, osc_atom_add, osc_atom_allocInt8(c + c), "+"},
			{ac, aC, osc_atom_add, osc_atom_allocInt16((int16_t)c + C), "+"},
			{ac, au, osc_atom_add, osc_atom_allocInt16((int16_t)c + u), "+"},
			{ac, aU, osc_atom_add, osc_atom_allocInt32((int32_t)c + (int32_t)U), "+"},
			{ac, ai, osc_atom_add, osc_atom_allocInt32((int32_t)c + i), "+"},
			{ac, aI, osc_atom_add, osc_atom_allocInt64((int64_t)c + (int64_t)I), "+"},
			{ac, ah, osc_atom_add, osc_atom_allocInt64((int64_t)c + h), "+"},
			{ac, aH, osc_atom_add, osc_atom_undefined, "+"},
			{ac, af, osc_atom_add, osc_atom_allocFloat((float)c + f), "+"},
			{ac, ad, osc_atom_add, osc_atom_allocDouble((double)c + d), "+"},

			{aC, ac, osc_atom_add, osc_atom_allocInt16((int16_t)C + (int16_t)c), "+"},
			{aC, aC, osc_atom_add, osc_atom_allocUInt8(C + C), "+"},
			{aC, au, osc_atom_add, osc_atom_allocInt16((int16_t)C + u), "+"},
			{aC, aU, osc_atom_add, osc_atom_allocUInt16((uint16_t)C + U), "+"},
			{aC, ai, osc_atom_add, osc_atom_allocInt32((int32_t)C + i), "+"},
			{aC, aI, osc_atom_add, osc_atom_allocUInt32((uint32_t)C + I), "+"},
			{aC, ah, osc_atom_add, osc_atom_allocInt64((int64_t)C + h), "+"},
			{aC, aH, osc_atom_add, osc_atom_allocUInt64((uint64_t)C + H), "+"},
			{aC, af, osc_atom_add, osc_atom_allocFloat((float)C + f), "+"},
			{aC, ad, osc_atom_add, osc_atom_allocDouble((double)C + d), "+"},
			 
			{au, ac, osc_atom_add, osc_atom_allocInt16(u + (int16_t)c), "+"},
			{au, aC, osc_atom_add, osc_atom_allocInt16(u + (int16_t)C), "+"},
			{au, au, osc_atom_add, osc_atom_allocInt16(u + u), "+"},
			{au, aU, osc_atom_add, osc_atom_allocInt32((int32_t)u + U), "+"},
			{au, ai, osc_atom_add, osc_atom_allocInt32((int32_t)u + i), "+"},
			{au, aI, osc_atom_add, osc_atom_allocInt64((int64_t)u + I), "+"},
			{au, ah, osc_atom_add, osc_atom_allocInt64((int64_t)u + h), "+"},
			{au, aH, osc_atom_add, osc_atom_undefined, "+"},
			{au, af, osc_atom_add, osc_atom_allocFloat((float)u + f), "+"},
			{au, ad, osc_atom_add, osc_atom_allocDouble((double)u + d), "+"},
			
			{aU, ac, osc_atom_add, osc_atom_allocInt32(U + (int32_t)c), "+"},
			{aU, aC, osc_atom_add, osc_atom_allocUInt16(U + (uint16_t)C), "+"},
			{aU, au, osc_atom_add, osc_atom_allocInt32((int32_t)U + (int32_t)u), "+"},
			{aU, aU, osc_atom_add, osc_atom_allocUInt16(U + U), "+"},
			{aU, ai, osc_atom_add, osc_atom_allocInt32((int32_t)U + i), "+"},
			{aU, aI, osc_atom_add, osc_atom_allocUInt32((uint32_t)U + I), "+"},
			{aU, ah, osc_atom_add, osc_atom_allocInt64((int64_t)U + h), "+"},
			{aU, aH, osc_atom_add, osc_atom_allocUInt64((uint64_t)U + H), "+"},
			{aU, af, osc_atom_add, osc_atom_allocFloat((float)U + f), "+"},
			{aU, ad, osc_atom_add, osc_atom_allocDouble((double)U + d), "+"},

			{ai, ac, osc_atom_add, osc_atom_allocInt32(i + (int32_t)c), "+"},
			{ai, aC, osc_atom_add, osc_atom_allocInt32(i + (int32_t)C), "+"},
			{ai, au, osc_atom_add, osc_atom_allocInt32(i + (int32_t)u), "+"},
			{ai, aU, osc_atom_add, osc_atom_allocInt32(i + (int32_t)U), "+"},
			{ai, ai, osc_atom_add, osc_atom_allocInt32(i + i), "+"},
			{ai, aI, osc_atom_add, osc_atom_allocInt64((int64_t)i + I), "+"},
			{ai, ah, osc_atom_add, osc_atom_allocInt64((int64_t)i + h), "+"},
			{ai, aH, osc_atom_add, osc_atom_undefined, "+"},
			{ai, af, osc_atom_add, osc_atom_allocDouble((double)i + f), "+"},
			{ai, ad, osc_atom_add, osc_atom_allocDouble((double)i + d), "+"},

			{aI, ac, osc_atom_add, osc_atom_allocInt64(I + (int64_t)c), "+"},
			{aI, aC, osc_atom_add, osc_atom_allocUInt32(I + (uint32_t)C), "+"},
			{aI, au, osc_atom_add, osc_atom_allocInt64(I + (int64_t)u), "+"},
			{aI, aU, osc_atom_add, osc_atom_allocUInt32(I + (uint32_t)U), "+"},
			{aI, ai, osc_atom_add, osc_atom_allocInt64(I + (int64_t)i), "+"},
			{aI, aI, osc_atom_add, osc_atom_allocUInt32(I + I), "+"},
			{aI, ah, osc_atom_add, osc_atom_allocInt64((int64_t)I + h), "+"},
			{aI, aH, osc_atom_add, osc_atom_allocUInt64((uint64_t)I + H), "+"},
			{aI, af, osc_atom_add, osc_atom_allocDouble((double)I + f), "+"},
			{aI, ad, osc_atom_add, osc_atom_allocDouble((double)I + d), "+"},

			{ah, ac, osc_atom_add, osc_atom_allocInt64(h + (int64_t)c), "+"},
			{ah, aC, osc_atom_add, osc_atom_allocInt64(h + (int64_t)C), "+"},
			{ah, au, osc_atom_add, osc_atom_allocInt64(h + (int64_t)u), "+"},
			{ah, aU, osc_atom_add, osc_atom_allocInt64(h + (int64_t)U), "+"},
			{ah, ai, osc_atom_add, osc_atom_allocInt64(h + (int64_t)i), "+"},
			{ah, aI, osc_atom_add, osc_atom_allocInt64(h + (int64_t)I), "+"},
			{ah, ah, osc_atom_add, osc_atom_allocInt64(h + h), "+"},
			{ah, aH, osc_atom_add, osc_atom_undefined, "+"},
			{ah, af, osc_atom_add, osc_atom_undefined, "+"},
			{ah, ad, osc_atom_add, osc_atom_undefined, "+"},

			{aH, ac, osc_atom_add, osc_atom_undefined, "+"},
			{aH, aC, osc_atom_add, osc_atom_allocUInt64(H + (uint64_t)C), "+"},
			{aH, au, osc_atom_add, osc_atom_undefined, "+"},
			{aH, aU, osc_atom_add, osc_atom_allocUInt64(H + (uint64_t)U), "+"},
			{aH, ai, osc_atom_add, osc_atom_undefined, "+"},
			{aH, aI, osc_atom_add, osc_atom_allocUInt64(H + (uint64_t)I), "+"},
			{aH, ah, osc_atom_add, osc_atom_undefined, "+"},
			{aH, aH, osc_atom_add, osc_atom_allocUInt64(H + (uint64_t)H), "+"},
			{aH, af, osc_atom_add, osc_atom_undefined, "+"},
			{aH, ad, osc_atom_add, osc_atom_undefined, "+"},

			{af, ac, osc_atom_add, osc_atom_allocFloat(f + (float)c), "+"},
			{af, aC, osc_atom_add, osc_atom_allocFloat(f + (float)C), "+"},
			{af, au, osc_atom_add, osc_atom_allocFloat(f + (float)u), "+"},
			{af, aU, osc_atom_add, osc_atom_allocFloat(f + (float)U), "+"},
			{af, ai, osc_atom_add, osc_atom_allocDouble(f + (double)i), "+"},
			{af, aI, osc_atom_add, osc_atom_allocDouble(f + (double)I), "+"},
			{af, ah, osc_atom_add, osc_atom_undefined, "+"},
			{af, aH, osc_atom_add, osc_atom_undefined, "+"},
			{af, af, osc_atom_add, osc_atom_allocFloat(f + f), "+"},
			{af, ad, osc_atom_add, osc_atom_allocDouble((double)f + d), "+"},

			{ad, ac, osc_atom_add, osc_atom_allocDouble(d + (double)c), "+"},
			{ad, aC, osc_atom_add, osc_atom_allocDouble(d + (double)C), "+"},
			{ad, au, osc_atom_add, osc_atom_allocDouble(d + (double)u), "+"},
			{ad, aU, osc_atom_add, osc_atom_allocDouble(d + (double)U), "+"},
			{ad, ai, osc_atom_add, osc_atom_allocDouble(d + (double)i), "+"},
			{ad, aI, osc_atom_add, osc_atom_allocDouble(d + (double)I), "+"},
			{ad, ah, osc_atom_add, osc_atom_undefined, "+"},
			{ad, aH, osc_atom_add, osc_atom_undefined, "+"},
			{ad, af, osc_atom_add, osc_atom_allocDouble(d + (double)f), "+"},
			{ad, ad, osc_atom_add, osc_atom_allocDouble(d + d), "+"},

			{ac, ac, osc_atom_sub, osc_atom_allocInt8(c - c), "-"},
			{ac, aC, osc_atom_sub, osc_atom_allocInt16((int16_t)c - C), "-"},
			{ac, au, osc_atom_sub, osc_atom_allocInt16((int16_t)c - u), "-"},
			{ac, aU, osc_atom_sub, osc_atom_allocInt32((int32_t)c - (int32_t)U), "-"},
			{ac, ai, osc_atom_sub, osc_atom_allocInt32((int32_t)c - i), "-"},
			{ac, aI, osc_atom_sub, osc_atom_allocInt64((int64_t)c - (int64_t)I), "-"},
			{ac, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)c - h), "-"},
			{ac, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{ac, af, osc_atom_sub, osc_atom_allocFloat((float)c - f), "-"},
			{ac, ad, osc_atom_sub, osc_atom_allocDouble((double)c - d), "-"},

			{aC, ac, osc_atom_sub, osc_atom_allocInt16((int16_t)C - (int16_t)c), "-"},
			{aC, aC, osc_atom_sub, osc_atom_allocUInt8(C - C), "-"},
			{aC, au, osc_atom_sub, osc_atom_allocInt16((int16_t)C - u), "-"},
			{aC, aU, osc_atom_sub, osc_atom_allocUInt16((uint16_t)C - U), "-"},
			{aC, ai, osc_atom_sub, osc_atom_allocInt32((int32_t)C - i), "-"},
			{aC, aI, osc_atom_sub, osc_atom_allocUInt32((uint32_t)C - I), "-"},
			{aC, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)C - h), "-"},
			{aC, aH, osc_atom_sub, osc_atom_allocUInt64((uint64_t)C - H), "-"},
			{aC, af, osc_atom_sub, osc_atom_allocFloat((float)C - f), "-"},
			{aC, ad, osc_atom_sub, osc_atom_allocDouble((double)C - d), "-"},
			 
			{au, ac, osc_atom_sub, osc_atom_allocInt16(u - (int16_t)c), "-"},
			{au, aC, osc_atom_sub, osc_atom_allocInt16(u - (int16_t)C), "-"},
			{au, au, osc_atom_sub, osc_atom_allocInt16(u - u), "-"},
			{au, aU, osc_atom_sub, osc_atom_allocInt32((int32_t)u - U), "-"},
			{au, ai, osc_atom_sub, osc_atom_allocInt32((int32_t)u - i), "-"},
			{au, aI, osc_atom_sub, osc_atom_allocInt64((int64_t)u - I), "-"},
			{au, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)u - h), "-"},
			{au, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{au, af, osc_atom_sub, osc_atom_allocFloat((float)u - f), "-"},
			{au, ad, osc_atom_sub, osc_atom_allocDouble((double)u - d), "-"},
			
			{aU, ac, osc_atom_sub, osc_atom_allocInt32(U - (int32_t)c), "-"},
			{aU, aC, osc_atom_sub, osc_atom_allocUInt16(U - (uint16_t)C), "-"},
			{aU, au, osc_atom_sub, osc_atom_allocInt32((int32_t)U - (int32_t)u), "-"},
			{aU, aU, osc_atom_sub, osc_atom_allocUInt16(U - U), "-"},
			{aU, ai, osc_atom_sub, osc_atom_allocInt32((int32_t)U - i), "-"},
			{aU, aI, osc_atom_sub, osc_atom_allocUInt32((uint32_t)U - I), "-"},
			{aU, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)U - h), "-"},
			{aU, aH, osc_atom_sub, osc_atom_allocUInt64((uint64_t)U - H), "-"},
			{aU, af, osc_atom_sub, osc_atom_allocFloat((float)U - f), "-"},
			{aU, ad, osc_atom_sub, osc_atom_allocDouble((double)U - d), "-"},

			{ai, ac, osc_atom_sub, osc_atom_allocInt32(i - (int32_t)c), "-"},
			{ai, aC, osc_atom_sub, osc_atom_allocInt32(i - (int32_t)C), "-"},
			{ai, au, osc_atom_sub, osc_atom_allocInt32(i - (int32_t)u), "-"},
			{ai, aU, osc_atom_sub, osc_atom_allocInt32(i - (int32_t)U), "-"},
			{ai, ai, osc_atom_sub, osc_atom_allocInt32(i - i), "-"},
			{ai, aI, osc_atom_sub, osc_atom_allocInt64((int64_t)i - I), "-"},
			{ai, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)i - h), "-"},
			{ai, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{ai, af, osc_atom_sub, osc_atom_allocDouble((double)i - f), "-"},
			{ai, ad, osc_atom_sub, osc_atom_allocDouble((double)i - d), "-"},

			{aI, ac, osc_atom_sub, osc_atom_allocInt64(I - (int64_t)c), "-"},
			{aI, aC, osc_atom_sub, osc_atom_allocUInt32(I - (uint32_t)C), "-"},
			{aI, au, osc_atom_sub, osc_atom_allocInt64(I - (int64_t)u), "-"},
			{aI, aU, osc_atom_sub, osc_atom_allocUInt32(I - (uint32_t)U), "-"},
			{aI, ai, osc_atom_sub, osc_atom_allocInt64(I - (int64_t)i), "-"},
			{aI, aI, osc_atom_sub, osc_atom_allocUInt32(I - I), "-"},
			{aI, ah, osc_atom_sub, osc_atom_allocInt64((int64_t)I - h), "-"},
			{aI, aH, osc_atom_sub, osc_atom_allocUInt64((uint64_t)I - H), "-"},
			{aI, af, osc_atom_sub, osc_atom_allocDouble((double)I - f), "-"},
			{aI, ad, osc_atom_sub, osc_atom_allocDouble((double)I - d), "-"},

			{ah, ac, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)c), "-"},
			{ah, aC, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)C), "-"},
			{ah, au, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)u), "-"},
			{ah, aU, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)U), "-"},
			{ah, ai, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)i), "-"},
			{ah, aI, osc_atom_sub, osc_atom_allocInt64(h - (int64_t)I), "-"},
			{ah, ah, osc_atom_sub, osc_atom_allocInt64(h - h), "-"},
			{ah, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{ah, af, osc_atom_sub, osc_atom_undefined, "-"},
			{ah, ad, osc_atom_sub, osc_atom_undefined, "-"},

			{aH, ac, osc_atom_sub, osc_atom_undefined, "-"},
			{aH, aC, osc_atom_sub, osc_atom_allocUInt64(H - (uint64_t)C), "-"},
			{aH, au, osc_atom_sub, osc_atom_undefined, "-"},
			{aH, aU, osc_atom_sub, osc_atom_allocUInt64(H - (uint64_t)U), "-"},
			{aH, ai, osc_atom_sub, osc_atom_undefined, "-"},
			{aH, aI, osc_atom_sub, osc_atom_allocUInt64(H - (uint64_t)I), "-"},
			{aH, ah, osc_atom_sub, osc_atom_undefined, "-"},
			{aH, aH, osc_atom_sub, osc_atom_allocUInt64(H - (uint64_t)H), "-"},
			{aH, af, osc_atom_sub, osc_atom_undefined, "-"},
			{aH, ad, osc_atom_sub, osc_atom_undefined, "-"},

			{af, ac, osc_atom_sub, osc_atom_allocFloat(f - (float)c), "-"},
			{af, aC, osc_atom_sub, osc_atom_allocFloat(f - (float)C), "-"},
			{af, au, osc_atom_sub, osc_atom_allocFloat(f - (float)u), "-"},
			{af, aU, osc_atom_sub, osc_atom_allocFloat(f - (float)U), "-"},
			{af, ai, osc_atom_sub, osc_atom_allocDouble(f - (double)i), "-"},
			{af, aI, osc_atom_sub, osc_atom_allocDouble(f - (double)I), "-"},
			{af, ah, osc_atom_sub, osc_atom_undefined, "-"},
			{af, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{af, af, osc_atom_sub, osc_atom_allocFloat(f - f), "-"},
			{af, ad, osc_atom_sub, osc_atom_allocDouble((double)f - d), "-"},

			{ad, ac, osc_atom_sub, osc_atom_allocDouble(d - (double)c), "-"},
			{ad, aC, osc_atom_sub, osc_atom_allocDouble(d - (double)C), "-"},
			{ad, au, osc_atom_sub, osc_atom_allocDouble(d - (double)u), "-"},
			{ad, aU, osc_atom_sub, osc_atom_allocDouble(d - (double)U), "-"},
			{ad, ai, osc_atom_sub, osc_atom_allocDouble(d - (double)i), "-"},
			{ad, aI, osc_atom_sub, osc_atom_allocDouble(d - (double)I), "-"},
			{ad, ah, osc_atom_sub, osc_atom_undefined, "-"},
			{ad, aH, osc_atom_sub, osc_atom_undefined, "-"},
			{ad, af, osc_atom_sub, osc_atom_allocDouble(d - (double)f), "-"},
			{ad, ad, osc_atom_sub, osc_atom_allocDouble(d - d), "-"},

						{ac, ac, osc_atom_mul, osc_atom_allocInt8(c * c), "*"},
			{ac, aC, osc_atom_mul, osc_atom_allocInt16((int16_t)c * C), "*"},
			{ac, au, osc_atom_mul, osc_atom_allocInt16((int16_t)c * u), "*"},
			{ac, aU, osc_atom_mul, osc_atom_allocInt32((int32_t)c * (int32_t)U), "*"},
			{ac, ai, osc_atom_mul, osc_atom_allocInt32((int32_t)c * i), "*"},
			{ac, aI, osc_atom_mul, osc_atom_allocInt64((int64_t)c * (int64_t)I), "*"},
			{ac, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)c * h), "*"},
			{ac, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{ac, af, osc_atom_mul, osc_atom_allocFloat((float)c * f), "*"},
			{ac, ad, osc_atom_mul, osc_atom_allocDouble((double)c * d), "*"},

			{aC, ac, osc_atom_mul, osc_atom_allocInt16((int16_t)C * (int16_t)c), "*"},
			{aC, aC, osc_atom_mul, osc_atom_allocUInt8(C * C), "*"},
			{aC, au, osc_atom_mul, osc_atom_allocInt16((int16_t)C * u), "*"},
			{aC, aU, osc_atom_mul, osc_atom_allocUInt16((uint16_t)C * U), "*"},
			{aC, ai, osc_atom_mul, osc_atom_allocInt32((int32_t)C * i), "*"},
			{aC, aI, osc_atom_mul, osc_atom_allocUInt32((uint32_t)C * I), "*"},
			{aC, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)C * h), "*"},
			{aC, aH, osc_atom_mul, osc_atom_allocUInt64((uint64_t)C * H), "*"},
			{aC, af, osc_atom_mul, osc_atom_allocFloat((float)C * f), "*"},
			{aC, ad, osc_atom_mul, osc_atom_allocDouble((double)C * d), "*"},
			 
			{au, ac, osc_atom_mul, osc_atom_allocInt16(u * (int16_t)c), "*"},
			{au, aC, osc_atom_mul, osc_atom_allocInt16(u * (int16_t)C), "*"},
			{au, au, osc_atom_mul, osc_atom_allocInt16(u * u), "*"},
			{au, aU, osc_atom_mul, osc_atom_allocInt32((int32_t)u * U), "*"},
			{au, ai, osc_atom_mul, osc_atom_allocInt32((int32_t)u * i), "*"},
			{au, aI, osc_atom_mul, osc_atom_allocInt64((int64_t)u * I), "*"},
			{au, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)u * h), "*"},
			{au, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{au, af, osc_atom_mul, osc_atom_allocFloat((float)u * f), "*"},
			{au, ad, osc_atom_mul, osc_atom_allocDouble((double)u * d), "*"},
			
			{aU, ac, osc_atom_mul, osc_atom_allocInt32(U * (int32_t)c), "*"},
			{aU, aC, osc_atom_mul, osc_atom_allocUInt16(U * (uint16_t)C), "*"},
			{aU, au, osc_atom_mul, osc_atom_allocInt32((int32_t)U * (int32_t)u), "*"},
			{aU, aU, osc_atom_mul, osc_atom_allocUInt16(U * U), "*"},
			{aU, ai, osc_atom_mul, osc_atom_allocInt32((int32_t)U * i), "*"},
			{aU, aI, osc_atom_mul, osc_atom_allocUInt32((uint32_t)U * I), "*"},
			{aU, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)U * h), "*"},
			{aU, aH, osc_atom_mul, osc_atom_allocUInt64((uint64_t)U * H), "*"},
			{aU, af, osc_atom_mul, osc_atom_allocFloat((float)U * f), "*"},
			{aU, ad, osc_atom_mul, osc_atom_allocDouble((double)U * d), "*"},

			{ai, ac, osc_atom_mul, osc_atom_allocInt32(i * (int32_t)c), "*"},
			{ai, aC, osc_atom_mul, osc_atom_allocInt32(i * (int32_t)C), "*"},
			{ai, au, osc_atom_mul, osc_atom_allocInt32(i * (int32_t)u), "*"},
			{ai, aU, osc_atom_mul, osc_atom_allocInt32(i * (int32_t)U), "*"},
			{ai, ai, osc_atom_mul, osc_atom_allocInt32(i * i), "*"},
			{ai, aI, osc_atom_mul, osc_atom_allocInt64((int64_t)i * I), "*"},
			{ai, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)i * h), "*"},
			{ai, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{ai, af, osc_atom_mul, osc_atom_allocDouble((double)i * f), "*"},
			{ai, ad, osc_atom_mul, osc_atom_allocDouble((double)i * d), "*"},

			{aI, ac, osc_atom_mul, osc_atom_allocInt64(I * (int64_t)c), "*"},
			{aI, aC, osc_atom_mul, osc_atom_allocUInt32(I * (uint32_t)C), "*"},
			{aI, au, osc_atom_mul, osc_atom_allocInt64(I * (int64_t)u), "*"},
			{aI, aU, osc_atom_mul, osc_atom_allocUInt32(I * (uint32_t)U), "*"},
			{aI, ai, osc_atom_mul, osc_atom_allocInt64(I * (int64_t)i), "*"},
			{aI, aI, osc_atom_mul, osc_atom_allocUInt32(I * I), "*"},
			{aI, ah, osc_atom_mul, osc_atom_allocInt64((int64_t)I * h), "*"},
			{aI, aH, osc_atom_mul, osc_atom_allocUInt64((uint64_t)I * H), "*"},
			{aI, af, osc_atom_mul, osc_atom_allocDouble((double)I * f), "*"},
			{aI, ad, osc_atom_mul, osc_atom_allocDouble((double)I * d), "*"},

			{ah, ac, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)c), "*"},
			{ah, aC, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)C), "*"},
			{ah, au, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)u), "*"},
			{ah, aU, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)U), "*"},
			{ah, ai, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)i), "*"},
			{ah, aI, osc_atom_mul, osc_atom_allocInt64(h * (int64_t)I), "*"},
			{ah, ah, osc_atom_mul, osc_atom_allocInt64(h * h), "*"},
			{ah, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{ah, af, osc_atom_mul, osc_atom_undefined, "*"},
			{ah, ad, osc_atom_mul, osc_atom_undefined, "*"},

			{aH, ac, osc_atom_mul, osc_atom_undefined, "*"},
			{aH, aC, osc_atom_mul, osc_atom_allocUInt64(H * (uint64_t)C), "*"},
			{aH, au, osc_atom_mul, osc_atom_undefined, "*"},
			{aH, aU, osc_atom_mul, osc_atom_allocUInt64(H * (uint64_t)U), "*"},
			{aH, ai, osc_atom_mul, osc_atom_undefined, "*"},
			{aH, aI, osc_atom_mul, osc_atom_allocUInt64(H * (uint64_t)I), "*"},
			{aH, ah, osc_atom_mul, osc_atom_undefined, "*"},
			{aH, aH, osc_atom_mul, osc_atom_allocUInt64(H * (uint64_t)H), "*"},
			{aH, af, osc_atom_mul, osc_atom_undefined, "*"},
			{aH, ad, osc_atom_mul, osc_atom_undefined, "*"},

			{af, ac, osc_atom_mul, osc_atom_allocFloat(f * (float)c), "*"},
			{af, aC, osc_atom_mul, osc_atom_allocFloat(f * (float)C), "*"},
			{af, au, osc_atom_mul, osc_atom_allocFloat(f * (float)u), "*"},
			{af, aU, osc_atom_mul, osc_atom_allocFloat(f * (float)U), "*"},
			{af, ai, osc_atom_mul, osc_atom_allocDouble(f * (double)i), "*"},
			{af, aI, osc_atom_mul, osc_atom_allocDouble(f * (double)I), "*"},
			{af, ah, osc_atom_mul, osc_atom_undefined, "*"},
			{af, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{af, af, osc_atom_mul, osc_atom_allocFloat(f * f), "*"},
			{af, ad, osc_atom_mul, osc_atom_allocDouble((double)f * d), "*"},

			{ad, ac, osc_atom_mul, osc_atom_allocDouble(d * (double)c), "*"},
			{ad, aC, osc_atom_mul, osc_atom_allocDouble(d * (double)C), "*"},
			{ad, au, osc_atom_mul, osc_atom_allocDouble(d * (double)u), "*"},
			{ad, aU, osc_atom_mul, osc_atom_allocDouble(d * (double)U), "*"},
			{ad, ai, osc_atom_mul, osc_atom_allocDouble(d * (double)i), "*"},
			{ad, aI, osc_atom_mul, osc_atom_allocDouble(d * (double)I), "*"},
			{ad, ah, osc_atom_mul, osc_atom_undefined, "*"},
			{ad, aH, osc_atom_mul, osc_atom_undefined, "*"},
			{ad, af, osc_atom_mul, osc_atom_allocDouble(d * (double)f), "*"},
			{ad, ad, osc_atom_mul, osc_atom_allocDouble(d * d), "*"},

						{ac, ac, osc_atom_div, osc_atom_allocInt8(c / c), "/"},
			{ac, aC, osc_atom_div, osc_atom_allocInt16((int16_t)c / C), "/"},
			{ac, au, osc_atom_div, osc_atom_allocInt16((int16_t)c / u), "/"},
			{ac, aU, osc_atom_div, osc_atom_allocInt32((int32_t)c / (int32_t)U), "/"},
			{ac, ai, osc_atom_div, osc_atom_allocInt32((int32_t)c / i), "/"},
			{ac, aI, osc_atom_div, osc_atom_allocInt64((int64_t)c / (int64_t)I), "/"},
			{ac, ah, osc_atom_div, osc_atom_allocInt64((int64_t)c / h), "/"},
			{ac, aH, osc_atom_div, osc_atom_undefined, "/"},
			{ac, af, osc_atom_div, osc_atom_allocFloat((float)c / f), "/"},
			{ac, ad, osc_atom_div, osc_atom_allocDouble((double)c / d), "/"},

			{aC, ac, osc_atom_div, osc_atom_allocInt16((int16_t)C / (int16_t)c), "/"},
			{aC, aC, osc_atom_div, osc_atom_allocUInt8(C / C), "/"},
			{aC, au, osc_atom_div, osc_atom_allocInt16((int16_t)C / u), "/"},
			{aC, aU, osc_atom_div, osc_atom_allocUInt16((uint16_t)C / U), "/"},
			{aC, ai, osc_atom_div, osc_atom_allocInt32((int32_t)C / i), "/"},
			{aC, aI, osc_atom_div, osc_atom_allocUInt32((uint32_t)C / I), "/"},
			{aC, ah, osc_atom_div, osc_atom_allocInt64((int64_t)C / h), "/"},
			{aC, aH, osc_atom_div, osc_atom_allocUInt64((uint64_t)C / H), "/"},
			{aC, af, osc_atom_div, osc_atom_allocFloat((float)C / f), "/"},
			{aC, ad, osc_atom_div, osc_atom_allocDouble((double)C / d), "/"},
			 
			{au, ac, osc_atom_div, osc_atom_allocInt16(u / (int16_t)c), "/"},
			{au, aC, osc_atom_div, osc_atom_allocInt16(u / (int16_t)C), "/"},
			{au, au, osc_atom_div, osc_atom_allocInt16(u / u), "/"},
			{au, aU, osc_atom_div, osc_atom_allocInt32((int32_t)u / U), "/"},
			{au, ai, osc_atom_div, osc_atom_allocInt32((int32_t)u / i), "/"},
			{au, aI, osc_atom_div, osc_atom_allocInt64((int64_t)u / I), "/"},
			{au, ah, osc_atom_div, osc_atom_allocInt64((int64_t)u / h), "/"},
			{au, aH, osc_atom_div, osc_atom_undefined, "/"},
			{au, af, osc_atom_div, osc_atom_allocFloat((float)u / f), "/"},
			{au, ad, osc_atom_div, osc_atom_allocDouble((double)u / d), "/"},
			
			{aU, ac, osc_atom_div, osc_atom_allocInt32(U / (int32_t)c), "/"},
			{aU, aC, osc_atom_div, osc_atom_allocUInt16(U / (uint16_t)C), "/"},
			{aU, au, osc_atom_div, osc_atom_allocInt32((int32_t)U / (int32_t)u), "/"},
			{aU, aU, osc_atom_div, osc_atom_allocUInt16(U / U), "/"},
			{aU, ai, osc_atom_div, osc_atom_allocInt32((int32_t)U / i), "/"},
			{aU, aI, osc_atom_div, osc_atom_allocUInt32((uint32_t)U / I), "/"},
			{aU, ah, osc_atom_div, osc_atom_allocInt64((int64_t)U / h), "/"},
			{aU, aH, osc_atom_div, osc_atom_allocUInt64((uint64_t)U / H), "/"},
			{aU, af, osc_atom_div, osc_atom_allocFloat((float)U / f), "/"},
			{aU, ad, osc_atom_div, osc_atom_allocDouble((double)U / d), "/"},

			{ai, ac, osc_atom_div, osc_atom_allocInt32(i / (int32_t)c), "/"},
			{ai, aC, osc_atom_div, osc_atom_allocInt32(i / (int32_t)C), "/"},
			{ai, au, osc_atom_div, osc_atom_allocInt32(i / (int32_t)u), "/"},
			{ai, aU, osc_atom_div, osc_atom_allocInt32(i / (int32_t)U), "/"},
			{ai, ai, osc_atom_div, osc_atom_allocInt32(i / i), "/"},
			{ai, aI, osc_atom_div, osc_atom_allocInt64((int64_t)i / I), "/"},
			{ai, ah, osc_atom_div, osc_atom_allocInt64((int64_t)i / h), "/"},
			{ai, aH, osc_atom_div, osc_atom_undefined, "/"},
			{ai, af, osc_atom_div, osc_atom_allocDouble((double)i / f), "/"},
			{ai, ad, osc_atom_div, osc_atom_allocDouble((double)i / d), "/"},

			{aI, ac, osc_atom_div, osc_atom_allocInt64(I / (int64_t)c), "/"},
			{aI, aC, osc_atom_div, osc_atom_allocUInt32(I / (uint32_t)C), "/"},
			{aI, au, osc_atom_div, osc_atom_allocInt64(I / (int64_t)u), "/"},
			{aI, aU, osc_atom_div, osc_atom_allocUInt32(I / (uint32_t)U), "/"},
			{aI, ai, osc_atom_div, osc_atom_allocInt64(I / (int64_t)i), "/"},
			{aI, aI, osc_atom_div, osc_atom_allocUInt32(I / I), "/"},
			{aI, ah, osc_atom_div, osc_atom_allocInt64((int64_t)I / h), "/"},
			{aI, aH, osc_atom_div, osc_atom_allocUInt64((uint64_t)I / H), "/"},
			{aI, af, osc_atom_div, osc_atom_allocDouble((double)I / f), "/"},
			{aI, ad, osc_atom_div, osc_atom_allocDouble((double)I / d), "/"},

			{ah, ac, osc_atom_div, osc_atom_allocInt64(h / (int64_t)c), "/"},
			{ah, aC, osc_atom_div, osc_atom_allocInt64(h / (int64_t)C), "/"},
			{ah, au, osc_atom_div, osc_atom_allocInt64(h / (int64_t)u), "/"},
			{ah, aU, osc_atom_div, osc_atom_allocInt64(h / (int64_t)U), "/"},
			{ah, ai, osc_atom_div, osc_atom_allocInt64(h / (int64_t)i), "/"},
			{ah, aI, osc_atom_div, osc_atom_allocInt64(h / (int64_t)I), "/"},
			{ah, ah, osc_atom_div, osc_atom_allocInt64(h / h), "/"},
			{ah, aH, osc_atom_div, osc_atom_undefined, "/"},
			{ah, af, osc_atom_div, osc_atom_undefined, "/"},
			{ah, ad, osc_atom_div, osc_atom_undefined, "/"},

			{aH, ac, osc_atom_div, osc_atom_undefined, "/"},
			{aH, aC, osc_atom_div, osc_atom_allocUInt64(H / (uint64_t)C), "/"},
			{aH, au, osc_atom_div, osc_atom_undefined, "/"},
			{aH, aU, osc_atom_div, osc_atom_allocUInt64(H / (uint64_t)U), "/"},
			{aH, ai, osc_atom_div, osc_atom_undefined, "/"},
			{aH, aI, osc_atom_div, osc_atom_allocUInt64(H / (uint64_t)I), "/"},
			{aH, ah, osc_atom_div, osc_atom_undefined, "/"},
			{aH, aH, osc_atom_div, osc_atom_allocUInt64(H / (uint64_t)H), "/"},
			{aH, af, osc_atom_div, osc_atom_undefined, "/"},
			{aH, ad, osc_atom_div, osc_atom_undefined, "/"},

			{af, ac, osc_atom_div, osc_atom_allocFloat(f / (float)c), "/"},
			{af, aC, osc_atom_div, osc_atom_allocFloat(f / (float)C), "/"},
			{af, au, osc_atom_div, osc_atom_allocFloat(f / (float)u), "/"},
			{af, aU, osc_atom_div, osc_atom_allocFloat(f / (float)U), "/"},
			{af, ai, osc_atom_div, osc_atom_allocDouble(f / (double)i), "/"},
			{af, aI, osc_atom_div, osc_atom_allocDouble(f / (double)I), "/"},
			{af, ah, osc_atom_div, osc_atom_undefined, "/"},
			{af, aH, osc_atom_div, osc_atom_undefined, "/"},
			{af, af, osc_atom_div, osc_atom_allocFloat(f / f), "/"},
			{af, ad, osc_atom_div, osc_atom_allocDouble((double)f / d), "/"},

			{ad, ac, osc_atom_div, osc_atom_allocDouble(d / (double)c), "/"},
			{ad, aC, osc_atom_div, osc_atom_allocDouble(d / (double)C), "/"},
			{ad, au, osc_atom_div, osc_atom_allocDouble(d / (double)u), "/"},
			{ad, aU, osc_atom_div, osc_atom_allocDouble(d / (double)U), "/"},
			{ad, ai, osc_atom_div, osc_atom_allocDouble(d / (double)i), "/"},
			{ad, aI, osc_atom_div, osc_atom_allocDouble(d / (double)I), "/"},
			{ad, ah, osc_atom_div, osc_atom_undefined, "/"},
			{ad, aH, osc_atom_div, osc_atom_undefined, "/"},
			{ad, af, osc_atom_div, osc_atom_allocDouble(d / (double)f), "/"},
			{ad, ad, osc_atom_div, osc_atom_allocDouble(d / d), "/"},

			{ac, ac, osc_atom_eqv, (c == c) ? osc_atom_true : osc_atom_false, "="},
			{ac, aC, osc_atom_eqv, ((int16_t)c == C) ? osc_atom_true : osc_atom_false, "="},
			{ac, au, osc_atom_eqv, ((int16_t)c == u) ? osc_atom_true : osc_atom_false, "="},
			{ac, aU, osc_atom_eqv, ((int32_t)c == (int32_t)U) ? osc_atom_true : osc_atom_false, "="},
			{ac, ai, osc_atom_eqv, ((int32_t)c == i) ? osc_atom_true : osc_atom_false, "="},
			{ac, aI, osc_atom_eqv, ((int64_t)c == (int64_t)I) ? osc_atom_true : osc_atom_false, "="},
			{ac, ah, osc_atom_eqv, ((int64_t)c == h) ? osc_atom_true : osc_atom_false, "="},
			{ac, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{ac, af, osc_atom_eqv, ((float)c == f) ? osc_atom_true : osc_atom_false, "="},
			{ac, ad, osc_atom_eqv, ((double)c == d) ? osc_atom_true : osc_atom_false, "="},

			{aC, ac, osc_atom_eqv, ((int16_t)C == (int16_t)c) ? osc_atom_true : osc_atom_false, "="},
			{aC, aC, osc_atom_eqv, (C == C) ? osc_atom_true : osc_atom_false, "="},
			{aC, au, osc_atom_eqv, ((int16_t)C == u) ? osc_atom_true : osc_atom_false, "="},
			{aC, aU, osc_atom_eqv, ((uint16_t)C == U) ? osc_atom_true : osc_atom_false, "="},
			{aC, ai, osc_atom_eqv, ((int32_t)C == i) ? osc_atom_true : osc_atom_false, "="},
			{aC, aI, osc_atom_eqv, ((uint32_t)C == I) ? osc_atom_true : osc_atom_false, "="},
			{aC, ah, osc_atom_eqv, ((int64_t)C == h) ? osc_atom_true : osc_atom_false, "="},
			{aC, aH, osc_atom_eqv, ((uint64_t)C == H) ? osc_atom_true : osc_atom_false, "="},
			{aC, af, osc_atom_eqv, ((float)C == f) ? osc_atom_true : osc_atom_false, "="},
			{aC, ad, osc_atom_eqv, ((double)C == d) ? osc_atom_true : osc_atom_false, "="},
			 
			{au, ac, osc_atom_eqv, (u == (int16_t)c) ? osc_atom_true : osc_atom_false, "="},
			{au, aC, osc_atom_eqv, (u == (int16_t)C) ? osc_atom_true : osc_atom_false, "="},
			{au, au, osc_atom_eqv, (u == u) ? osc_atom_true : osc_atom_false, "="},
			{au, aU, osc_atom_eqv, ((int32_t)u == U) ? osc_atom_true : osc_atom_false, "="},
			{au, ai, osc_atom_eqv, ((int32_t)u == i) ? osc_atom_true : osc_atom_false, "="},
			{au, aI, osc_atom_eqv, ((int64_t)u == I) ? osc_atom_true : osc_atom_false, "="},
			{au, ah, osc_atom_eqv, ((int64_t)u == h) ? osc_atom_true : osc_atom_false, "="},
			{au, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{au, af, osc_atom_eqv, ((float)u == f) ? osc_atom_true : osc_atom_false, "="},
			{au, ad, osc_atom_eqv, ((double)u == d) ? osc_atom_true : osc_atom_false, "="},
			
			{aU, ac, osc_atom_eqv, (U == (int32_t)c) ? osc_atom_true : osc_atom_false, "="},
			{aU, aC, osc_atom_eqv, (U == (uint16_t)C) ? osc_atom_true : osc_atom_false, "="},
			{aU, au, osc_atom_eqv, ((int32_t)U == (int32_t)u) ? osc_atom_true : osc_atom_false, "="},
			{aU, aU, osc_atom_eqv, (U == U) ? osc_atom_true : osc_atom_false, "="},
			{aU, ai, osc_atom_eqv, ((int32_t)U == i) ? osc_atom_true : osc_atom_false, "="},
			{aU, aI, osc_atom_eqv, ((uint32_t)U == I) ? osc_atom_true : osc_atom_false, "="},
			{aU, ah, osc_atom_eqv, ((int64_t)U == h) ? osc_atom_true : osc_atom_false, "="},
			{aU, aH, osc_atom_eqv, ((uint64_t)U == H) ? osc_atom_true : osc_atom_false, "="},
			{aU, af, osc_atom_eqv, ((float)U == f) ? osc_atom_true : osc_atom_false, "="},
			{aU, ad, osc_atom_eqv, ((double)U == d) ? osc_atom_true : osc_atom_false, "="},

			{ai, ac, osc_atom_eqv, (i == (int32_t)c) ? osc_atom_true : osc_atom_false, "="},
			{ai, aC, osc_atom_eqv, (i == (int32_t)C) ? osc_atom_true : osc_atom_false, "="},
			{ai, au, osc_atom_eqv, (i == (int32_t)u) ? osc_atom_true : osc_atom_false, "="},
			{ai, aU, osc_atom_eqv, (i == (int32_t)U) ? osc_atom_true : osc_atom_false, "="},
			{ai, ai, osc_atom_eqv, (i == i) ? osc_atom_true : osc_atom_false, "="},
			{ai, aI, osc_atom_eqv, ((int64_t)i == I) ? osc_atom_true : osc_atom_false, "="},
			{ai, ah, osc_atom_eqv, ((int64_t)i == h) ? osc_atom_true : osc_atom_false, "="},
			{ai, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{ai, af, osc_atom_eqv, ((double)i == f) ? osc_atom_true : osc_atom_false, "="},
			{ai, ad, osc_atom_eqv, ((double)i == d) ? osc_atom_true : osc_atom_false, "="},

			{aI, ac, osc_atom_eqv, (I == (int64_t)c) ? osc_atom_true : osc_atom_false, "="},
			{aI, aC, osc_atom_eqv, (I == (uint32_t)C) ? osc_atom_true : osc_atom_false, "="},
			{aI, au, osc_atom_eqv, (I == (int64_t)u) ? osc_atom_true : osc_atom_false, "="},
			{aI, aU, osc_atom_eqv, (I == (uint32_t)U) ? osc_atom_true : osc_atom_false, "="},
			{aI, ai, osc_atom_eqv, (I == (int64_t)i) ? osc_atom_true : osc_atom_false, "="},
			{aI, aI, osc_atom_eqv, (I == I) ? osc_atom_true : osc_atom_false, "="},
			{aI, ah, osc_atom_eqv, ((int64_t)I == h) ? osc_atom_true : osc_atom_false, "="},
			{aI, aH, osc_atom_eqv, ((uint64_t)I == H) ? osc_atom_true : osc_atom_false, "="},
			{aI, af, osc_atom_eqv, ((double)I == f) ? osc_atom_true : osc_atom_false, "="},
			{aI, ad, osc_atom_eqv, ((double)I == d) ? osc_atom_true : osc_atom_false, "="},

			{ah, ac, osc_atom_eqv, (h == (int64_t)c) ? osc_atom_true : osc_atom_false, "="},
			{ah, aC, osc_atom_eqv, (h == (int64_t)C) ? osc_atom_true : osc_atom_false, "="},
			{ah, au, osc_atom_eqv, (h == (int64_t)u) ? osc_atom_true : osc_atom_false, "="},
			{ah, aU, osc_atom_eqv, (h == (int64_t)U) ? osc_atom_true : osc_atom_false, "="},
			{ah, ai, osc_atom_eqv, (h == (int64_t)i) ? osc_atom_true : osc_atom_false, "="},
			{ah, aI, osc_atom_eqv, (h == (int64_t)I) ? osc_atom_true : osc_atom_false, "="},
			{ah, ah, osc_atom_eqv, (h == h) ? osc_atom_true : osc_atom_false, "="},
			{ah, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{ah, af, osc_atom_eqv, osc_atom_undefined, "="},
			{ah, ad, osc_atom_eqv, osc_atom_undefined, "="},

			{aH, ac, osc_atom_eqv, osc_atom_undefined, "="},
			{aH, aC, osc_atom_eqv, (H == (uint64_t)C) ? osc_atom_true : osc_atom_false, "="},
			{aH, au, osc_atom_eqv, osc_atom_undefined, "="},
			{aH, aU, osc_atom_eqv, (H == (uint64_t)U) ? osc_atom_true : osc_atom_false, "="},
			{aH, ai, osc_atom_eqv, osc_atom_undefined, "="},
			{aH, aI, osc_atom_eqv, (H == (uint64_t)I) ? osc_atom_true : osc_atom_false, "="},
			{aH, ah, osc_atom_eqv, osc_atom_undefined, "="},
			{aH, aH, osc_atom_eqv, (H == (uint64_t)H) ? osc_atom_true : osc_atom_false, "="},
			{aH, af, osc_atom_eqv, osc_atom_undefined, "="},
			{aH, ad, osc_atom_eqv, osc_atom_undefined, "="},

			{af, ac, osc_atom_eqv, (f == (float)c) ? osc_atom_true : osc_atom_false, "="},
			{af, aC, osc_atom_eqv, (f == (float)C) ? osc_atom_true : osc_atom_false, "="},
			{af, au, osc_atom_eqv, (f == (float)u) ? osc_atom_true : osc_atom_false, "="},
			{af, aU, osc_atom_eqv, (f == (float)U) ? osc_atom_true : osc_atom_false, "="},
			{af, ai, osc_atom_eqv, (f == (double)i) ? osc_atom_true : osc_atom_false, "="},
			{af, aI, osc_atom_eqv, (f == (double)I) ? osc_atom_true : osc_atom_false, "="},
			{af, ah, osc_atom_eqv, osc_atom_undefined, "="},
			{af, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{af, af, osc_atom_eqv, (f == f) ? osc_atom_true : osc_atom_false, "="},
			{af, ad, osc_atom_eqv, ((double)f == d) ? osc_atom_true : osc_atom_false, "="},

			{ad, ac, osc_atom_eqv, (d == (double)c) ? osc_atom_true : osc_atom_false, "="},
			{ad, aC, osc_atom_eqv, (d == (double)C) ? osc_atom_true : osc_atom_false, "="},
			{ad, au, osc_atom_eqv, (d == (double)u) ? osc_atom_true : osc_atom_false, "="},
			{ad, aU, osc_atom_eqv, (d == (double)U) ? osc_atom_true : osc_atom_false, "="},
			{ad, ai, osc_atom_eqv, (d == (double)i) ? osc_atom_true : osc_atom_false, "="},
			{ad, aI, osc_atom_eqv, (d == (double)I) ? osc_atom_true : osc_atom_false, "="},
			{ad, ah, osc_atom_eqv, osc_atom_undefined, "="},
			{ad, aH, osc_atom_eqv, osc_atom_undefined, "="},
			{ad, af, osc_atom_eqv, (d == (double)f) ? osc_atom_true : osc_atom_false, "="},
			{ad, ad, osc_atom_eqv, (d == d) ? osc_atom_true : osc_atom_false, "="},

						{ac, ac, osc_atom_lt, (c < c) ? osc_atom_true : osc_atom_false, "<"},
			{ac, aC, osc_atom_lt, ((int16_t)c < C) ? osc_atom_true : osc_atom_false, "<"},
			{ac, au, osc_atom_lt, ((int16_t)c < u) ? osc_atom_true : osc_atom_false, "<"},
			{ac, aU, osc_atom_lt, ((int32_t)c < (int32_t)U) ? osc_atom_true : osc_atom_false, "<"},
			{ac, ai, osc_atom_lt, ((int32_t)c < i) ? osc_atom_true : osc_atom_false, "<"},
			{ac, aI, osc_atom_lt, ((int64_t)c < (int64_t)I) ? osc_atom_true : osc_atom_false, "<"},
			{ac, ah, osc_atom_lt, ((int64_t)c < h) ? osc_atom_true : osc_atom_false, "<"},
			{ac, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{ac, af, osc_atom_lt, ((float)c < f) ? osc_atom_true : osc_atom_false, "<"},
			{ac, ad, osc_atom_lt, ((double)c < d) ? osc_atom_true : osc_atom_false, "<"},

			{aC, ac, osc_atom_lt, ((int16_t)C < (int16_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{aC, aC, osc_atom_lt, (C < C) ? osc_atom_true : osc_atom_false, "<"},
			{aC, au, osc_atom_lt, ((int16_t)C < u) ? osc_atom_true : osc_atom_false, "<"},
			{aC, aU, osc_atom_lt, ((uint16_t)C < U) ? osc_atom_true : osc_atom_false, "<"},
			{aC, ai, osc_atom_lt, ((int32_t)C < i) ? osc_atom_true : osc_atom_false, "<"},
			{aC, aI, osc_atom_lt, ((uint32_t)C < I) ? osc_atom_true : osc_atom_false, "<"},
			{aC, ah, osc_atom_lt, ((int64_t)C < h) ? osc_atom_true : osc_atom_false, "<"},
			{aC, aH, osc_atom_lt, ((uint64_t)C < H) ? osc_atom_true : osc_atom_false, "<"},
			{aC, af, osc_atom_lt, ((float)C < f) ? osc_atom_true : osc_atom_false, "<"},
			{aC, ad, osc_atom_lt, ((double)C < d) ? osc_atom_true : osc_atom_false, "<"},
			 
			{au, ac, osc_atom_lt, (u < (int16_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{au, aC, osc_atom_lt, (u < (int16_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{au, au, osc_atom_lt, (u < u) ? osc_atom_true : osc_atom_false, "<"},
			{au, aU, osc_atom_lt, ((int32_t)u < U) ? osc_atom_true : osc_atom_false, "<"},
			{au, ai, osc_atom_lt, ((int32_t)u < i) ? osc_atom_true : osc_atom_false, "<"},
			{au, aI, osc_atom_lt, ((int64_t)u < I) ? osc_atom_true : osc_atom_false, "<"},
			{au, ah, osc_atom_lt, ((int64_t)u < h) ? osc_atom_true : osc_atom_false, "<"},
			{au, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{au, af, osc_atom_lt, ((float)u < f) ? osc_atom_true : osc_atom_false, "<"},
			{au, ad, osc_atom_lt, ((double)u < d) ? osc_atom_true : osc_atom_false, "<"},
			
			{aU, ac, osc_atom_lt, (U < (int32_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{aU, aC, osc_atom_lt, (U < (uint16_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{aU, au, osc_atom_lt, ((int32_t)U < (int32_t)u) ? osc_atom_true : osc_atom_false, "<"},
			{aU, aU, osc_atom_lt, (U < U) ? osc_atom_true : osc_atom_false, "<"},
			{aU, ai, osc_atom_lt, ((int32_t)U < i) ? osc_atom_true : osc_atom_false, "<"},
			{aU, aI, osc_atom_lt, ((uint32_t)U < I) ? osc_atom_true : osc_atom_false, "<"},
			{aU, ah, osc_atom_lt, ((int64_t)U < h) ? osc_atom_true : osc_atom_false, "<"},
			{aU, aH, osc_atom_lt, ((uint64_t)U < H) ? osc_atom_true : osc_atom_false, "<"},
			{aU, af, osc_atom_lt, ((float)U < f) ? osc_atom_true : osc_atom_false, "<"},
			{aU, ad, osc_atom_lt, ((double)U < d) ? osc_atom_true : osc_atom_false, "<"},

			{ai, ac, osc_atom_lt, (i < (int32_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{ai, aC, osc_atom_lt, (i < (int32_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{ai, au, osc_atom_lt, (i < (int32_t)u) ? osc_atom_true : osc_atom_false, "<"},
			{ai, aU, osc_atom_lt, (i < (int32_t)U) ? osc_atom_true : osc_atom_false, "<"},
			{ai, ai, osc_atom_lt, (i < i) ? osc_atom_true : osc_atom_false, "<"},
			{ai, aI, osc_atom_lt, ((int64_t)i < I) ? osc_atom_true : osc_atom_false, "<"},
			{ai, ah, osc_atom_lt, ((int64_t)i < h) ? osc_atom_true : osc_atom_false, "<"},
			{ai, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{ai, af, osc_atom_lt, ((double)i < f) ? osc_atom_true : osc_atom_false, "<"},
			{ai, ad, osc_atom_lt, ((double)i < d) ? osc_atom_true : osc_atom_false, "<"},

			{aI, ac, osc_atom_lt, (I < (int64_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{aI, aC, osc_atom_lt, (I < (uint32_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{aI, au, osc_atom_lt, (I < (int64_t)u) ? osc_atom_true : osc_atom_false, "<"},
			{aI, aU, osc_atom_lt, (I < (uint32_t)U) ? osc_atom_true : osc_atom_false, "<"},
			{aI, ai, osc_atom_lt, (I < (int64_t)i) ? osc_atom_true : osc_atom_false, "<"},
			{aI, aI, osc_atom_lt, (I < I) ? osc_atom_true : osc_atom_false, "<"},
			{aI, ah, osc_atom_lt, ((int64_t)I < h) ? osc_atom_true : osc_atom_false, "<"},
			{aI, aH, osc_atom_lt, ((uint64_t)I < H) ? osc_atom_true : osc_atom_false, "<"},
			{aI, af, osc_atom_lt, ((double)I < f) ? osc_atom_true : osc_atom_false, "<"},
			{aI, ad, osc_atom_lt, ((double)I < d) ? osc_atom_true : osc_atom_false, "<"},

			{ah, ac, osc_atom_lt, (h < (int64_t)c) ? osc_atom_true : osc_atom_false, "<"},
			{ah, aC, osc_atom_lt, (h < (int64_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{ah, au, osc_atom_lt, (h < (int64_t)u) ? osc_atom_true : osc_atom_false, "<"},
			{ah, aU, osc_atom_lt, (h < (int64_t)U) ? osc_atom_true : osc_atom_false, "<"},
			{ah, ai, osc_atom_lt, (h < (int64_t)i) ? osc_atom_true : osc_atom_false, "<"},
			{ah, aI, osc_atom_lt, (h < (int64_t)I) ? osc_atom_true : osc_atom_false, "<"},
			{ah, ah, osc_atom_lt, (h < h) ? osc_atom_true : osc_atom_false, "<"},
			{ah, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{ah, af, osc_atom_lt, osc_atom_undefined, "<"},
			{ah, ad, osc_atom_lt, osc_atom_undefined, "<"},

			{aH, ac, osc_atom_lt, osc_atom_undefined, "<"},
			{aH, aC, osc_atom_lt, (H < (uint64_t)C) ? osc_atom_true : osc_atom_false, "<"},
			{aH, au, osc_atom_lt, osc_atom_undefined, "<"},
			{aH, aU, osc_atom_lt, (H < (uint64_t)U) ? osc_atom_true : osc_atom_false, "<"},
			{aH, ai, osc_atom_lt, osc_atom_undefined, "<"},
			{aH, aI, osc_atom_lt, (H < (uint64_t)I) ? osc_atom_true : osc_atom_false, "<"},
			{aH, ah, osc_atom_lt, osc_atom_undefined, "<"},
			{aH, aH, osc_atom_lt, (H < (uint64_t)H) ? osc_atom_true : osc_atom_false, "<"},
			{aH, af, osc_atom_lt, osc_atom_undefined, "<"},
			{aH, ad, osc_atom_lt, osc_atom_undefined, "<"},

			{af, ac, osc_atom_lt, (f < (float)c) ? osc_atom_true : osc_atom_false, "<"},
			{af, aC, osc_atom_lt, (f < (float)C) ? osc_atom_true : osc_atom_false, "<"},
			{af, au, osc_atom_lt, (f < (float)u) ? osc_atom_true : osc_atom_false, "<"},
			{af, aU, osc_atom_lt, (f < (float)U) ? osc_atom_true : osc_atom_false, "<"},
			{af, ai, osc_atom_lt, (f < (double)i) ? osc_atom_true : osc_atom_false, "<"},
			{af, aI, osc_atom_lt, (f < (double)I) ? osc_atom_true : osc_atom_false, "<"},
			{af, ah, osc_atom_lt, osc_atom_undefined, "<"},
			{af, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{af, af, osc_atom_lt, (f < f) ? osc_atom_true : osc_atom_false, "<"},
			{af, ad, osc_atom_lt, ((double)f < d) ? osc_atom_true : osc_atom_false, "<"},

			{ad, ac, osc_atom_lt, (d < (double)c) ? osc_atom_true : osc_atom_false, "<"},
			{ad, aC, osc_atom_lt, (d < (double)C) ? osc_atom_true : osc_atom_false, "<"},
			{ad, au, osc_atom_lt, (d < (double)u) ? osc_atom_true : osc_atom_false, "<"},
			{ad, aU, osc_atom_lt, (d < (double)U) ? osc_atom_true : osc_atom_false, "<"},
			{ad, ai, osc_atom_lt, (d < (double)i) ? osc_atom_true : osc_atom_false, "<"},
			{ad, aI, osc_atom_lt, (d < (double)I) ? osc_atom_true : osc_atom_false, "<"},
			{ad, ah, osc_atom_lt, osc_atom_undefined, "<"},
			{ad, aH, osc_atom_lt, osc_atom_undefined, "<"},
			{ad, af, osc_atom_lt, (d < (double)f) ? osc_atom_true : osc_atom_false, "<"},
			{ad, ad, osc_atom_lt, (d < d) ? osc_atom_true : osc_atom_false, "<"},

						{ac, ac, osc_atom_le, (c <= c) ? osc_atom_true : osc_atom_false, "<="},
			{ac, aC, osc_atom_le, ((int16_t)c <= C) ? osc_atom_true : osc_atom_false, "<="},
			{ac, au, osc_atom_le, ((int16_t)c <= u) ? osc_atom_true : osc_atom_false, "<="},
			{ac, aU, osc_atom_le, ((int32_t)c <= (int32_t)U) ? osc_atom_true : osc_atom_false, "<="},
			{ac, ai, osc_atom_le, ((int32_t)c <= i) ? osc_atom_true : osc_atom_false, "<="},
			{ac, aI, osc_atom_le, ((int64_t)c <= (int64_t)I) ? osc_atom_true : osc_atom_false, "<="},
			{ac, ah, osc_atom_le, ((int64_t)c <= h) ? osc_atom_true : osc_atom_false, "<="},
			{ac, aH, osc_atom_le, osc_atom_undefined, "<="},
			{ac, af, osc_atom_le, ((float)c <= f) ? osc_atom_true : osc_atom_false, "<="},
			{ac, ad, osc_atom_le, ((double)c <= d) ? osc_atom_true : osc_atom_false, "<="},

			{aC, ac, osc_atom_le, ((int16_t)C <= (int16_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{aC, aC, osc_atom_le, (C <= C) ? osc_atom_true : osc_atom_false, "<="},
			{aC, au, osc_atom_le, ((int16_t)C <= u) ? osc_atom_true : osc_atom_false, "<="},
			{aC, aU, osc_atom_le, ((uint16_t)C <= U) ? osc_atom_true : osc_atom_false, "<="},
			{aC, ai, osc_atom_le, ((int32_t)C <= i) ? osc_atom_true : osc_atom_false, "<="},
			{aC, aI, osc_atom_le, ((uint32_t)C <= I) ? osc_atom_true : osc_atom_false, "<="},
			{aC, ah, osc_atom_le, ((int64_t)C <= h) ? osc_atom_true : osc_atom_false, "<="},
			{aC, aH, osc_atom_le, ((uint64_t)C <= H) ? osc_atom_true : osc_atom_false, "<="},
			{aC, af, osc_atom_le, ((float)C <= f) ? osc_atom_true : osc_atom_false, "<="},
			{aC, ad, osc_atom_le, ((double)C <= d) ? osc_atom_true : osc_atom_false, "<="},
			 
			{au, ac, osc_atom_le, (u <= (int16_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{au, aC, osc_atom_le, (u <= (int16_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{au, au, osc_atom_le, (u <= u) ? osc_atom_true : osc_atom_false, "<="},
			{au, aU, osc_atom_le, ((int32_t)u <= U) ? osc_atom_true : osc_atom_false, "<="},
			{au, ai, osc_atom_le, ((int32_t)u <= i) ? osc_atom_true : osc_atom_false, "<="},
			{au, aI, osc_atom_le, ((int64_t)u <= I) ? osc_atom_true : osc_atom_false, "<="},
			{au, ah, osc_atom_le, ((int64_t)u <= h) ? osc_atom_true : osc_atom_false, "<="},
			{au, aH, osc_atom_le, osc_atom_undefined, "<="},
			{au, af, osc_atom_le, ((float)u <= f) ? osc_atom_true : osc_atom_false, "<="},
			{au, ad, osc_atom_le, ((double)u <= d) ? osc_atom_true : osc_atom_false, "<="},
			
			{aU, ac, osc_atom_le, (U <= (int32_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{aU, aC, osc_atom_le, (U <= (uint16_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{aU, au, osc_atom_le, ((int32_t)U <= (int32_t)u) ? osc_atom_true : osc_atom_false, "<="},
			{aU, aU, osc_atom_le, (U <= U) ? osc_atom_true : osc_atom_false, "<="},
			{aU, ai, osc_atom_le, ((int32_t)U <= i) ? osc_atom_true : osc_atom_false, "<="},
			{aU, aI, osc_atom_le, ((uint32_t)U <= I) ? osc_atom_true : osc_atom_false, "<="},
			{aU, ah, osc_atom_le, ((int64_t)U <= h) ? osc_atom_true : osc_atom_false, "<="},
			{aU, aH, osc_atom_le, ((uint64_t)U <= H) ? osc_atom_true : osc_atom_false, "<="},
			{aU, af, osc_atom_le, ((float)U <= f) ? osc_atom_true : osc_atom_false, "<="},
			{aU, ad, osc_atom_le, ((double)U <= d) ? osc_atom_true : osc_atom_false, "<="},

			{ai, ac, osc_atom_le, (i <= (int32_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{ai, aC, osc_atom_le, (i <= (int32_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{ai, au, osc_atom_le, (i <= (int32_t)u) ? osc_atom_true : osc_atom_false, "<="},
			{ai, aU, osc_atom_le, (i <= (int32_t)U) ? osc_atom_true : osc_atom_false, "<="},
			{ai, ai, osc_atom_le, (i <= i) ? osc_atom_true : osc_atom_false, "<="},
			{ai, aI, osc_atom_le, ((int64_t)i <= I) ? osc_atom_true : osc_atom_false, "<="},
			{ai, ah, osc_atom_le, ((int64_t)i <= h) ? osc_atom_true : osc_atom_false, "<="},
			{ai, aH, osc_atom_le, osc_atom_undefined, "<="},
			{ai, af, osc_atom_le, ((double)i <= f) ? osc_atom_true : osc_atom_false, "<="},
			{ai, ad, osc_atom_le, ((double)i <= d) ? osc_atom_true : osc_atom_false, "<="},

			{aI, ac, osc_atom_le, (I <= (int64_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{aI, aC, osc_atom_le, (I <= (uint32_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{aI, au, osc_atom_le, (I <= (int64_t)u) ? osc_atom_true : osc_atom_false, "<="},
			{aI, aU, osc_atom_le, (I <= (uint32_t)U) ? osc_atom_true : osc_atom_false, "<="},
			{aI, ai, osc_atom_le, (I <= (int64_t)i) ? osc_atom_true : osc_atom_false, "<="},
			{aI, aI, osc_atom_le, (I <= I) ? osc_atom_true : osc_atom_false, "<="},
			{aI, ah, osc_atom_le, ((int64_t)I <= h) ? osc_atom_true : osc_atom_false, "<="},
			{aI, aH, osc_atom_le, ((uint64_t)I <= H) ? osc_atom_true : osc_atom_false, "<="},
			{aI, af, osc_atom_le, ((double)I <= f) ? osc_atom_true : osc_atom_false, "<="},
			{aI, ad, osc_atom_le, ((double)I <= d) ? osc_atom_true : osc_atom_false, "<="},

			{ah, ac, osc_atom_le, (h <= (int64_t)c) ? osc_atom_true : osc_atom_false, "<="},
			{ah, aC, osc_atom_le, (h <= (int64_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{ah, au, osc_atom_le, (h <= (int64_t)u) ? osc_atom_true : osc_atom_false, "<="},
			{ah, aU, osc_atom_le, (h <= (int64_t)U) ? osc_atom_true : osc_atom_false, "<="},
			{ah, ai, osc_atom_le, (h <= (int64_t)i) ? osc_atom_true : osc_atom_false, "<="},
			{ah, aI, osc_atom_le, (h <= (int64_t)I) ? osc_atom_true : osc_atom_false, "<="},
			{ah, ah, osc_atom_le, (h <= h) ? osc_atom_true : osc_atom_false, "<="},
			{ah, aH, osc_atom_le, osc_atom_undefined, "<="},
			{ah, af, osc_atom_le, osc_atom_undefined, "<="},
			{ah, ad, osc_atom_le, osc_atom_undefined, "<="},

			{aH, ac, osc_atom_le, osc_atom_undefined, "<="},
			{aH, aC, osc_atom_le, (H <= (uint64_t)C) ? osc_atom_true : osc_atom_false, "<="},
			{aH, au, osc_atom_le, osc_atom_undefined, "<="},
			{aH, aU, osc_atom_le, (H <= (uint64_t)U) ? osc_atom_true : osc_atom_false, "<="},
			{aH, ai, osc_atom_le, osc_atom_undefined, "<="},
			{aH, aI, osc_atom_le, (H <= (uint64_t)I) ? osc_atom_true : osc_atom_false, "<="},
			{aH, ah, osc_atom_le, osc_atom_undefined, "<="},
			{aH, aH, osc_atom_le, (H <= (uint64_t)H) ? osc_atom_true : osc_atom_false, "<="},
			{aH, af, osc_atom_le, osc_atom_undefined, "<="},
			{aH, ad, osc_atom_le, osc_atom_undefined, "<="},

			{af, ac, osc_atom_le, (f <= (float)c) ? osc_atom_true : osc_atom_false, "<="},
			{af, aC, osc_atom_le, (f <= (float)C) ? osc_atom_true : osc_atom_false, "<="},
			{af, au, osc_atom_le, (f <= (float)u) ? osc_atom_true : osc_atom_false, "<="},
			{af, aU, osc_atom_le, (f <= (float)U) ? osc_atom_true : osc_atom_false, "<="},
			{af, ai, osc_atom_le, (f <= (double)i) ? osc_atom_true : osc_atom_false, "<="},
			{af, aI, osc_atom_le, (f <= (double)I) ? osc_atom_true : osc_atom_false, "<="},
			{af, ah, osc_atom_le, osc_atom_undefined, "<="},
			{af, aH, osc_atom_le, osc_atom_undefined, "<="},
			{af, af, osc_atom_le, (f <= f) ? osc_atom_true : osc_atom_false, "<="},
			{af, ad, osc_atom_le, ((double)f <= d) ? osc_atom_true : osc_atom_false, "<="},

			{ad, ac, osc_atom_le, (d <= (double)c) ? osc_atom_true : osc_atom_false, "<="},
			{ad, aC, osc_atom_le, (d <= (double)C) ? osc_atom_true : osc_atom_false, "<="},
			{ad, au, osc_atom_le, (d <= (double)u) ? osc_atom_true : osc_atom_false, "<="},
			{ad, aU, osc_atom_le, (d <= (double)U) ? osc_atom_true : osc_atom_false, "<="},
			{ad, ai, osc_atom_le, (d <= (double)i) ? osc_atom_true : osc_atom_false, "<="},
			{ad, aI, osc_atom_le, (d <= (double)I) ? osc_atom_true : osc_atom_false, "<="},
			{ad, ah, osc_atom_le, osc_atom_undefined, "<="},
			{ad, aH, osc_atom_le, osc_atom_undefined, "<="},
			{ad, af, osc_atom_le, (d <= (double)f) ? osc_atom_true : osc_atom_false, "<="},
			{ad, ad, osc_atom_le, (d <= d) ? osc_atom_true : osc_atom_false, "<="},

						{ac, ac, osc_atom_gt, (c > c) ? osc_atom_true : osc_atom_false, ">"},
			{ac, aC, osc_atom_gt, ((int16_t)c > C) ? osc_atom_true : osc_atom_false, ">"},
			{ac, au, osc_atom_gt, ((int16_t)c > u) ? osc_atom_true : osc_atom_false, ">"},
			{ac, aU, osc_atom_gt, ((int32_t)c > (int32_t)U) ? osc_atom_true : osc_atom_false, ">"},
			{ac, ai, osc_atom_gt, ((int32_t)c > i) ? osc_atom_true : osc_atom_false, ">"},
			{ac, aI, osc_atom_gt, ((int64_t)c > (int64_t)I) ? osc_atom_true : osc_atom_false, ">"},
			{ac, ah, osc_atom_gt, ((int64_t)c > h) ? osc_atom_true : osc_atom_false, ">"},
			{ac, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{ac, af, osc_atom_gt, ((float)c > f) ? osc_atom_true : osc_atom_false, ">"},
			{ac, ad, osc_atom_gt, ((double)c > d) ? osc_atom_true : osc_atom_false, ">"},

			{aC, ac, osc_atom_gt, ((int16_t)C > (int16_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{aC, aC, osc_atom_gt, (C > C) ? osc_atom_true : osc_atom_false, ">"},
			{aC, au, osc_atom_gt, ((int16_t)C > u) ? osc_atom_true : osc_atom_false, ">"},
			{aC, aU, osc_atom_gt, ((uint16_t)C > U) ? osc_atom_true : osc_atom_false, ">"},
			{aC, ai, osc_atom_gt, ((int32_t)C > i) ? osc_atom_true : osc_atom_false, ">"},
			{aC, aI, osc_atom_gt, ((uint32_t)C > I) ? osc_atom_true : osc_atom_false, ">"},
			{aC, ah, osc_atom_gt, ((int64_t)C > h) ? osc_atom_true : osc_atom_false, ">"},
			{aC, aH, osc_atom_gt, ((uint64_t)C > H) ? osc_atom_true : osc_atom_false, ">"},
			{aC, af, osc_atom_gt, ((float)C > f) ? osc_atom_true : osc_atom_false, ">"},
			{aC, ad, osc_atom_gt, ((double)C > d) ? osc_atom_true : osc_atom_false, ">"},
			 
			{au, ac, osc_atom_gt, (u > (int16_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{au, aC, osc_atom_gt, (u > (int16_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{au, au, osc_atom_gt, (u > u) ? osc_atom_true : osc_atom_false, ">"},
			{au, aU, osc_atom_gt, ((int32_t)u > U) ? osc_atom_true : osc_atom_false, ">"},
			{au, ai, osc_atom_gt, ((int32_t)u > i) ? osc_atom_true : osc_atom_false, ">"},
			{au, aI, osc_atom_gt, ((int64_t)u > I) ? osc_atom_true : osc_atom_false, ">"},
			{au, ah, osc_atom_gt, ((int64_t)u > h) ? osc_atom_true : osc_atom_false, ">"},
			{au, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{au, af, osc_atom_gt, ((float)u > f) ? osc_atom_true : osc_atom_false, ">"},
			{au, ad, osc_atom_gt, ((double)u > d) ? osc_atom_true : osc_atom_false, ">"},
			
			{aU, ac, osc_atom_gt, (U > (int32_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{aU, aC, osc_atom_gt, (U > (uint16_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{aU, au, osc_atom_gt, ((int32_t)U > (int32_t)u) ? osc_atom_true : osc_atom_false, ">"},
			{aU, aU, osc_atom_gt, (U > U) ? osc_atom_true : osc_atom_false, ">"},
			{aU, ai, osc_atom_gt, ((int32_t)U > i) ? osc_atom_true : osc_atom_false, ">"},
			{aU, aI, osc_atom_gt, ((uint32_t)U > I) ? osc_atom_true : osc_atom_false, ">"},
			{aU, ah, osc_atom_gt, ((int64_t)U > h) ? osc_atom_true : osc_atom_false, ">"},
			{aU, aH, osc_atom_gt, ((uint64_t)U > H) ? osc_atom_true : osc_atom_false, ">"},
			{aU, af, osc_atom_gt, ((float)U > f) ? osc_atom_true : osc_atom_false, ">"},
			{aU, ad, osc_atom_gt, ((double)U > d) ? osc_atom_true : osc_atom_false, ">"},

			{ai, ac, osc_atom_gt, (i > (int32_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{ai, aC, osc_atom_gt, (i > (int32_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{ai, au, osc_atom_gt, (i > (int32_t)u) ? osc_atom_true : osc_atom_false, ">"},
			{ai, aU, osc_atom_gt, (i > (int32_t)U) ? osc_atom_true : osc_atom_false, ">"},
			{ai, ai, osc_atom_gt, (i > i) ? osc_atom_true : osc_atom_false, ">"},
			{ai, aI, osc_atom_gt, ((int64_t)i > I) ? osc_atom_true : osc_atom_false, ">"},
			{ai, ah, osc_atom_gt, ((int64_t)i > h) ? osc_atom_true : osc_atom_false, ">"},
			{ai, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{ai, af, osc_atom_gt, ((double)i > f) ? osc_atom_true : osc_atom_false, ">"},
			{ai, ad, osc_atom_gt, ((double)i > d) ? osc_atom_true : osc_atom_false, ">"},

			{aI, ac, osc_atom_gt, (I > (int64_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{aI, aC, osc_atom_gt, (I > (uint32_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{aI, au, osc_atom_gt, (I > (int64_t)u) ? osc_atom_true : osc_atom_false, ">"},
			{aI, aU, osc_atom_gt, (I > (uint32_t)U) ? osc_atom_true : osc_atom_false, ">"},
			{aI, ai, osc_atom_gt, (I > (int64_t)i) ? osc_atom_true : osc_atom_false, ">"},
			{aI, aI, osc_atom_gt, (I > I) ? osc_atom_true : osc_atom_false, ">"},
			{aI, ah, osc_atom_gt, ((int64_t)I > h) ? osc_atom_true : osc_atom_false, ">"},
			{aI, aH, osc_atom_gt, ((uint64_t)I > H) ? osc_atom_true : osc_atom_false, ">"},
			{aI, af, osc_atom_gt, ((double)I > f) ? osc_atom_true : osc_atom_false, ">"},
			{aI, ad, osc_atom_gt, ((double)I > d) ? osc_atom_true : osc_atom_false, ">"},

			{ah, ac, osc_atom_gt, (h > (int64_t)c) ? osc_atom_true : osc_atom_false, ">"},
			{ah, aC, osc_atom_gt, (h > (int64_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{ah, au, osc_atom_gt, (h > (int64_t)u) ? osc_atom_true : osc_atom_false, ">"},
			{ah, aU, osc_atom_gt, (h > (int64_t)U) ? osc_atom_true : osc_atom_false, ">"},
			{ah, ai, osc_atom_gt, (h > (int64_t)i) ? osc_atom_true : osc_atom_false, ">"},
			{ah, aI, osc_atom_gt, (h > (int64_t)I) ? osc_atom_true : osc_atom_false, ">"},
			{ah, ah, osc_atom_gt, (h > h) ? osc_atom_true : osc_atom_false, ">"},
			{ah, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{ah, af, osc_atom_gt, osc_atom_undefined, ">"},
			{ah, ad, osc_atom_gt, osc_atom_undefined, ">"},

			{aH, ac, osc_atom_gt, osc_atom_undefined, ">"},
			{aH, aC, osc_atom_gt, (H > (uint64_t)C) ? osc_atom_true : osc_atom_false, ">"},
			{aH, au, osc_atom_gt, osc_atom_undefined, ">"},
			{aH, aU, osc_atom_gt, (H > (uint64_t)U) ? osc_atom_true : osc_atom_false, ">"},
			{aH, ai, osc_atom_gt, osc_atom_undefined, ">"},
			{aH, aI, osc_atom_gt, (H > (uint64_t)I) ? osc_atom_true : osc_atom_false, ">"},
			{aH, ah, osc_atom_gt, osc_atom_undefined, ">"},
			{aH, aH, osc_atom_gt, (H > (uint64_t)H) ? osc_atom_true : osc_atom_false, ">"},
			{aH, af, osc_atom_gt, osc_atom_undefined, ">"},
			{aH, ad, osc_atom_gt, osc_atom_undefined, ">"},

			{af, ac, osc_atom_gt, (f > (float)c) ? osc_atom_true : osc_atom_false, ">"},
			{af, aC, osc_atom_gt, (f > (float)C) ? osc_atom_true : osc_atom_false, ">"},
			{af, au, osc_atom_gt, (f > (float)u) ? osc_atom_true : osc_atom_false, ">"},
			{af, aU, osc_atom_gt, (f > (float)U) ? osc_atom_true : osc_atom_false, ">"},
			{af, ai, osc_atom_gt, (f > (double)i) ? osc_atom_true : osc_atom_false, ">"},
			{af, aI, osc_atom_gt, (f > (double)I) ? osc_atom_true : osc_atom_false, ">"},
			{af, ah, osc_atom_gt, osc_atom_undefined, ">"},
			{af, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{af, af, osc_atom_gt, (f > f) ? osc_atom_true : osc_atom_false, ">"},
			{af, ad, osc_atom_gt, ((double)f > d) ? osc_atom_true : osc_atom_false, ">"},

			{ad, ac, osc_atom_gt, (d > (double)c) ? osc_atom_true : osc_atom_false, ">"},
			{ad, aC, osc_atom_gt, (d > (double)C) ? osc_atom_true : osc_atom_false, ">"},
			{ad, au, osc_atom_gt, (d > (double)u) ? osc_atom_true : osc_atom_false, ">"},
			{ad, aU, osc_atom_gt, (d > (double)U) ? osc_atom_true : osc_atom_false, ">"},
			{ad, ai, osc_atom_gt, (d > (double)i) ? osc_atom_true : osc_atom_false, ">"},
			{ad, aI, osc_atom_gt, (d > (double)I) ? osc_atom_true : osc_atom_false, ">"},
			{ad, ah, osc_atom_gt, osc_atom_undefined, ">"},
			{ad, aH, osc_atom_gt, osc_atom_undefined, ">"},
			{ad, af, osc_atom_gt, (d > (double)f) ? osc_atom_true : osc_atom_false, ">"},
			{ad, ad, osc_atom_gt, (d > d) ? osc_atom_true : osc_atom_false, ">"},

						{ac, ac, osc_atom_ge, (c >= c) ? osc_atom_true : osc_atom_false, ">="},
			{ac, aC, osc_atom_ge, ((int16_t)c >= C) ? osc_atom_true : osc_atom_false, ">="},
			{ac, au, osc_atom_ge, ((int16_t)c >= u) ? osc_atom_true : osc_atom_false, ">="},
			{ac, aU, osc_atom_ge, ((int32_t)c >= (int32_t)U) ? osc_atom_true : osc_atom_false, ">="},
			{ac, ai, osc_atom_ge, ((int32_t)c >= i) ? osc_atom_true : osc_atom_false, ">="},
			{ac, aI, osc_atom_ge, ((int64_t)c >= (int64_t)I) ? osc_atom_true : osc_atom_false, ">="},
			{ac, ah, osc_atom_ge, ((int64_t)c >= h) ? osc_atom_true : osc_atom_false, ">="},
			{ac, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{ac, af, osc_atom_ge, ((float)c >= f) ? osc_atom_true : osc_atom_false, ">="},
			{ac, ad, osc_atom_ge, ((double)c >= d) ? osc_atom_true : osc_atom_false, ">="},

			{aC, ac, osc_atom_ge, ((int16_t)C >= (int16_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{aC, aC, osc_atom_ge, (C >= C) ? osc_atom_true : osc_atom_false, ">="},
			{aC, au, osc_atom_ge, ((int16_t)C >= u) ? osc_atom_true : osc_atom_false, ">="},
			{aC, aU, osc_atom_ge, ((uint16_t)C >= U) ? osc_atom_true : osc_atom_false, ">="},
			{aC, ai, osc_atom_ge, ((int32_t)C >= i) ? osc_atom_true : osc_atom_false, ">="},
			{aC, aI, osc_atom_ge, ((uint32_t)C >= I) ? osc_atom_true : osc_atom_false, ">="},
			{aC, ah, osc_atom_ge, ((int64_t)C >= h) ? osc_atom_true : osc_atom_false, ">="},
			{aC, aH, osc_atom_ge, ((uint64_t)C >= H) ? osc_atom_true : osc_atom_false, ">="},
			{aC, af, osc_atom_ge, ((float)C >= f) ? osc_atom_true : osc_atom_false, ">="},
			{aC, ad, osc_atom_ge, ((double)C >= d) ? osc_atom_true : osc_atom_false, ">="},
			 
			{au, ac, osc_atom_ge, (u >= (int16_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{au, aC, osc_atom_ge, (u >= (int16_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{au, au, osc_atom_ge, (u >= u) ? osc_atom_true : osc_atom_false, ">="},
			{au, aU, osc_atom_ge, ((int32_t)u >= U) ? osc_atom_true : osc_atom_false, ">="},
			{au, ai, osc_atom_ge, ((int32_t)u >= i) ? osc_atom_true : osc_atom_false, ">="},
			{au, aI, osc_atom_ge, ((int64_t)u >= I) ? osc_atom_true : osc_atom_false, ">="},
			{au, ah, osc_atom_ge, ((int64_t)u >= h) ? osc_atom_true : osc_atom_false, ">="},
			{au, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{au, af, osc_atom_ge, ((float)u >= f) ? osc_atom_true : osc_atom_false, ">="},
			{au, ad, osc_atom_ge, ((double)u >= d) ? osc_atom_true : osc_atom_false, ">="},
			
			{aU, ac, osc_atom_ge, (U >= (int32_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{aU, aC, osc_atom_ge, (U >= (uint16_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{aU, au, osc_atom_ge, ((int32_t)U >= (int32_t)u) ? osc_atom_true : osc_atom_false, ">="},
			{aU, aU, osc_atom_ge, (U >= U) ? osc_atom_true : osc_atom_false, ">="},
			{aU, ai, osc_atom_ge, ((int32_t)U >= i) ? osc_atom_true : osc_atom_false, ">="},
			{aU, aI, osc_atom_ge, ((uint32_t)U >= I) ? osc_atom_true : osc_atom_false, ">="},
			{aU, ah, osc_atom_ge, ((int64_t)U >= h) ? osc_atom_true : osc_atom_false, ">="},
			{aU, aH, osc_atom_ge, ((uint64_t)U >= H) ? osc_atom_true : osc_atom_false, ">="},
			{aU, af, osc_atom_ge, ((float)U >= f) ? osc_atom_true : osc_atom_false, ">="},
			{aU, ad, osc_atom_ge, ((double)U >= d) ? osc_atom_true : osc_atom_false, ">="},

			{ai, ac, osc_atom_ge, (i >= (int32_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{ai, aC, osc_atom_ge, (i >= (int32_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{ai, au, osc_atom_ge, (i >= (int32_t)u) ? osc_atom_true : osc_atom_false, ">="},
			{ai, aU, osc_atom_ge, (i >= (int32_t)U) ? osc_atom_true : osc_atom_false, ">="},
			{ai, ai, osc_atom_ge, (i >= i) ? osc_atom_true : osc_atom_false, ">="},
			{ai, aI, osc_atom_ge, ((int64_t)i >= I) ? osc_atom_true : osc_atom_false, ">="},
			{ai, ah, osc_atom_ge, ((int64_t)i >= h) ? osc_atom_true : osc_atom_false, ">="},
			{ai, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{ai, af, osc_atom_ge, ((double)i >= f) ? osc_atom_true : osc_atom_false, ">="},
			{ai, ad, osc_atom_ge, ((double)i >= d) ? osc_atom_true : osc_atom_false, ">="},

			{aI, ac, osc_atom_ge, (I >= (int64_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{aI, aC, osc_atom_ge, (I >= (uint32_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{aI, au, osc_atom_ge, (I >= (int64_t)u) ? osc_atom_true : osc_atom_false, ">="},
			{aI, aU, osc_atom_ge, (I >= (uint32_t)U) ? osc_atom_true : osc_atom_false, ">="},
			{aI, ai, osc_atom_ge, (I >= (int64_t)i) ? osc_atom_true : osc_atom_false, ">="},
			{aI, aI, osc_atom_ge, (I >= I) ? osc_atom_true : osc_atom_false, ">="},
			{aI, ah, osc_atom_ge, ((int64_t)I >= h) ? osc_atom_true : osc_atom_false, ">="},
			{aI, aH, osc_atom_ge, ((uint64_t)I >= H) ? osc_atom_true : osc_atom_false, ">="},
			{aI, af, osc_atom_ge, ((double)I >= f) ? osc_atom_true : osc_atom_false, ">="},
			{aI, ad, osc_atom_ge, ((double)I >= d) ? osc_atom_true : osc_atom_false, ">="},

			{ah, ac, osc_atom_ge, (h >= (int64_t)c) ? osc_atom_true : osc_atom_false, ">="},
			{ah, aC, osc_atom_ge, (h >= (int64_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{ah, au, osc_atom_ge, (h >= (int64_t)u) ? osc_atom_true : osc_atom_false, ">="},
			{ah, aU, osc_atom_ge, (h >= (int64_t)U) ? osc_atom_true : osc_atom_false, ">="},
			{ah, ai, osc_atom_ge, (h >= (int64_t)i) ? osc_atom_true : osc_atom_false, ">="},
			{ah, aI, osc_atom_ge, (h >= (int64_t)I) ? osc_atom_true : osc_atom_false, ">="},
			{ah, ah, osc_atom_ge, (h >= h) ? osc_atom_true : osc_atom_false, ">="},
			{ah, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{ah, af, osc_atom_ge, osc_atom_undefined, ">="},
			{ah, ad, osc_atom_ge, osc_atom_undefined, ">="},

			{aH, ac, osc_atom_ge, osc_atom_undefined, ">="},
			{aH, aC, osc_atom_ge, (H >= (uint64_t)C) ? osc_atom_true : osc_atom_false, ">="},
			{aH, au, osc_atom_ge, osc_atom_undefined, ">="},
			{aH, aU, osc_atom_ge, (H >= (uint64_t)U) ? osc_atom_true : osc_atom_false, ">="},
			{aH, ai, osc_atom_ge, osc_atom_undefined, ">="},
			{aH, aI, osc_atom_ge, (H >= (uint64_t)I) ? osc_atom_true : osc_atom_false, ">="},
			{aH, ah, osc_atom_ge, osc_atom_undefined, ">="},
			{aH, aH, osc_atom_ge, (H >= (uint64_t)H) ? osc_atom_true : osc_atom_false, ">="},
			{aH, af, osc_atom_ge, osc_atom_undefined, ">="},
			{aH, ad, osc_atom_ge, osc_atom_undefined, ">="},

			{af, ac, osc_atom_ge, (f >= (float)c) ? osc_atom_true : osc_atom_false, ">="},
			{af, aC, osc_atom_ge, (f >= (float)C) ? osc_atom_true : osc_atom_false, ">="},
			{af, au, osc_atom_ge, (f >= (float)u) ? osc_atom_true : osc_atom_false, ">="},
			{af, aU, osc_atom_ge, (f >= (float)U) ? osc_atom_true : osc_atom_false, ">="},
			{af, ai, osc_atom_ge, (f >= (double)i) ? osc_atom_true : osc_atom_false, ">="},
			{af, aI, osc_atom_ge, (f >= (double)I) ? osc_atom_true : osc_atom_false, ">="},
			{af, ah, osc_atom_ge, osc_atom_undefined, ">="},
			{af, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{af, af, osc_atom_ge, (f >= f) ? osc_atom_true : osc_atom_false, ">="},
			{af, ad, osc_atom_ge, ((double)f >= d) ? osc_atom_true : osc_atom_false, ">="},

			{ad, ac, osc_atom_ge, (d >= (double)c) ? osc_atom_true : osc_atom_false, ">="},
			{ad, aC, osc_atom_ge, (d >= (double)C) ? osc_atom_true : osc_atom_false, ">="},
			{ad, au, osc_atom_ge, (d >= (double)u) ? osc_atom_true : osc_atom_false, ">="},
			{ad, aU, osc_atom_ge, (d >= (double)U) ? osc_atom_true : osc_atom_false, ">="},
			{ad, ai, osc_atom_ge, (d >= (double)i) ? osc_atom_true : osc_atom_false, ">="},
			{ad, aI, osc_atom_ge, (d >= (double)I) ? osc_atom_true : osc_atom_false, ">="},
			{ad, ah, osc_atom_ge, osc_atom_undefined, ">="},
			{ad, aH, osc_atom_ge, osc_atom_undefined, ">="},
			{ad, af, osc_atom_ge, (d >= (double)f) ? osc_atom_true : osc_atom_false, ">="},
			{ad, ad, osc_atom_ge, (d >= d) ? osc_atom_true : osc_atom_false, ">="},
		};
		PRINT_TEST_HEADER("binops");
		char *dots = "................................................................";
		int padlength = 32;
		for(int i = 0; i < sizeof(optest) / sizeof(struct _optest); i++){
			t_osc_atom *res = optest[i].fn(optest[i].lhs, optest[i].rhs);
			osc_atom_format_m((t_osc_atom_m *)(optest[i].result), 0);
			osc_atom_format_m((t_osc_atom_m *)(res), 0);
			osc_atom_format_m((t_osc_atom_m *)(optest[i].lhs), 0);
			osc_atom_format_m((t_osc_atom_m *)(optest[i].rhs), 0);
			PRINT_TEST("%s (%c) %s %s (%c) = %s (%c):", osc_atom_getPrettyPtr(optest[i].lhs), osc_atom_getTypetag(optest[i].lhs), optest[i].opstring, osc_atom_getPrettyPtr(optest[i].rhs), osc_atom_getTypetag(optest[i].rhs), osc_atom_getPrettyPtr(res), osc_atom_getTypetag(res));
			if(osc_atom_eql(res, optest[i].result) == osc_atom_false){
				PRINT_FAILED("Expected %s (%c) but got %s (%c)\n", osc_atom_getPrettyPtr(optest[i].result), osc_atom_getTypetag(optest[i].result), osc_atom_getPrettyPtr(res), osc_atom_getTypetag(res));
			}else{
				PRINT_PASSED
			}
			
			osc_atom_release(res);
			osc_atom_release(optest[i].result);
		}
	}

	for(int i = 0; i < sizeof(atoms) / sizeof(t_osc_atom*); i++){
		osc_atom_release(atoms[i]);
		if(blobs[i]){
			osc_mem_free(blobs[i]);
		}
	}
	osc_mem_free(blobs);
	t_osc_test_ret r = {faultcount};
	return r;
}
