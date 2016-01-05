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

#pragma once

#include <windows.h>

typedef struct {
	HINSTANCE hInstance;
	const char *className;
} Application;

typedef struct {
	Application *application;
	char *title;
	int width;
	int height;
	int bitsPerPixel;
} GL_WindowInit;

typedef struct {
	HWND          hWnd;
	HDC           hDC;
	HGLRC         hRC;
	GL_WindowInit init;
	BOOL          isVisible;
} GL_Window;

void TerminateApplication(GL_Window *window);

BOOL Initialize(GL_Window *window);
void Deinitialize(void);
void Update();
void Draw(void);
