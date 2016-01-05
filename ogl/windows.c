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

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "gl_window.h"

static volatile BOOL g_isProgramLooping;

void TerminateApplication(GL_Window *window)
{
	PostMessage(window->hWnd, WM_QUIT, 0, 0);
	g_isProgramLooping = FALSE;
}

void ReshapeGL(int width, int height)
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)(width)/(GLfloat)(height),	1.0f, 100.0f);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel)	
{
	DEVMODE dmScreenSettings;
	ZeroMemory(&dmScreenSettings, sizeof (DEVMODE));
	dmScreenSettings.dmSize = sizeof (DEVMODE);
	dmScreenSettings.dmPelsWidth = width;
	dmScreenSettings.dmPelsHeight = height;
	dmScreenSettings.dmBitsPerPel = bitsPerPixel;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return TRUE;
}

BOOL CreateWindowGL(GL_Window *window)
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	PIXELFORMATDESCRIPTOR pfd =	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		window->init.bitsPerPixel,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	RECT windowRect = {0, 0, window->init.width, window->init.height};	

	GLuint PixelFormat;

	AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);

	window->hWnd = CreateWindowEx(windowExtendedStyle,
		window->init.application->className,
		window->init.title,
		windowStyle,
		0, 0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		HWND_DESKTOP,
		0,
		window->init.application->hInstance,
		window);

	if (window->hWnd == 0) {
		return FALSE;
	}

	window->hDC = GetDC(window->hWnd);
	if (window->hDC == 0) {
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	PixelFormat = ChoosePixelFormat(window->hDC, &pfd);				
	if (PixelFormat == 0) {
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	if (SetPixelFormat(window->hDC, PixelFormat, &pfd) == FALSE) {
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;					
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	window->hRC = wglCreateContext(window->hDC);
	if (window->hRC == 0)	{
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	if (wglMakeCurrent(window->hDC, window->hRC) == FALSE) {
		wglDeleteContext(window->hRC);
		window->hRC = 0;
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	ShowWindow(window->hWnd, SW_NORMAL);
	window->isVisible = TRUE;

	ReshapeGL(window->init.width, window->init.height);

	return TRUE;
}

BOOL DestroyWindowGL(GL_Window *window)
{
	if (window->hWnd != 0) {	
		if (window->hDC != 0) {
			wglMakeCurrent(window->hDC, 0);
			if (window->hRC != 0) {
				wglDeleteContext(window->hRC);
				window->hRC = 0;
			}
			ReleaseDC(window->hWnd, window->hDC);
			window->hDC = 0;
		}
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
	}

	return TRUE;
}

#define GWL_USERDATA (-21) //FIXME
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GL_Window *window = (GL_Window*)(GetWindowLong(hWnd, GWL_USERDATA));

	switch (uMsg) {
	case WM_CREATE:
		{
			CREATESTRUCT *creation = (CREATESTRUCT*)(lParam);
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;
	case WM_CLOSE:
		TerminateApplication(window);
		return 0;
	case WM_SIZE:
		switch (wParam) {
		case SIZE_MINIMIZED:
			window->isVisible = FALSE;
			return 0;
		case SIZE_MAXIMIZED:
			window->isVisible = TRUE;
			ReshapeGL(LOWORD (lParam), HIWORD (lParam));
			return 0;
		case SIZE_RESTORED:
			window->isVisible = TRUE;
			ReshapeGL(LOWORD (lParam), HIWORD (lParam));
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);					
}

BOOL RegisterWindowClass(Application *application)						
{
	WNDCLASSEX windowClass;												
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						
	windowClass.cbSize = sizeof (WNDCLASSEX);					
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	windowClass.lpfnWndProc = (WNDPROC)(WindowProc);				
	windowClass.hInstance = application->hInstance;				
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);			
	windowClass.lpszClassName	= application->className;				
	if (RegisterClassEx(&windowClass) == 0) {
		MessageBox (HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													
	}
	return TRUE;														
}

int create_window()
{
	Application application;									
	GL_Window window;						
	BOOL isMessagePumpActive;							
	MSG msg;											
	DWORD tickCount;										

	application.className = "OpenGL";									
	application.hInstance = GetModuleHandle("GPAC OpenGL texture");									

	ZeroMemory(&window, sizeof(GL_Window));
	window.init.application = &application;							
	window.init.title = "GPAC OpenGL texture";			
	window.init.width = 640;									
	window.init.height = 480;									
	window.init.bitsPerPixel = 16;								
	
	if (RegisterWindowClass(&application) == FALSE) {
		MessageBox(HWND_DESKTOP, "Error Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														
	}

	g_isProgramLooping = TRUE;
	while (g_isProgramLooping) {
		if (CreateWindowGL(&window) == TRUE) {
			if (Initialize(&window) == FALSE) {
				TerminateApplication(&window);
			} else {
				isMessagePumpActive = TRUE;
				while (isMessagePumpActive == TRUE)	{
					if (PeekMessage(&msg, window.hWnd, 0, 0, PM_REMOVE) != 0) {
						if (msg.message != WM_QUIT) {
							DispatchMessage(&msg);
						} else {
							isMessagePumpActive = FALSE;
						}
					} else {
						if (window.isVisible == FALSE) {
							WaitMessage();
						} else {
							tickCount = GetTickCount();
							Update();
							Draw();
							SwapBuffers(window.hDC);
						}
					}
				}
			}

			Deinitialize();
			DestroyWindowGL(&window);
		} else {
			MessageBox(HWND_DESKTOP, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;
		}
	}

	UnregisterClass(application.className, application.hInstance);
	return 0;
}
