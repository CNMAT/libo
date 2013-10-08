#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_bundle_u.h"
#include "osc_strfmt.h"



int main(int len, char **argv)
{
	char *str = "ab\"c\"";
	int n = osc_strfmt_quotedString(NULL, 0, str);
	int nn = osc_strfmt_stringWithQuotedMeta(NULL, 0, str);
	int nnn = osc_strfmt_quotedStringWithQuotedMeta(NULL, 0, str);
	printf("%d %d %d\n", n, nn, nnn);

	int x = 3;
	int xx = 10;
	char s[n + xx];
	char ss[nn + xx];
	char sss[nnn + xx];

	n = osc_strfmt_quotedString(s, n + xx, str);
	nn = osc_strfmt_stringWithQuotedMeta(ss, nn + xx, str);
	nnn = osc_strfmt_quotedStringWithQuotedMeta(sss, nnn + xx, str);

	printf("%d: %s\n", n, s);
	printf("%d: %s\n", nn, ss);
	printf("%d: %s\n", nnn, sss);
}
