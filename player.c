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

#include "opengl_shared_module.h"
#include "opt.h"
#include <gpac/terminal.h>

volatile Bool connected = GF_FALSE;

Bool GPAC_EventProc(void *ptr, GF_Event *evt)
{
	switch (evt->type) {
	case GF_EVENT_CONNECT:
		connected = GF_TRUE;
		break;
	case GF_EVENT_DISCONNECT:
	case GF_EVENT_QUIT:
		connected = GF_FALSE;
		break;
	default:
		break;
	}
  
	return GF_FALSE;
}

Bool player(char *fn)
{
	Bool ret = GF_FALSE;
	GF_Err e;
	GF_User user;
	GF_Terminal *term = NULL;
	GF_Config *cfg_file;
	memset(&user, 0, sizeof(user));
	cfg_file = gf_cfg_init(NULL, NULL);
	user.modules = gf_modules_new(NULL, cfg_file);
	e = manually_register_opengl_shared_module(user.modules);
	if (e != GF_OK) goto exit;
	set_cfg_option(cfg_file, "Video:DriverName=" OPENGL_SHARED_MODULE_NAME_STR);
	user.config = cfg_file;
	user.opaque = &user;
	user.EventProc = GPAC_EventProc;
	term = gf_term_new(&user);
	if (!term) goto exit;

	gf_term_connect(term, fn);
	while (!connected) gf_sleep(1);
	while ( connected) {
		gf_term_process_step(term);
		if (gf_prompt_has_input()) {
			switch (gf_prompt_get_char()) {
			case 'q':
				{
					GF_Event evt;
					memset(&evt, 0, sizeof(GF_Event));
					evt.type = GF_EVENT_QUIT;
					gf_term_send_event(term, &evt);
				}
				break;
			}
		}
	}
	ret = GF_TRUE;

exit:
	gf_term_disconnect(term);
	gf_term_del(term);
	gf_modules_del(user.modules);
	gf_cfg_del(cfg_file);

	return ret;
}
