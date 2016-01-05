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
#include <gl\gl.h>
#include <gl\glu.h>
#include <vfw.h> //Romain
#include "gl_window.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "vfw32.lib" )

GL_Window *g_window;

AVISTREAMINFO psi;
PAVISTREAM pavi;
PGETFRAME pgf;
BITMAPINFOHEADER bmih;
int width;
int height;
char *pdata;
int mpf;

HDRAWDIB hdd;													
HBITMAP hBitmap;												
HDC hdc;
unsigned char *data = 0;										

void OpenAVI(LPCSTR szFile)										
{
	TCHAR	title[100];
	hdc = CreateCompatibleDC(0);

	AVIFileInit();												

	if (AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0) {
		MessageBox(HWND_DESKTOP, "Failed To Open The AVI Stream", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	AVIStreamInfo(pavi, &psi, sizeof(psi));						
	width=psi.rcFrame.right-psi.rcFrame.left;					
	height=psi.rcFrame.bottom-psi.rcFrame.top;					

	bmih.biSize = sizeof(BITMAPINFOHEADER);					
	bmih.biPlanes = 1;											
	bmih.biBitCount = 24;										
	bmih.biWidth = 256;											
	bmih.biHeight = 256;										
	bmih.biCompression = BI_RGB;								

	hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, 0);
	SelectObject (hdc, hBitmap);								

	pgf=AVIStreamGetFrameOpen(pavi, NULL);
	if (pgf==NULL) {
		MessageBox (HWND_DESKTOP, "Failed To Open The AVI Frame", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	wsprintf (title, "GPAC OpenGL texture");
	SetWindowText(g_window->hWnd, title);
}

void GrabAVIFrame(int frame)									
{
	LPBITMAPINFOHEADER lpbi;									
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);	
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed  *sizeof(RGBQUAD);	

	DrawDibDraw (hdd, hdc, 0, 0, 256, 256, lpbi, pdata, 0, 0, width, height, 0);

	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void CloseAVI()
{
	DeleteObject(hBitmap);										
	DrawDibClose(hdd);											
	AVIStreamGetFrameClose(pgf);								
	AVIStreamRelease(pavi);										
	AVIFileExit();												
}

BOOL Initialize(GL_Window *window)
{
	g_window = window;

	hdd = DrawDibOpen();
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	OpenAVI("data/face2.avi");									

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	return TRUE;
}

void Deinitialize(void)
{
	CloseAVI();
}

void Update()
{
	//Romain: check 'q' with GPAC
	//TerminateApplication(g_window);
}

void Draw(void)
{
	static int frame = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GrabAVIFrame(frame++);

	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 11.0f,  8.3f, -20.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-11.0f,  8.3f, -20.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-11.0f, -8.3f, -20.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 11.0f, -8.3f, -20.0f);
	glEnd();

	glFlush();
}
