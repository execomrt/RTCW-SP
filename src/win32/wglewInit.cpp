/*
** The OpenGL Extension Wrangler Library
** Copyright (C) 2008-2015, Nigel Stewart <nigels[]users sourceforge net>
** Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
** Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
** Copyright (C) 2002, Lev Povalahev
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
** 
** * Redistributions of source code must retain the above copyright notice, 
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, 
**   this list of conditions and the following disclaimer in the documentation 
**   and/or other materials provided with the distribution.
** * The mName of the author may be used to endorse or promote products 
**   derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
** THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "wglewInit.h"
#include <stdio.h>

#pragma comment(lib, "opengl32.lib")

void LogV(const char* msg, ...)
{
	va_list argptr;
	char string[32000];

	va_start(argptr, msg);
	
	vsnprintf(string, sizeof(string), msg, argptr);	// Knightmare- buffer overflow fix

	OutputDebugStringA(string);
	va_end(argptr);


}

static void CALLBACK _wglewCallbackDebug(GLenum source, GLenum type, GLuint id, GLenum severity,
										 GLsizei length, const GLchar* message, const void* userParam)
{
	userParam;
	length;

	LogV("\n---------------------opengl-callback-start------------\n");
	LogV("message: %s\n", message);
	LogV("source: %d\n", source);
	LogV("\ttype: ");
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		LogV("\tERROR\n");
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		LogV("\tDEPRECATED_BEHAVIOR\n");
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		LogV("\tUNDEFINED_BEHAVIOR\n");
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		LogV("\tPORTABILITY\n");
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		LogV("\tPERFORMANCE\n");
		break;
	case GL_DEBUG_TYPE_OTHER:
		LogV("\tOTHER\n");
		break;
	}

	LogV("\t\tid: %d\n", id);
	LogV("\t\tseverity: ");
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		LogV("LOW\n");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		LogV("MEDIUM\n");
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		LogV("HIGH\n");
		break;
	default:
		LogV("OTHER\n");
		break;

	}

	LogV("---------------------opengl-callback-end--------------\n");


}

#define ADD_ATTRIBi(a, b) do { \
    iAttrbList[iAttrbCount++] = (a); \
    iAttrbList[iAttrbCount++] = (b); \
    LogV("  - Int Attribute: 0x%x = %d\n", (a), (b)); \
} while(0)

#define ADD_ATTRIBf(a, b) do { \
    fAttrbList[iAttrbCount++] = (a); \
    fAttrbList[iAttrbCount++] = (b); \
    LogV("  - Float Attribute: 0x%x = %f\n", (a), (b)); \
} while(0)


static int wglewChoosePixelFormat(wglewContext* context, HDC hDC)
{

	GLfloat fAttrbList[32] = { 0 };
	GLint iAttrbList[32] = { 0 };
	GLint iCompatiblesFormats[32] = { 0 };

	LogV("=== Choosing Pixel Format ===\n");

	GLint values[2] = { 0 };
	iAttrbList[0] = WGL_NUMBER_PIXEL_FORMATS_ARB;
	iAttrbList[1] = 0;

	int maxSamples = 0;
	if (wglGetPixelFormatAttribivARB(hDC, 0, 0, 1, iAttrbList, values)) {
		int numPixelFormats = values[0];
		
		LogV("%d pixels format ===\n", numPixelFormats);
		for (int i = 0; i < numPixelFormats; i++) {
			iAttrbList[0] = WGL_SAMPLES_ARB;
			iAttrbList[1] = 0;
			if (wglGetPixelFormatAttribivARB(hDC, i + 1, 0, 1, iAttrbList, values)) {
				maxSamples = max(maxSamples, values[0]);
			}
		}

		LogV("Max sample count supported: %d\n", maxSamples);
	}

	while (true)
	{
		GLuint numFormats = 0;
		GLint iAttrbCount = 0;

		// Query maximum sample count if multisampling is supported
		if (WGLEW_ARB_multisample)
		{
			
		}

		if (WGLEW_ARB_pixel_format)
		{
			LogV("Trying WGL_ARB_pixel_format...\n");

			ADD_ATTRIBi(WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
			ADD_ATTRIBi(WGL_DOUBLE_BUFFER_ARB, GL_TRUE);
			ADD_ATTRIBi(WGL_ACCELERATION_ARB, context->noAcceleration ? WGL_NO_ACCELERATION_ARB : WGL_FULL_ACCELERATION_ARB);
			ADD_ATTRIBi(WGL_DRAW_TO_WINDOW_ARB, GL_TRUE);
			ADD_ATTRIBi(WGL_COLOR_BITS_ARB, context->colorBits);			
			ADD_ATTRIBi(WGL_DEPTH_BITS_ARB, context->depthBits);

			if (context->stencilBits)
				ADD_ATTRIBi(WGL_STENCIL_BITS_ARB, context->stencilBits);
			if (context->stereo)
				ADD_ATTRIBi(WGL_STEREO_ARB, GL_TRUE);
			if (context->sampleCount > 1) {
				ADD_ATTRIBi(WGL_SAMPLE_BUFFERS_ARB, GL_TRUE);
				ADD_ATTRIBi(WGL_SAMPLES_ARB, context->sampleCount);
			}

			if (wglChoosePixelFormatARB(hDC, iAttrbList, fAttrbList, 1, iCompatiblesFormats, &numFormats) && numFormats > 0) {
				LogV("Pixel format selected via ARB.\n");
				break;
			}
			else {
				LogV("wglChoosePixelFormatARB failed or returned 0 formats.\n");
			}
		}
		else if (WGLEW_EXT_pixel_format)
		{
			LogV("Trying WGL_EXT_pixel_format...\n");

			ADD_ATTRIBi(WGL_DOUBLE_BUFFER_EXT, GL_TRUE);
			ADD_ATTRIBi(WGL_DRAW_TO_WINDOW_EXT, GL_TRUE);
			ADD_ATTRIBi(WGL_ACCELERATION_EXT, context->noAcceleration ? WGL_NO_ACCELERATION_EXT : WGL_ACCELERATION_EXT);
			ADD_ATTRIBi(WGL_COLOR_BITS_EXT, context->colorBits);
			ADD_ATTRIBi(WGL_DEPTH_BITS_EXT, context->depthBits);

			if (context->sampleCount > 1) {
				ADD_ATTRIBi(WGL_SAMPLE_BUFFERS_EXT, GL_TRUE);
				ADD_ATTRIBi(WGL_SAMPLES_EXT, context->sampleCount);
			}

			if (wglChoosePixelFormatEXT(hDC, iAttrbList, fAttrbList, 1, iCompatiblesFormats, &numFormats) && numFormats > 0) {
				LogV("Pixel format selected via EXT.\n");
				break;
			}
			else {
				LogV("wglChoosePixelFormatEXT failed or returned 0 formats.\n");
			}
		}
		else
		{
			LogV("Falling back to legacy ChoosePixelFormat...\n");

			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA,
				(BYTE)context->colorBits,
				0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0,
				(BYTE)context->depthBits,
				(BYTE)context->stencilBits,
				0,
				PFD_MAIN_PLANE,
				0, 0, 0, 0
			};

			iCompatiblesFormats[0] = ChoosePixelFormat(hDC, &pfd);

			if (iCompatiblesFormats[0]) {
				LogV("Pixel format selected via legacy ChoosePixelFormat.\n");
				break;
			}
			else {
				LogV("ChoosePixelFormat failed.\n");
			}
		}

		// Fallback logic
		LogV("Pixel format selection failed. Trying fallback adjustments...\n");
		if (context->stereo) {
			context->stereo = 0;
			LogV("Disabling stereo and retrying.\n");
		}
		else if (context->sampleCount) {
			context->sampleCount = 0;
			LogV("Disabling multisampling and retrying.\n");
		}
		else if (context->stencilBits) {
			context->stencilBits = 0;
			LogV("Disabling stencil buffer and retrying.\n");
		}
		else if (context->depthBits == 24) {
			context->depthBits = 16;
			LogV("Reducing depth bits to 16 and retrying.\n");
		}
		else if (context->colorBits == 32) {
			context->colorBits = 16;
			LogV("Reducing color bits to 16 and retrying.\n");
		}
		else {
			LogV("No more fallbacks available. Aborting pixel format selection.\n");
			break;
		}
	}

	LogV("Final selected format: %d\n", iCompatiblesFormats[0]);
	return iCompatiblesFormats[0];
}

int wglewDescribePixelFormat(wglewContext* context, HDC hDC)
{
	DescribePixelFormat(hDC, context->pixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &context->pixelFormatDescriptor);
	return 0;
}


HGLRC wglewCreateContext(wglewContext* context, HDC hDC)
{
	HGLRC ret;
	
	if (context->majorVersion >= 3)
	{
		int iAttrbCount = 0;
		GLint iAttrbList[32] = { 0 };

		ADD_ATTRIBi(WGL_CONTEXT_MAJOR_VERSION_ARB, context->majorVersion);
		ADD_ATTRIBi(WGL_CONTEXT_MINOR_VERSION_ARB, context->minorVersion);

		ADD_ATTRIBi(WGL_CONTEXT_FLAGS_ARB, (context->debugBits ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) | (context->compatibilityBits ? 0 : WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB));
		ADD_ATTRIBi(WGL_CONTEXT_PROFILE_MASK_ARB, context->compatibilityBits ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB);


		if (!SetPixelFormat(hDC, context->pixelFormatIndex, &context->pixelFormatDescriptor))				// Are We Able To Set The Pixel Format?
		{
			goto onError;
		}

		auto hGLRCTemp = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hGLRCTemp);

		ret = wglCreateContextAttribsARB(hDC, 0, iAttrbList);
		if (ret && context->debugBits) {

			if (GLEW_KHR_debug)
			{
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(_wglewCallbackDebug, NULL);
				GLuint unusedIds = 0;
				glDebugMessageControl(
					GL_DONT_CARE,
					GL_DONT_CARE,
					GL_DONT_CARE,
					0,
					&unusedIds,
					true);				
			}
		}
		

		wglDeleteContext(hGLRCTemp);
	}
	else
	{
		ret = wglCreateContext(hDC);

	}

	

	return ret;

onError:

	return NULL;
		


}
int wglewInitContext(wglewContext* context)
{

	// Create dummy windows and class. Access to WGL extensions
	WNDCLASS wcTemp;
	HWND hWndTemp = 0;
	HDC hDCTemp = 0;
	HGLRC hGLRCTemp;
	int pfdTemp;
	ZeroMemory(&wcTemp, sizeof(wcTemp));

	wcTemp.hInstance = GetModuleHandle(NULL);
	wcTemp.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcTemp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcTemp.lpfnWndProc = DefWindowProc;
#ifdef UNICODE
	wcTemp.lpszClassName = L"DummyGL2";
#else
	wcTemp.lpszClassName = "DummyGL2";
#endif

	if (!RegisterClass(&wcTemp))
	{
		return -1;
	}

	hWndTemp = CreateWindowEx(0, wcTemp.lpszClassName, wcTemp.lpszClassName, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
		0, 0, wcTemp.hInstance, NULL);

	if (!hWndTemp)
	{
		return -2;
	}

	hDCTemp = GetDC(hWndTemp);
	if (!hWndTemp)
	{
		return -3;
	}

	pfdTemp = ChoosePixelFormat(hDCTemp, &context->pixelFormatDescriptor);
	if (pfdTemp == 0)
	{
		ReleaseDC(hWndTemp, hDCTemp);
		if (!UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance))
		{
		}
		DestroyWindow(hWndTemp);
		return -4;
	}
	if (!SetPixelFormat(hDCTemp, pfdTemp, &context->pixelFormatDescriptor))
	{
		ReleaseDC(hWndTemp, hDCTemp);
		if (!UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance))
		{
		}
		DestroyWindow(hWndTemp);
		return -5;
	}
	hGLRCTemp = wglCreateContext(hDCTemp);
	wglMakeCurrent(hDCTemp, hGLRCTemp);
	glewInit();
		
	context->pixelFormatIndex = wglewChoosePixelFormat(context, hDCTemp);
	if (context->pixelFormatIndex < 0)
	{
		wglDeleteContext(hGLRCTemp);
		ReleaseDC(hWndTemp, hDCTemp);
		if (!UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance))
		{
		}
		DestroyWindow(hWndTemp);
		return -6;
	}
	DescribePixelFormat(hDCTemp, context->pixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &context->pixelFormatDescriptor);
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWndTemp, hDCTemp);
	wglDeleteContext(hGLRCTemp);
	UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance);
	DestroyWindow(hWndTemp);
		

	return 0;


}

#if 0


// Choose best pixel format
GLint wglCoreChoosePixelFormat(HDC pHdc, GLint *pBpp, GLint *pDepth, GLint *pStencil, GLint *pDbl, GLint *pAcc, GLint *pDib)
{
	GLint wbpp = pBpp == NULL ? -1 : *pBpp;
	GLint wdepth = pDepth == NULL ? 16 : *pDepth;
	GLint wstencil = pStencil == NULL ? 0 : *pDepth;
	GLint wdbl = pDbl == NULL ? -1 : *pDbl;
	GLint wacc = pAcc == NULL ? 1 : *pAcc;
	GLint wdib = pDib == NULL ? -1 : *pDib;
	GLint num;
	GLuint maxqual = 0;
	GLint maxindex = 0;
	GLint max_bpp, max_depth, max_dbl, max_acc, max_dib, max_stencil;
	GLint i;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;

	num = DescribePixelFormat(pHdc, 1, sizeof(pfd), &pfd);
	if (num == 0)
		return 0;

	for (i = 1; i <= num; i++)
	{
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		DescribePixelFormat(pHdc, i, sizeof(pfd), &pfd);
		{
			GLint bpp = pfd.cColorBits;
			GLint depth = pfd.cDepthBits;
			GLint stencil = pfd.cStencilBits;

			GLboolean pal = !!(pfd.iPixelType == PFD_TYPE_COLORINDEX);
			GLboolean mcd = !!((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean soft = !!((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean icd = (!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean opengl = !!(pfd.dwFlags & PFD_SUPPORT_OPENGL);
			GLboolean window = !!(pfd.dwFlags & PFD_DRAW_TO_WINDOW);
			GLboolean bitmap = !!(pfd.dwFlags & PFD_DRAW_TO_BITMAP);
			GLboolean dbuff = !!(pfd.dwFlags & PFD_DOUBLEBUFFER);

			GLuint q = 0;

			if (opengl && window)
				q += 0x10000;
			if (wdepth == -1 || (wdepth > 0 && depth > 0))
				q += 0x8000;
			if (wstencil == -1 || (wstencil > 0 && stencil > 0))
				q += 0x4000;
			if (wdbl == -1 || (wdbl == 0 && !dbuff) || (wdbl == 1 && dbuff))
				q += 0x2000;
			if (wacc == -1 || (wacc == 0 && soft) || (wacc == 1 && (mcd || icd)))
				q += 0x1000;
			if (mcd || icd)
				q += 0x0040;
			if (icd)
				q += 0x0002;
			if (wbpp == -1 || (wbpp == bpp))
				q += 0x0800;
			if ((wdib == -1) || (wdib == 0 && !bitmap) || (wdbl == 1 && bitmap))
				q += 0x10000;
			if (bpp >= 16)
				q += 0x0020;
			if (bpp == 16)
				q += 0x0008;
			if (wdepth == -1 || (wdepth == depth))
				q += 0x0400;

			if (depth >= 16)
				q += 0x0010;
			if (depth == 16)
				q += 0x0004;
			if (!pal)
				q += 0x0080;

			if (q > maxqual)
			{
				maxqual = q;
				maxindex = i;
				max_bpp = bpp;
				max_depth = depth;
				max_stencil = stencil;
				max_dbl = dbuff ? 1 : 0;
				max_acc = soft ? 0 : 1;
				max_dib = bitmap ? 1 : 0;
			}
		}
	}
	if (maxindex == 0)
		return maxindex;

	if (pBpp != NULL)
		*pBpp = max_bpp;
	if (pDepth != NULL)
		*pDepth = max_depth;
	if (pStencil != NULL)
		*pStencil = max_stencil;
	if (pDbl != NULL)
		*pDbl = max_dbl;
	if (pAcc != NULL)
		*pAcc = max_acc;
	if (pDib != NULL)
		*pDib = max_dib;

	return maxindex;
}

#endif