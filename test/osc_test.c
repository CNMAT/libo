#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_match.h"

int main(int len, char **argv)
{
	//char *pattern = "/*cd{e,ee,eee}ef/{bar,car}/barf";
	//char *address = "/abcdcdeeef/bar";
	char *pattern = "/a*{cc}d";
	char *address = "/abccd";
	int po = 0, ao = 0;
	int ret = osc_match(pattern, address, &po, &ao);
	printf("ret = %d, po = %d, ao = %d\n", ret, po, ao);
	printf("%s %s\n", pattern + po, address + ao);
}
