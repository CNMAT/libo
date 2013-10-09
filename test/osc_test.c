#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_bundle_u.h"
#include "osc_strfmt.h"
#include "osc_parser.h"
#define __OSC_PROFILE__
#include "osc_profile.h"

int main(int len, char **argv)
{
	char *st = "/foo [\n/bar 1 2 3\n]\n";
	t_osc_bndl_u *bndl = NULL;
	t_osc_parser_subst *subs = NULL;
	long nsubs = 0;
	t_osc_err e = osc_parser_parseString(strlen(st), st, &bndl, &nsubs, &subs);

	long sbndllen = 0;
	char *sbndl = NULL;
	osc_bundle_u_serialize(bndl, &sbndllen, &sbndl);

	long flen = osc_bundle_u_nformat(NULL, 0, bndl, 0);
	char fbuf[flen + 1];
	osc_bundle_u_nformat(fbuf, flen + 1, bndl, 0);
	/*
	long flen = 0;
	char *fbuf = NULL;
	osc_bundle_s_format(sbndllen, sbndl, &flen, &fbuf);
	*/
	printf("%s\n", fbuf);
	for(int i = 0; i < flen; i++){
		printf("%d: %c %d\n", i, fbuf[i], fbuf[i]);
	}
}
