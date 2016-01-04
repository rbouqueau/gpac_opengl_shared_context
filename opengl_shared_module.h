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

#include <gpac/module.h>

#define OPENGL_SHARED_MODULE_NAME_STR "OpenGL-Shared Video Output"

GF_Err manually_register_opengl_shared_module(GF_ModuleManager *mgr);
