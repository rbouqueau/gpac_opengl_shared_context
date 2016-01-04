/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Romain Bouqueau
 *			Copyright (c) Romain Bouqueau - Telecom ParisTech 2016
 *					All rights reserved
 *
 *  This file is part of GPAC - sample GPAC player with shared OpenGL
 *  shared context
 *
 */

#include "opt.h"
#include <stdio.h>
#include <string.h>

//copied from MP4Client
void set_cfg_option(GF_Config *cfg_file, const char *opt_string)
{
	char *sep, *sep2, szSec[1024], szKey[1024], szVal[1024];
	sep = strchr(opt_string, ':');
	if (!sep) {
		fprintf(stderr, "Badly formatted option %s - expected Section:Name=Value\n", opt_string);
		return;
	}
	sep[0] = 0;
	strcpy(szSec, opt_string);
	sep[0] = ':';
	sep ++;
	sep2 = strchr(sep, '=');
	if (!sep2) {
		fprintf(stderr, "Badly formatted option %s - expected Section:Name=Value\n", opt_string);
		return;
	}
	sep2[0] = 0;
	strcpy(szKey, sep);
	strcpy(szVal, sep2+1);
	sep2[0] = '=';
	
	if (!stricmp(szKey, "*")) {
		if (stricmp(szVal, "null")) {
			fprintf(stderr, "Badly formatted option %s - expected Section:*=null\n", opt_string);
			return;
		}
		gf_cfg_del_section(cfg_file, szSec);
		return;
	}
	
	if (!stricmp(szVal, "null")) {
		szVal[0]=0;
	}
	gf_cfg_set_key(cfg_file, szSec, szKey, szVal[0] ? szVal : NULL);
}
