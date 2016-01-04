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
#include "opengl_shared_module.h"
#include <gpac/list.h>
#include <gpac/modules/video_out.h>

static GF_Err Setup(struct _video_out *vout, void *os_handle, void *os_display, u32 init_flags)
{
	return GF_OK;
}

static void *NewOpenGLSharedVideoOutput()
{
	GF_VideoOutput *driv = (GF_VideoOutput *) gf_malloc(sizeof(GF_VideoOutput));
	memset(driv, 0, sizeof(GF_VideoOutput));
	GF_REGISTER_MODULE_INTERFACE(driv, GF_VIDEO_OUTPUT_INTERFACE, OPENGL_SHARED_MODULE_NAME_STR, "gpac distribution");

	driv->Setup = Setup;
#if 0 //not implemented yet
	driv->opaque = pCtx;
	driv->Flush = DD_Flush;
	driv->Shutdown = DD_Shutdown;
	driv->SetFullScreen = DD_SetFullScreen;
	driv->ProcessEvent = DD_ProcessEvent;
#endif

	return (void *)driv;
}

static void DeleteVideoOutput(void *ifce)
{
	GF_VideoOutput *driv = (GF_VideoOutput *) ifce;
	gf_free(driv);
}

/*interface query*/
GPAC_MODULE_EXPORT
const u32 *QueryInterfaces()
{
	static u32 si [] = {
		GF_VIDEO_OUTPUT_INTERFACE,
		0
	};
	return si;
}

/*interface create*/
GPAC_MODULE_EXPORT
GF_BaseInterface *LoadInterface(u32 InterfaceType)
{
	if (InterfaceType == GF_VIDEO_OUTPUT_INTERFACE)
		return (GF_BaseInterface*)NewOpenGLSharedVideoOutput();
	else
		return NULL;
}

/*interface destroy*/
GPAC_MODULE_EXPORT
void ShutdownInterface(GF_BaseInterface *ifce)
{
	switch (ifce->InterfaceType) {
	case GF_VIDEO_OUTPUT_INTERFACE:
		DeleteVideoOutput((GF_VideoOutput *)ifce);
		break;
	}
}

GPAC_MODULE_STATIC_DECLARATION( opengl_shared_out )

#define GET_MODULE_REGISTER_FN(__name) gf_register_module_##__name

GF_Err manually_register_opengl_shared_module(GF_ModuleManager *mgr)
{
	GF_Err e = gf_module_load_static(mgr, GET_MODULE_REGISTER_FN(opengl_shared_out));
	gf_modules_refresh(mgr);
	return e;
}
