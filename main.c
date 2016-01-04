/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Romain Bouqueau
 *			Copyright (c) Romain Bouqueau 2016
 *					All rights reserved
 *
 *  This file is part of GPAC - sample GPAC player with shared OpenGL
 *  shared context
 *
 */

#include "player.h"
#include <stdio.h>

#ifdef WIN32
#include <malloc.h>
#define alloca _alloca
#endif

int main(int argc, char **argv)
{
	int ret = 0;
	char *fn;
	const char *gpac360 = "#LIVE360TV";

	if (argc != 2) {
		fprintf(stderr, "USAGE: %s file\n", argv[0]);
		ret = 1;
		goto exit;
	}

#ifdef GPAC_MEMORY_TRACKING
	gf_sys_init(GF_TRUE);
#else
	gf_sys_init(GF_FALSE);
#endif
	gf_log_set_tool_level(GF_LOG_ALL, GF_LOG_WARNING);

	fn = (char*)alloca(strlen(argv[1]) + strlen(gpac360) + 1);
	sprintf(fn, "%s%s", argv[1], gpac360);
	if (!player(fn)) {
		ret = 1;
		goto exit;
	}

exit:
	gf_sys_close();
	return ret;
}
