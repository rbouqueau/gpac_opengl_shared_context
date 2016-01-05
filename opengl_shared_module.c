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

#define GPAC_STATIC_MODULES
#include "dx_hw_wrapper.h"
#include "opengl_shared_module.h"
#include <gpac/list.h>
#include <gpac/modules/video_out.h>


/*interface query*/
GPAC_MODULE_EXPORT
const u32 *QueryInterfaces()
{
	static u32 si [] = {
		GF_VIDEO_OUTPUT_INTERFACE,
		0
	};
	assert((sizeof(si) == sizeof(u32)*2) && (si[0]==GF_VIDEO_OUTPUT_INTERFACE) && (si[1]==0));
	return si;
}

/*interface create*/
GPAC_MODULE_EXPORT
GF_BaseInterface *LoadInterface(u32 InterfaceType)
{
	GF_BaseInterface *ifce = NULL;
	if (InterfaceType == GF_VIDEO_OUTPUT_INTERFACE) {
		GF_InterfaceRegister *reg = gf_register_module_dx_out();
		ifce = reg->LoadInterface(InterfaceType);
		GF_REGISTER_MODULE_INTERFACE(ifce, GF_VIDEO_OUTPUT_INTERFACE, OPENGL_SHARED_MODULE_NAME_STR, "gpac distribution");
		gf_free(reg);
	}

	return ifce;
}

/*interface destroy*/
GPAC_MODULE_EXPORT
void ShutdownInterface(GF_BaseInterface *ifce)
{
	GF_InterfaceRegister *reg = gf_register_module_dx_out();
	switch (ifce->InterfaceType) {
	case GF_VIDEO_OUTPUT_INTERFACE:
		reg->ShutdownInterface(ifce);
		break;
	}
	gf_free(reg);
}

GPAC_MODULE_STATIC_DECLARATION( opengl_shared_out )

GF_Err manually_register_opengl_shared_module(GF_ModuleManager *mgr)
{
	GF_Err e = gf_module_load_static(mgr, gf_register_module_opengl_shared_out);
	gf_modules_refresh(mgr);
	return e;
}
